#include "SnapshotPopup.h"

#include "PrefManager.h"
#include "TimeFormatter.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QLabel>
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QDesktopServices>
#include <QUrl>
#include <QEvent>


/// @brief Elides a file path by removing leading segments until it fits within the specified width, using "..." to indicate omitted segments. 
/// The function preserves whole directory names and does not cut in the middle of a folder name (only if the last segment doesn't fit we elide it in the middle).
/// @param path 
/// @param fm 
/// @param maxWidth 
/// @return 
static QString elidePathOnSeparators(const QString& path, const QFontMetrics& fm, int maxWidth)
{
    if (fm.horizontalAdvance(path) <= maxWidth)
        return path;

    const QStringList parts = path.split('/', Qt::SkipEmptyParts);

    // we keep fewer and fewer trailing segments until it fits
    QString best;
    for (int i = parts.size() - 1; i >= 0; --i) {
        const QString candidate = ".../" + QStringList(parts.mid(i)).join('/');
        if (fm.horizontalAdvance(candidate) > maxWidth)
            break;
        best = candidate;
    }

    // if the smallest candidate is still too wide, we just elide the last segment in the middle
    if (best.isEmpty())
        return fm.elidedText(".../" + parts.last(), Qt::ElideMiddle, maxWidth);

    return best;
}

SnapshotPopup::SnapshotPopup(QWidget* anchor, const QString& filePath, int64_t vlcTime, double fps)
    // Qt::Tool so when we display the image, the popup doesn't go in front of it
    : QFrame(anchor, Qt::Tool | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint)
    , m_anchor(anchor)
{
    setAttribute(Qt::WA_TranslucentBackground);

    m_closeTimer = new QTimer(this);
    m_closeTimer->setSingleShot(true);
    m_closeTimer->setInterval(m_durationMs);
    connect(m_closeTimer, &QTimer::timeout, this, &SnapshotPopup::fadeOutAndClose);

    buildUi(filePath, vlcTime, fps);

    adjustSize();
    reposition();

    // detect mouse enter/leave in the popup to restart the auto-close timer
    installEventFilter(this);
    m_anchor->installEventFilter(this);
}

SnapshotPopup::~SnapshotPopup()
{
    if (m_anchor)
        m_anchor->removeEventFilter(this);
}

