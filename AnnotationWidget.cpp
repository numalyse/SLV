#include "AnnotationWidget.h"
#include "IconHelper.h"
#include "PrefManager.h"
#include "TimeFormatter.h"
#include "Project/ProjectManager.h"
#include "SignalManager.h"

#include <QMouseEvent>
#include <QGuiApplication>
#include <QStyleHints>
#include <QAbstractTextDocumentLayout>
#include <QtMath>
#include <QApplication>

AnnotationWidget::AnnotationWidget(QWidget *parent, const Annotation &annotation)
    : QWidget{parent}, m_annotation(annotation)
{
    m_isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

#ifdef Q_OS_MAC
    QColor colorBtn = qApp->palette().color(QPalette::Button);
    QColor enhancedColor = m_isDarkMode ? colorBtn.lighter(150) : colorBtn.darker(150);
    m_palbtnColor = m_isDarkMode ? enhancedColor : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? QString(enhancedColor.name()) : "palette(mid)";
#else
    m_palbtnColor = m_isDarkMode ? "palette(button)" : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? "palette(button)" : "palette(mid)";
#endif

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(3,4,3,4);
    mainLayout->setSpacing(2);

    m_colorBar = new QFrame();
    m_colorBar->setFixedWidth(6);
    mainLayout->addWidget(m_colorBar);

    m_mediaThumbnailLabel = new QLabel();
    m_mediaThumbnailLabel->setFixedSize(m_thumbnailSize);
    if (QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
        m_mediaThumbnailImage = new QPixmap(":/icons/image_preview_white");
    } else {
        m_mediaThumbnailImage = new QPixmap(":/icons/image_preview");
    }
    m_mediaThumbnailLabel->setPixmap(m_mediaThumbnailImage->scaled(20,20, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_mediaThumbnailLabel->setAlignment(Qt::AlignCenter);

    // column holding the top row and the note editor below it
    m_contentLayout = new QVBoxLayout();
    m_contentLayout->setSpacing(2);

    m_topRowLayout = new QHBoxLayout();
    m_topRowLayout->setSpacing(2);

    // for start / end timecodes
    m_timeLayout = new QVBoxLayout();
    m_timeLayout->setSpacing(2);

    m_startLabel = new QLabel("00:00:00.00");
    m_endLabel = new QLabel("00:00:00.00");

    // fixed size so they stay in place when switching between display and edit mode
    for(QLabel* timeLabel : {m_startLabel, m_endLabel}){
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setFixedSize(70, 20);
    }

    m_startEdit = new QLineEdit();
    m_endEdit = new QLineEdit();
    for(QLineEdit* timeEdit : {m_startEdit, m_endEdit}){
        timeEdit->setInputMask("99:99:99.99");
        timeEdit->setAlignment(Qt::AlignCenter);
        timeEdit->setFixedSize(70, 20);
        timeEdit->hide();
    }

    m_timeLayout->addWidget(m_startLabel);
    m_timeLayout->addWidget(m_endLabel);
    m_timeLayout->addWidget(m_startEdit);
    m_timeLayout->addWidget(m_endEdit);

    m_topRowLayout->addWidget(m_mediaThumbnailLabel);
    m_topRowLayout->addLayout(m_timeLayout);
    // the top row keeps the same height (42) in every mode, since the note lives below it,
    // so the default vertical centering keeps the thumbnail aligned with the timecodes

    // bloc name / note
    m_infoLayout = new QVBoxLayout();
    m_infoLayout->setSpacing(2);

    m_nameLabel = new QLabel();
    m_nameLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    m_noteLabel = new QLabel();
    m_noteLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    m_nameEdit = new QLineEdit();
    m_nameEdit->hide();

    m_infoLayout->addWidget(m_nameLabel);
    m_infoLayout->addWidget(m_noteLabel);
    m_infoLayout->addWidget(m_nameEdit);

    m_topRowLayout->addLayout(m_infoLayout, 1);
    m_topRowLayout->addSpacing(10);

    m_updateBtn = new ToolbarButton(this, "edit_white", PrefManager::instance().getText("edit"));

    connect(m_updateBtn, &ToolbarButton::clicked, this, [this](){
        if(m_mode != Mode::Edited)
            setMode(Mode::Edited);
    });

    // fixed height so update btn stays in place when switching between display and edit mode
    QWidget* updateBtnContainer = new QWidget(this);
    updateBtnContainer->setFixedHeight(42);
    QVBoxLayout* updateBtnLayout = new QVBoxLayout(updateBtnContainer);
    updateBtnLayout->setContentsMargins(0,0,0,0);
    updateBtnLayout->addStretch();
    updateBtnLayout->addWidget(m_updateBtn);
    updateBtnLayout->addStretch();

    m_topRowLayout->addWidget(updateBtnContainer);

    m_contentLayout->addLayout(m_topRowLayout);

    m_noteEdit = new QTextEdit();
    m_noteEdit->setFixedHeight(80);
    m_noteEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_noteEdit->hide();
    m_contentLayout->addWidget(m_noteEdit);

    // grow the note editor with its content, queued so the resize happens later once everything is set
    connect(m_noteEdit->document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged,
            this, &AnnotationWidget::adjustNoteEditHeight, Qt::QueuedConnection);

    // color / confirm / cancel row 
    QHBoxLayout *editActionsLayout = new QHBoxLayout();
    editActionsLayout->setSpacing(8);

    m_colorBtn = new QPushButton();
    m_colorBtn->setFixedSize(60, 24);
    m_colorBtn->setToolTip(PrefManager::instance().getText("tooltip_color_tool"));
    m_colorBtn->hide();
    connect(m_colorBtn, &QPushButton::clicked, this, &AnnotationWidget::openColorPicker);

    m_confirmBtn = new ToolbarButton(this, "check_white", PrefManager::instance().getText("confirm"));
    m_cancelBtn = new ToolbarButton(this, "cancel_white", PrefManager::instance().getText("cancel_action"));
    m_deleteBtn = new ToolbarButton(this, "delete_white", PrefManager::instance().getText("delete"));

    m_confirmBtn->hide();
    m_cancelBtn->hide();
    m_deleteBtn->hide();

    connect(m_confirmBtn, &QPushButton::released, this, [this](){
        auto* annotManager = ProjectManager::instance().annotationManager();
        if( annotManager && annotManager->hasConflict(editedAnnotation()))
        {
            m_startEdit->setStyleSheet(QString("border: 2px solid tomato; border-radius: 4px;"));
            m_endEdit->setStyleSheet(QString("border: 2px solid tomato; border-radius: 4px;"));
        } else
            applyEdits();
    });
    connect(m_cancelBtn, &QPushButton::released, this, [this]{ setMode(Mode::Minimized); });
    connect(m_deleteBtn, &QPushButton::released, this, [this]{ emit removeAnnotationRequested(m_annotation.id); });

    editActionsLayout->addWidget(m_colorBtn);
    editActionsLayout->addStretch();
    editActionsLayout->addWidget(m_confirmBtn);
    editActionsLayout->addWidget(m_cancelBtn);
    editActionsLayout->addWidget(m_deleteBtn);

    m_contentLayout->addLayout(editActionsLayout);

    mainLayout->addLayout(m_contentLayout, 1);

    m_clickTimer = new QTimer(this);
    m_clickTimer->setSingleShot(true);
    m_clickTimer->setInterval(QApplication::doubleClickInterval()); // double click detection speed based on os
    connect(m_clickTimer, &QTimer::timeout, this, [this]{
        emit SignalManager::instance().timelineSetPosition(m_annotation.start);
    });

    initStyle();
    refreshContent();
}

void AnnotationWidget::initStyle()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(50);
    setAttribute(Qt::WA_StyledBackground, true);
    setContentsMargins(0,0,0,0);
    setStyleSheet("AnnotationWidget{border-style: solid; border: 1px solid " + m_palbtnColorStr + "; border-radius: 4px;}");

    m_updateBtn->setFixedSize(24,24);
    m_updateBtn->setMaximumWidth(20);

    m_deleteBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "QPushButton:hover{"
        "   background-color: tomato;"
        "   border: 1px solid " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}");
}

