#include "ExtractSequenceWidget.h"
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>

ExtractSequenceWidget::ExtractSequenceWidget(const Media& media, QWidget *parent, int startTime, int endTime)
    : QDialog{parent}, m_media(media)
{
    const int MIN_END_MARGIN = 100;       
    const int DEFAULT_DURATION = 10000; 

    int duration = media.duration();

    // limite le debut à duration - MIN_END_MARGIN
    if (startTime >= duration - MIN_END_MARGIN) {
        m_startTime = duration - MIN_END_MARGIN;
    } else {
        m_startTime = startTime;
    }

    // limite la fin à duration
    if (endTime < 0) {
        int defaultEndTime = m_startTime + DEFAULT_DURATION;
        if (defaultEndTime < duration) {
            m_endTime = defaultEndTime;
        } else {
            m_endTime = duration; 
        }
    } else {
        m_endTime = endTime;
    }

    m_thumbnailPendingTime = 50;
    m_isExec = false;
    m_startTimeEditor = new TimeEditor(this, startTime, duration, 0, duration, media.fps());
    m_endTimeEditor = new TimeEditor(this, m_endTime, duration, startTime, duration, media.fps());
    m_thumbnailStartTimer = new QTimer(this);
    m_thumbnailStartTimer->setSingleShot(true);
    connect(m_thumbnailStartTimer, &QTimer::timeout, this, [this](){ requestStartFrameDisplay(); });
    m_thumbnailEndTimer = new QTimer(this);
    m_thumbnailEndTimer->setSingleShot(true);
    connect(m_thumbnailEndTimer, &QTimer::timeout, this, [this](){ requestEndFrameDisplay(); });
    createButtons();
    initUiLayout();
    connect(this, &QDialog::finished, this, [this, parent](int res){ if(res == QDialog::Accepted){
            QMessageBox::information(parent, "", PrefManager::instance().getText("messagebox_extract_sequence_completed"));
        }
    });
}

void ExtractSequenceWidget::createButtons()
{
    m_thumbnailWorker = new ThumbnailWorker(this);
    m_okButton = new QPushButton(PrefManager::instance().getText("extract_sequence_action"));
    m_audioOnlyButton = new QPushButton(PrefManager::instance().getText("extract_sequence_audio_only_action"));
    m_cancelButton = new QPushButton(PrefManager::instance().getText("cancel_action"));
    m_startFrameDisplay = new AspectRatioPixmapLabel();
    m_startFrameDisplay->setAlignment(Qt::AlignCenter);
    m_endFrameDisplay = new AspectRatioPixmapLabel();
    m_endFrameDisplay->setAlignment(Qt::AlignCenter);
    connect(m_thumbnailWorker, &ThumbnailWorker::thumbnailReady, this, &ExtractSequenceWidget::onThumbnailReady);
    m_thumbnailWorker->start();
    connect(m_okButton, &QPushButton::released, this, [this](){
        confirmExtraction(SequenceExtractionHelper::ExtractionType::Original);
    });    
    connect(m_audioOnlyButton, &QPushButton::released, this, [this](){
        confirmExtraction(SequenceExtractionHelper::ExtractionType::AudioOnly);
    });
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

    m_thumbnailWidth = (m_media.sar() > 0 ) ? m_media.width() * m_media.sar() : m_media.width();
    m_thumbnailHeight = m_media.height();


    m_startFrameDisplay->setPixmap(QPixmap(m_thumbnailWidth, m_thumbnailHeight));
    m_endFrameDisplay->setPixmap(QPixmap(m_thumbnailWidth, m_thumbnailHeight));

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
    confirmLayout->addWidget(m_audioOnlyButton);
    confirmLayout->addWidget(m_cancelButton);

    mainLayout->addLayout(timeSelectionLayout);
    mainLayout->addLayout(confirmLayout);

    requestStartFrameDisplay();
    requestEndFrameDisplay();
}

void ExtractSequenceWidget::requestStartFrameDisplay()
{
    if(m_media.type() == MediaType::Video){
        m_thumbnailWorker->keepNQueue(2); // Pour clear la queue sauf 2 éléments (clearQueue empêche parfois d'afficher les frames sur un des deux)
        m_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ExtractSequence, 0, m_startTime, 0, m_media.filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, m_media.sar());
    }
}

void ExtractSequenceWidget::requestEndFrameDisplay()
{
    if(m_media.type() == MediaType::Video){
        m_thumbnailWorker->keepNQueue(2);
        m_thumbnailWorker->requestThumbnail(ThumbnailWorker::Requester::ExtractSequence, 1, m_endTime, 0, m_media.filePath(), {int(m_thumbnailWidth), int(m_thumbnailHeight)}, m_media.sar());
    }
}

void ExtractSequenceWidget::onThumbnailReady(ThumbnailWorker::Requester requester, int requestId, const QImage& image)
{
    if(requester != ThumbnailWorker::Requester::ExtractSequence) return;

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

void ExtractSequenceWidget::confirmExtraction(SequenceExtractionHelper::ExtractionType type)
{
    auto& prefManager = PrefManager::instance();
    QString saveSequencePath = QFileDialog::getSaveFileName(this, tr("Extract sequence"), prefManager.getPref("Paths", "lp_extract_sequence")
        + '/' + m_media.fileName()+"_"+TimeFormatter::fileFormatMsToHHMMSSFF(m_startTime, m_media.fps())+"_"+TimeFormatter::fileFormatMsToHHMMSSFF(m_endTime, m_media.fps()));
    if(saveSequencePath != ""){
        // On garde seulement la base du nom sans l'extension avec un split
        QProcess* sequenceExtractor = SequenceExtractionHelper::extractSequence(m_media.filePath(), m_startTime, m_endTime, saveSequencePath.split('.')[0] + '.' + m_media.fileExtension(), type);
        connect(sequenceExtractor, &QProcess::finished, this, &QDialog::accept);
        QFileInfo fileInfo (saveSequencePath);
        prefManager.setPref("Paths", "lp_extract_sequence", fileInfo.absolutePath());
    }
}
