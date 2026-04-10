#include "ExtractSequenceWidget.h"

ExtractSequenceWidget::ExtractSequenceWidget(const Media& media, QWidget *parent, int startTime)
    : QDialog{parent}, m_media(media)
{
    m_startTime = startTime;
    m_endTime = startTime+10000;
    m_thumbnailPendingTime = 50;
    m_isExec = false;
    int duration = media.duration();
    m_startTimeEditor = new TimeEditor(this, startTime, duration, 0, duration, media.fps());
    m_endTimeEditor = new TimeEditor(this, startTime+10000, duration, startTime, duration, media.fps());
    m_thumbnailStartTimer = new QTimer(this);
    m_thumbnailStartTimer->setSingleShot(true);
    connect(m_thumbnailStartTimer, &QTimer::timeout, this, [this](){ requestStartFrameDisplay(); });
    m_thumbnailEndTimer = new QTimer(this);
    m_thumbnailEndTimer->setSingleShot(true);
    connect(m_thumbnailEndTimer, &QTimer::timeout, this, [this](){ requestEndFrameDisplay(); });
    createButtons();
    initUiLayout();
}

void ExtractSequenceWidget::createButtons()
{
    m_thumbnailWorker = new ThumbnailWorker(this);
    m_okButton = new QPushButton("OK");
    m_cancelButton = new QPushButton(PrefManager::instance().getText("cancel_action"));
    m_startFrameDisplay = new QLabel();
    m_endFrameDisplay = new QLabel();
    connect(m_thumbnailWorker, &ThumbnailWorker::thumbnailReady, this, &ExtractSequenceWidget::onThumbnailReady);
    m_thumbnailWorker->start();
    connect(m_okButton, &QPushButton::released, this, &ExtractSequenceWidget::confirmExtraction);
    connect(m_cancelButton, &QPushButton::released, this, &QDialog::reject);
    connect(m_startTimeEditor, &TimeEditor::timeChanged, m_endTimeEditor, &TimeEditor::onMinTimeChanged);
    connect(m_startTimeEditor, &TimeEditor::timeChanged, this, &ExtractSequenceWidget::onStartTimeChanged);
    connect(m_endTimeEditor, &TimeEditor::timeChanged, this, &ExtractSequenceWidget::onEndTimeChanged);
}

void ExtractSequenceWidget::initUiLayout()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *timeSelectionLayout = new QHBoxLayout();
    QVBoxLayout *startTimeSelectionLayout = new QVBoxLayout();
    QLabel *startLabel = new QLabel("<b>" + PrefManager::instance().getText("extract_start_label") + " :<b>");

    m_startFrameDisplay->setScaledContents(true);
    m_endFrameDisplay->setScaledContents(true);
    m_startFrameDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_endFrameDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_startFrameDisplay->setPixmap(QPixmap(720, 480));
    m_endFrameDisplay->setPixmap(QPixmap(720, 480));

    startTimeSelectionLayout->addWidget(startLabel);
    startTimeSelectionLayout->addWidget(m_startFrameDisplay);
    startTimeSelectionLayout->addWidget(m_startTimeEditor);

    QVBoxLayout *endTimeSelectionLayout = new QVBoxLayout();
    QLabel *endLabel = new QLabel("<b>" + PrefManager::instance().getText("extract_end_label") + " :<b>");
    endTimeSelectionLayout->addWidget(endLabel);
    endTimeSelectionLayout->addWidget(m_endFrameDisplay);
    endTimeSelectionLayout->addWidget(m_endTimeEditor);

    timeSelectionLayout->addLayout(startTimeSelectionLayout);
    timeSelectionLayout->addLayout(endTimeSelectionLayout);

    QHBoxLayout *confirmLayout = new QHBoxLayout();
    confirmLayout->addStretch();
    confirmLayout->addWidget(m_okButton);
    confirmLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(timeSelectionLayout);
    mainLayout->addLayout(confirmLayout);

    requestStartFrameDisplay();
    requestEndFrameDisplay();
}

void ExtractSequenceWidget::requestStartFrameDisplay()
{
    m_thumbnailWorker->keepNQueue(2); // Pour clear la queue sauf 2 éléments (clearQueue empêche parfois d'afficher les frames sur un des deux)
    m_thumbnailWorker->requestThumbnail(0, m_startTime, 0, m_media.filePath(), {720, 480});
}

void ExtractSequenceWidget::requestEndFrameDisplay()
{
    m_thumbnailWorker->keepNQueue(2);
    m_thumbnailWorker->requestThumbnail(1, m_endTime, 0, m_media.filePath(), {720, 480});
}

void ExtractSequenceWidget::onThumbnailReady(int requestId, const QImage& image)
{
    QPixmap pixmap = QPixmap::fromImage(image);
    switch(requestId){
    case 0:
        m_startFrameDisplay->setPixmap(pixmap);
        break;
    case 1:
        m_endFrameDisplay->setPixmap(pixmap);
        break;
    }
}

void ExtractSequenceWidget::onStartTimeChanged(const int newTime)
{
    m_startTime = newTime;
    if(!m_thumbnailStartTimer->isActive())
        m_thumbnailStartTimer->start(m_thumbnailPendingTime);
}

void ExtractSequenceWidget::onEndTimeChanged(const int newTime)
{
    m_endTime = newTime;
    if(!m_thumbnailEndTimer->isActive())
        m_thumbnailEndTimer->start(m_thumbnailPendingTime);
}

void ExtractSequenceWidget::confirmExtraction()
{
    auto& prefManager = PrefManager::instance();
    QString saveSequencePath = QFileDialog::getSaveFileName(this, tr("Extract sequence"), prefManager.getPref("Paths", "lp_extract_sequence"));
    if(saveSequencePath != ""){
        QProcess* sequenceExtractor = SequenceExtractionHelper::extractSequence(m_media.filePath(), m_startTime, m_endTime, saveSequencePath + '.' + m_media.fileExtension());
        connect(sequenceExtractor, &QProcess::finished, this, &QDialog::accept);
        QFileInfo fileInfo (saveSequencePath);
        prefManager.setPref("Paths", "lp_extract_sequence", fileInfo.absolutePath());
    }
}