void AnnotationWidget::refreshContent()
{
    m_colorBar->setStyleSheet("background-color: " + m_annotation.color.name() + "; border-radius: 2px;");

    double fps = currentFps();
    QString start = TimeFormatter::msToHHMMSSFF(m_annotation.start, fps);
    QString end = TimeFormatter::msToHHMMSSFF(m_annotation.end, fps);
    m_startLabel->setText(start);
    m_endLabel->setText(end);
    
    m_nameLabel->setText(m_annotation.name);
    m_noteLabel->setText(m_annotation.note);
    if (m_mode == Mode::Minimized)
        m_noteLabel->setToolTip(m_annotation.note);

    // keep the edit fields in sync with the annotation
    if (m_mode == Mode::Edited) {
        m_startEdit->setText(start);
        m_endEdit->setText(end);
        m_nameEdit->setText(m_annotation.name);
        m_noteEdit->setPlainText(m_annotation.note);
        m_editColor = m_annotation.color;
        // remove red borders (from when a conflict occurs)
        m_startEdit->setStyleSheet(QString());
        m_endEdit->setStyleSheet(QString());
        updateColorButton();
    }
}

void AnnotationWidget::setMode(Mode mode)
{

    m_mode = mode;
    bool isEditMode = (m_mode == Mode::Edited);

    if (isEditMode)
        refreshContent(); // populates the edit fields since m_mode is already Edited

    // in extended mode the note moves under the top row, in the note editor slot,
    // so display and edit mode are the same
    if (m_mode == Mode::Extended) {
        if (m_contentLayout->indexOf(m_noteLabel) == -1) {
            m_infoLayout->removeWidget(m_noteLabel);
            m_contentLayout->insertWidget(1, m_noteLabel);
        }
        m_noteLabel->setContentsMargins(5, 3, 5, 3);
        m_noteLabel->setToolTip("");
        m_noteLabel->setWordWrap(true);
        // matches the note editor frame + document margins so the text lines up
    } else {
        if (m_infoLayout->indexOf(m_noteLabel) == -1) {
            m_contentLayout->removeWidget(m_noteLabel);
            m_infoLayout->insertWidget(1, m_noteLabel);
        }
        m_noteLabel->setContentsMargins(0, 0, 0, 0);
        m_noteLabel->setToolTip(m_annotation.note);
        m_noteLabel->setWordWrap(false);
    }

    m_startLabel->setVisible(!isEditMode);
    m_endLabel->setVisible(!isEditMode);
    m_nameLabel->setVisible(!isEditMode);
    m_noteLabel->setVisible(!isEditMode);

    m_startEdit->setVisible(isEditMode);
    m_endEdit->setVisible(isEditMode);
    m_nameEdit->setVisible(isEditMode);
    m_noteEdit->setVisible(isEditMode);
    m_colorBtn->setVisible(isEditMode);
    m_confirmBtn->setVisible(isEditMode);
    m_cancelBtn->setVisible(isEditMode);
    m_deleteBtn->setVisible(isEditMode);

    if (isEditMode || m_mode == Mode::Extended) {
        // release the fixed height
        setMinimumHeight(0);
        setMaximumHeight(QWIDGETSIZE_MAX);
        adjustNoteEditHeight();
    } else {
        setFixedHeight(50);
    }
}