void SnapshotPopup::buildUi(const QString& filePath, int64_t vlcTime, double fps)
{
    // container for the card with rounded corners and shadow
    const QColor bgColor = palette().color(QPalette::Window);
    QFrame* card = new QFrame(this);
    card->setObjectName("snapshotCard");
    card->setStyleSheet(QString(
        "QFrame#snapshotCard {"
        "  background-color: %1;"
        "  border-radius: 12px;"
        "}").arg(bgColor.name()));

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(card);
    shadow->setBlurRadius(24);
    shadow->setOffset(0, 4);
    shadow->setColor(QColor(0, 0, 0, 160));
    card->setGraphicsEffect(shadow);

    QVBoxLayout* outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(12, 12, 12, 12);
    outerLayout->addWidget(card);

    QHBoxLayout* layout = new QHBoxLayout(card);
    layout->setContentsMargins(16, 14, 14, 14);
    layout->setSpacing(12);

    QLabel* thumbLabel = new QLabel(card);
    const int thumbHeight = 48; // fixed height and width is scaled to keep aspect ratio
    QPixmap thumb(filePath);
    if (!thumb.isNull()) {
        const qreal dpr = devicePixelRatioF();
        const int h = qRound(thumbHeight * dpr); // use devicePixelRatioF() to scale the thumbnail for high-DPI displays
        thumb = thumb.scaledToHeight(h, Qt::SmoothTransformation);
        const int w = thumb.width();

        // draw the thumbnail with rounded corners
        QPixmap rounded(w, h);
        rounded.setDevicePixelRatio(dpr);
        rounded.fill(Qt::transparent);
        QPainter thumbPainter(&rounded);
        thumbPainter.setRenderHint(QPainter::Antialiasing, true);
        QPainterPath clipPath;
        clipPath.addRoundedRect(0, 0, w, h, 6 * dpr, 6 * dpr);
        thumbPainter.setClipPath(clipPath);
        thumbPainter.drawPixmap(0, 0, thumb);
        thumbPainter.end();

        thumbLabel->setPixmap(rounded);
        thumbLabel->setFixedSize(qRound(w / dpr), thumbHeight);
    } else {
        thumbLabel->setFixedSize(thumbHeight, thumbHeight);
    }
    layout->addWidget(thumbLabel, 0, Qt::AlignVCenter);

    // column for the text (title, subtitle, folder)
    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(1);

    // text colors from the palette
    const QColor textColor = palette().color(QPalette::WindowText);
    QColor dimColor = textColor;
    dimColor.setAlphaF(0.6);
    const QString dimColorString = QString("rgba(%1, %2, %3, %4)")
        .arg(dimColor.red()).arg(dimColor.green()).arg(dimColor.blue()).arg(dimColor.alphaF());

    QLabel* titleLabel = new QLabel(PrefManager::instance().getText("snapshot_saved_title"), card);
    titleLabel->setStyleSheet(QString(
        "QLabel { color: %1; font-weight: 600; font-size: 13px; background: transparent; }")
        .arg(textColor.name()));
    textLayout->addWidget(titleLabel);

    // if the text is too long, we elide it with "..." at the start
    const int maxTextWidth = 200;

    QLabel* subtitleLabel = new QLabel(card);
    subtitleLabel->setStyleSheet(QString(
        "QLabel { color: %1; font-size: 11px; background: transparent; }").arg(dimColorString));
    subtitleLabel->setMaximumWidth(maxTextWidth);
    // only the file name is elided so the timecode stays visible even for long file names
    const QString baseName = QFileInfo(filePath).completeBaseName();
    const QString timecode = TimeFormatter::msToHHMMSSFF(vlcTime, fps);
    const QString separator = QStringLiteral(" • ");
    const QFontMetrics subtitleFm = subtitleLabel->fontMetrics();
    const int reservedWidth = subtitleFm.horizontalAdvance(separator + timecode);
    const QString elidedName = subtitleFm.elidedText(baseName, Qt::ElideRight, qMax(0, maxTextWidth - reservedWidth));
    subtitleLabel->setText(elidedName + separator + timecode);
    textLayout->addWidget(subtitleLabel);

    QLabel* folderLabel = new QLabel(card);
    folderLabel->setStyleSheet(QString(
        "QLabel { color: %1; font-size: 11px; background: transparent; }").arg(dimColorString));
    folderLabel->setMaximumWidth(maxTextWidth);

    const QString folderText = QFileInfo(filePath).absolutePath();
    folderLabel->setText(elidePathOnSeparators(folderText, folderLabel->fontMetrics(), maxTextWidth));
    folderLabel->setToolTip(folderText); // full path as tooltip

    textLayout->addWidget(folderLabel);

    layout->addLayout(textLayout, 1);

    ToolbarButton* openButton = new ToolbarButton(card, "open_in_new_white", PrefManager::instance().getText("open_screenshot_button"));
    connect(openButton, &ToolbarButton::clicked, this, [filePath]() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
    });
    layout->addWidget(openButton, 0, Qt::AlignVCenter);
}

void SnapshotPopup::showWithFade()
{
    setWindowOpacity(0.0);
    show();

    m_anim = new QPropertyAnimation(this, "windowOpacity", this);
    m_anim->setDuration(m_fadeMs);
    m_anim->setStartValue(0.0);
    m_anim->setEndValue(1.0);
    m_anim->setEasingCurve(QEasingCurve::OutCubic);
    m_anim->start();

    m_closeTimer->start();
}

void SnapshotPopup::fadeOutAndClose()
{
    m_anim = new QPropertyAnimation(this, "windowOpacity", this);
    m_anim->setDuration(m_fadeMs);
    m_anim->setStartValue(windowOpacity());
    m_anim->setEndValue(0.0);
    m_anim->setEasingCurve(QEasingCurve::InCubic);
    connect(m_anim, &QPropertyAnimation::finished, this, [this]() {
        hide();
        deleteLater();
    });
    m_anim->start();
}

void SnapshotPopup::reposition()
{
    if (!m_anchor) return;
    move(m_anchor->mapToGlobal(QPoint(m_anchor->width() - sizeHint().width() - 10, 10)));
}

bool SnapshotPopup::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == this) {
        if (event->type() == QEvent::Enter) {
            // mouse entered the popup: stop the auto-close timer and reset opacity to 1.0
            m_closeTimer->stop();
            if (m_anim) m_anim->stop();
            setWindowOpacity(1.0);
        } else if (event->type() == QEvent::Leave) {
            // mouse leaves the popup: restart the auto-close timer
            m_closeTimer->start();
        }
    }
    return QFrame::eventFilter(watched, event);
}