Annotation AnnotationWidget::editedAnnotation() const
{
    Annotation edited = m_annotation;
    double fps = currentFps();
    edited.start = TimeFormatter::HHMMSSFFToMs(m_startEdit->text(), fps);
    edited.end = TimeFormatter::HHMMSSFFToMs(m_endEdit->text(), fps);
    edited.name = m_nameEdit->text();
    edited.note = m_noteEdit->toPlainText();
    edited.color = m_editColor;
    return edited;
}

void AnnotationWidget::applyEdits()
{
    Annotation updated = editedAnnotation();
    setMode(Mode::Minimized);
    emit updateAnnotationRequested(updated);
}

void AnnotationWidget::openColorPicker()
{
    IconHelper::execColorPickerMenu(this, m_colorBtn, [this](const QColor& color){
        m_editColor = color;
        updateColorButton();
    });
}

void AnnotationWidget::updateColorButton()
{
    m_colorBtn->setStyleSheet("QPushButton{"
        "   background-color: " + m_editColor.name() + ";"
        "   border: 1px solid palette(mid);"
        "   border-radius: 4px;"
        "}");
}

void AnnotationWidget::adjustNoteEditHeight()
{
    if (m_mode != Mode::Edited)
        return;

    int chrome = m_noteEdit->height() - m_noteEdit->viewport()->height();
    int docHeight = qCeil(m_noteEdit->document()->size().height());
    m_noteEdit->setFixedHeight(qMax(80, docHeight + chrome));
}

void AnnotationWidget::updateAnnotation(const Annotation &annotation)
{
    m_annotation = annotation;
    refreshContent();
    refreshThumbnail();
}

void AnnotationWidget::refreshThumbnail()
{
    if (m_thumbnailTimeMs == m_annotation.start)
        return; // start is not updated, do not request thumbnail

    m_thumbnailTimeMs = m_annotation.start;
    emit thumbnailRequested(m_annotation.id, m_annotation.start, m_thumbnailSize);
}

void AnnotationWidget::setThumbnail(const QImage &image)
{
    m_mediaThumbnailLabel->setPixmap(QPixmap::fromImage(image));
}

double AnnotationWidget::currentFps() const
{
    Media* media = ProjectManager::instance().media();
    if (media && media->fps() > 0.0)
        return media->fps();
    return 1.0;
}

void AnnotationWidget::enterEvent(QEnterEvent *event)
{
    setStyleSheet("AnnotationWidget{border-style: solid; border: 2px solid " + m_palbtnColorStr + "; border-radius: 3px;}");
}

void AnnotationWidget::leaveEvent(QEvent *event)
{
    setStyleSheet("AnnotationWidget{border-style: solid; border: 1px solid " + m_palbtnColorStr + "; border-radius: 4px;}");
}

void AnnotationWidget::mouseReleaseEvent(QMouseEvent *event)
{    
    if(m_ignoreNextRelease){
        m_ignoreNextRelease = false;
        return;
    }

    m_clickTimer->start();
}

void AnnotationWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // prevent mouse release event from happening
    m_clickTimer->stop();
    m_ignoreNextRelease = true;
    
    if (m_mode == Mode::Minimized){
        setMode(Mode::Extended);
    }else if (m_mode == Mode::Extended){
        setMode(Mode::Minimized);
    }
}
