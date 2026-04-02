#include "Toolbars/SimpleToolbar.h"
#include "TextManager.h"
#include "TimeFormatter.h"
#include "SignalManager.h"

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ToolbarButtons/ToolbarPopupButton.h" 

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include "SimpleToolbar.h"
#include <QGraphicsDropShadowEffect>
#include <QPushButton>

SimpleToolbar::SimpleToolbar(QWidget *parent) : Toolbar(parent)
{
    createTimeEdit();
    m_nameLabel = new QLabel("", this);
    createTimeTotBtn();

    createSlider();

    QVBoxLayout* volumeFrameLayout = new QVBoxLayout();

    m_volumeSlider = new QSlider(Qt::Vertical);
    m_volumeSlider->setRange(0,100);
    m_volumeSlider->setValue(100);
    m_volumeSlider->adjustSize();

    m_volumeLabel = new QLabel("100%");
    volumeFrameLayout->addWidget(m_volumeLabel);
    volumeFrameLayout->addWidget(m_volumeSlider);

    volumeFrameLayout->setAlignment(m_volumeSlider, Qt::AlignHCenter);
    volumeFrameLayout->setAlignment(m_volumeLabel, Qt::AlignHCenter);

    m_muteBtn = new ToolbarToggleHoverButton(
        this,
        volumeFrameLayout,
        false,
        "sound_off_white",
        TextManager::instance().get("tooltip_sound_on"),
        "sound_on_white",
        TextManager::instance().get("tooltip_sound_off")
    );
    m_muteBtn->setEnabled(true);
    
    QVBoxLayout* speedFrameLayout = new QVBoxLayout();

    QHBoxLayout* speedInfoLayout = new QHBoxLayout();
    speedFrameLayout->addLayout(speedInfoLayout);
    m_speedLabel = new QLabel("x1");
    QLabel* slowIcon = new QLabel();
    slowIcon->setPixmap(QPixmap(":/icons/slow_white").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    QLabel* fastIcon = new QLabel();
    fastIcon->setPixmap(QPixmap(":/icons/speed_white").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    speedInfoLayout->addWidget(slowIcon);
    speedInfoLayout->addStretch();
    speedInfoLayout->addWidget(m_speedLabel);
    speedInfoLayout->addStretch();
    speedInfoLayout->addWidget(fastIcon);

    QSlider* speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(0,6);
    speedSlider->setValue(3);
    speedSlider->setTickPosition(QSlider::TicksAbove);
    speedSlider->setTickInterval(1);

    speedFrameLayout->addWidget(speedSlider);

    m_speedBtn = new ToolbarToggleHoverButton(this, speedFrameLayout, false, "speed_white",  TextManager::instance().get("tooltip_speed"), "speed_white", TextManager::instance().get("tooltip_speed"));

    m_loopBtn = new ToolbarToggleButton(
        this,
        true,
        "loop_on.png",
        TextManager::instance().get("tooltip_loop_on"),
        "loop_off_white",
        TextManager::instance().get("tooltip_loop_off")
    );

    // Languages/Subtitles Popup display
    QVBoxLayout* langLayout = new QVBoxLayout();

    // Languages
    QHBoxLayout* audioLangLayout = new QHBoxLayout();
    langLayout->addLayout(audioLangLayout);
    QLabel* audioLangLabel = new QLabel();
    audioLangLabel->setStyleSheet("border:none;");
    audioLangLabel->setText(TextManager::instance().get("languages"));

    m_audioLangComboBox = new QComboBox(this);
    audioLangLayout->addWidget(audioLangLabel);
    audioLangLayout->addWidget(m_audioLangComboBox);
    connect(m_audioLangComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SimpleToolbar::setAudioTrack);
    //connect(m_audioLangComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){setAudioTrack(index, true);});

    // Subtitles
    QHBoxLayout* subLangLayout = new QHBoxLayout();
    langLayout->addLayout(subLangLayout);
    QLabel* subLangLabel = new QLabel();
    subLangLabel->setStyleSheet("border:none;");
    subLangLabel->setText(TextManager::instance().get("subtitles"));

    m_subLangComboBox = new QComboBox(this);
    subLangLayout->addWidget(subLangLabel);
    subLangLayout->addWidget(m_subLangComboBox);
    connect(m_subLangComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SimpleToolbar::setSubtitlesTrack);

    m_langBtn = new ToolbarPopupButton(this, langLayout, "lang_white", TextManager::instance().get("tooltip_lang"));

    m_removePlayerBtn = new ToolbarButton(this, "remove_media_white", TextManager::instance().get("tooltip_delete_player"));
    m_duplicatePlayerBtn = new ToolbarButton(this, "duplicate_media_white", TextManager::instance().get("tooltip_duplicate_player"));

    connect(m_duplicatePlayerBtn, &ToolbarButton::clicked, this,  &SimpleToolbar::duplicatePlayerAction);
    connect(m_removePlayerBtn, &ToolbarButton::clicked, this, &SimpleToolbar::removePlayerRequest);
    connect(m_muteBtn, &ToolbarToggleHoverButton::stateActivated, this, &SimpleToolbar::enableMuteRequest);
    connect(m_muteBtn, &ToolbarToggleHoverButton::stateDeactivated, this, &SimpleToolbar::disableMuteRequest);
    connect(m_volumeSlider, &QSlider::valueChanged, this, &SimpleToolbar::volumeChanged);
    connect(speedSlider, &QSlider::valueChanged, this, &SimpleToolbar::speedChanged);
    connect(m_loopBtn, &ToolbarToggleButton::stateActivated, this, &SimpleToolbar::enableLoopModeRequest);
    connect(m_loopBtn, &ToolbarToggleButton::stateDeactivated, this, &SimpleToolbar::disableLoopModeRequest);

    setDefaultUI();
    disableButtons();
}

void SimpleToolbar::setFullscreenUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    // Créer un layout quand on est en fullscreen
}


void SimpleToolbar::setDefaultUI()
{
    if (layout() != nullptr) {
        delete layout();
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(5,5,5,5);
    mainLayout->setSpacing(1);

    QHBoxLayout* timecodeLayout = new QHBoxLayout();
    timecodeLayout->addWidget(m_timeEdit, 1, Qt::AlignLeft);
    timecodeLayout->addWidget(m_nameLabel, 1, Qt::AlignCenter);
    timecodeLayout->addWidget(m_durationBtn, 1, Qt::AlignRight);
    mainLayout->addLayout(timecodeLayout);

    mainLayout->addWidget(m_slider);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0,0,0,0);
    buttonLayout->setSpacing(1);
    buttonLayout->addWidget(m_muteBtn);
    buttonLayout->addWidget(m_langBtn);
    buttonLayout->addSpacing(m_langBtn->width());
    buttonLayout->addSpacing(m_langBtn->width());
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_speedBtn);
    buttonLayout->addWidget(m_stopBtn);
    //buttonLayout->addWidget(m_slowDownBtn);
    buttonLayout->addWidget(m_playPauseBtn);
    //buttonLayout->addWidget(m_speedUpBtn);

    buttonLayout->addWidget(m_ejectBtn);
    buttonLayout->addWidget(m_loopBtn);

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_screenshotBtn);
    buttonLayout->addWidget(m_duplicatePlayerBtn);
    buttonLayout->addWidget(m_removePlayerBtn);
    buttonLayout->addWidget(m_fullscreenBtn);
    mainLayout->addLayout(buttonLayout);


}

void SimpleToolbar::resetSlider()
{
    m_timeEdit->setText(TimeFormatter::msToHHMMSSFF(0,1));
    m_durationBtn->setText(TimeFormatter::msToHHMMSSFF(0,1));
    m_slider->setRange(0,0);
    m_slider->setValue(0);
}

void SimpleToolbar::stopSlider()
{
    m_timeEdit->setText(TimeFormatter::msToHHMMSSFF(0,m_media_fps));
    m_slider->setValue(0);
}

void SimpleToolbar::updateSliderRange(int64_t mediaDuration){
    Q_ASSERT(mediaDuration < static_cast<int64_t>(std::numeric_limits<int>::max()));
    
    m_discardVlcUiUpdates = false;

    if (mediaDuration >= static_cast<int64_t>(std::numeric_limits<int>::max()))
    {
        m_slider->setMaximum(std::numeric_limits<int>::max());
    }else {
        m_slider->setMaximum(mediaDuration);
    }
    m_media_duration = mediaDuration;
    updateDurationText();

}

void SimpleToolbar::updateSliderValue(int64_t currentTime){
    
    if( m_draggingSlider || m_discardVlcUiUpdates) return;

    Q_ASSERT(currentTime < static_cast<int64_t>(std::numeric_limits<int>::max()));

    if (currentTime >= static_cast<int64_t>(std::numeric_limits<int>::max()))
    {
        m_slider->setValue(static_cast<int64_t>(std::numeric_limits<int>::max()));
    }

    m_slider->setValue(currentTime);

    if( !m_editingTime ) m_timeEdit->setText(TimeFormatter::msToHHMMSSFF(currentTime, m_media_fps));
    if ( m_showRemainingTime ) updateDurationText();
    
}

void SimpleToolbar::updateFps(double newFps){
    m_media_fps = newFps;
}

void SimpleToolbar::playUiUpdate()
{
    m_playPauseBtn->setButtonState(true);
}

void SimpleToolbar::pauseUiUpdate()
{
    m_playPauseBtn->setButtonState(false);
}

void SimpleToolbar::muteUiUpdate()
{
    m_muteBtn->setButtonState(true);
    QPalette p = m_volumeSlider->palette();
    p.setColor(QPalette::Highlight, Qt::darkGray);
    m_volumeSlider->setPalette(p);
}

void SimpleToolbar::unmuteUiUpdate()
{
    m_muteBtn->setButtonState(false);
    m_volumeSlider->setStyleSheet("");

}

void SimpleToolbar::ejectUiUpdate()
{
    m_discardVlcUiUpdates = true;
    m_nameLabel->setText("");
    m_media_duration = 0;
    updateDurationText();
    resetSlider();
    pauseUiUpdate();
}

void SimpleToolbar::stopUiUpdate()
{
    stopSlider();
    pauseUiUpdate();
}

void SimpleToolbar::enableLoopUiUpdate()
{
    m_loopBtn->setButtonState(true);
}

void SimpleToolbar::disableLoopUiUpdate()
{
    m_loopBtn->setButtonState(false);
}

void SimpleToolbar::nameUiUpdate(const QString & mediaName)
{
    m_nameLabel->setText(mediaName);
}

void SimpleToolbar::volumeUiUpdate(const QString & newVolume)
{
    m_volumeLabel->setText(newVolume + "%");
}

void SimpleToolbar::speedUiUpdate(const QString & newSpeed)
{
    m_speedLabel->setText("x" + newSpeed);
}

void SimpleToolbar::enableFullscreenUiUpdate()
{
    m_fullscreenBtn->setButtonState(true);
    m_fullscreenBtn->toggleUpdateIcon();
}

void SimpleToolbar::disableFullscreenUiUpdate()
{
    m_fullscreenBtn->setButtonState(false);
}

void SimpleToolbar::disableLoopMode()
{
    emit m_loopBtn->stateDeactivated();
}

void SimpleToolbar::enableButtons()
{
    m_playPauseBtn->setEnabled(true);
    m_stopBtn->setEnabled(true);
    m_ejectBtn->setEnabled(true);
    // m_muteBtn->setEnabled(true);
    m_langBtn->setEnabled(true);
    m_loopBtn->setEnabled(true);
    m_duplicatePlayerBtn->setEnabled(true);
    // m_removePlayerBtn->setEnabled(true);
    // m_speedBtn->setEnabled(true);
    m_fullscreenBtn->setEnabled(true);
    m_screenshotBtn->setEnabled(true);
}

void SimpleToolbar::disableButtons()
{
    m_stopBtn->setEnabled(false);
    m_ejectBtn->setEnabled(false);
    // m_muteBtn->setEnabled(false);
    m_langBtn->setEnabled(false);
    m_loopBtn->setEnabled(false);
    m_duplicatePlayerBtn->setEnabled(false);
    // m_removePlayerBtn->setEnabled(false);
    // m_speedBtn->setEnabled(false);
    m_fullscreenBtn->setEnabled(false);
    m_screenshotBtn->setEnabled(false);
}


void SimpleToolbar::onSliderPressed() {
    m_draggingSlider = true;
    m_timeEdit->setText(TimeFormatter::msToHHMMSSFF(m_slider->value(), m_media_fps));
    updateDurationText();
}

void SimpleToolbar::onSliderReleased() {
    m_draggingSlider = false;
    m_seekTimer->stop(); 
    emit setPositionRequested(m_slider->value()); 
}

void SimpleToolbar::onSliderMoved(int value) {
    m_timeEdit->setText(TimeFormatter::msToHHMMSSFF(value, m_media_fps));
    updateDurationText();
    if (!m_seekTimer->isActive()) {
        m_seekTimer->start(m_seekPendingTime);
    }
}

void SimpleToolbar::duplicatePlayerAction()
{
    emit duplicatePlayerRequested();
}

void SimpleToolbar::updateAudioTracks(const QList<QPair<int, QString>>& tracks){
    m_audioLangComboBox->clear();

    if (tracks.isEmpty()) {
        //qDebug() << "[SIMPLETOOLBAR] No languages available";
        m_audioLangComboBox->addItem(TextManager::instance().get("no_audio"));
    }
    
    for (const auto& track : tracks) {
        //qDebug() << "n° " << track.second << " : " << track.first;
        m_audioLangComboBox->addItem(track.second, track.first);
    }

}

void SimpleToolbar::updateSubtitlesTracks(const QList<QPair<int, QString>>& tracks){
    m_subLangComboBox->clear();

    if (tracks.isEmpty()) {
        //qDebug() << "[SIMPLETOOLBAR] No subtitles available";
        m_subLangComboBox->addItem(TextManager::instance().get("no_subtitles"));
    }
    
    for (const auto& track : tracks) {
        //qDebug() << "n° " << track.second << " : " << track.first;
        m_subLangComboBox->addItem(track.second, track.first);
    }

}

void SimpleToolbar::setAudioTrackDefault(){
    //m_audioLangComboBox->blockSignals(true);
    m_audioLangComboBox->setCurrentIndex(1);
    //m_audioLangComboBox->blockSignals(false);
}

void SimpleToolbar::setSubtitlesTrackDefault(){
    //m_subLangComboBox->blockSignals(true);
    m_subLangComboBox->setCurrentIndex(0);
    //m_subLangComboBox->blockSignals(false);
}

void SimpleToolbar::setAudioTrack(int index){
    QVariant data = m_audioLangComboBox->itemData(index);

    if (!data.isValid())
        return;

    int trackNumber = data.toInt();

    qDebug() << "[SimpleToolbar] changement demandé sur : " << trackNumber;

    emit setAudioTrackRequested(trackNumber);
    
}

void SimpleToolbar::setSubtitlesTrack(int index){
    QVariant data = m_subLangComboBox->itemData(index);

    if (!data.isValid())
        return;

    int trackNumber = data.toInt();
    qDebug() << "[SimpleToolbar] changement demandé sur : " << trackNumber;
    emit setSubtitlesTrackRequested(trackNumber);
    
}

void SimpleToolbar::createTimeEdit(){
    m_timeEdit = new TimeEdit("00:00:00.00", this);
    m_timeEdit->setFixedWidth(75);
    connect(m_timeEdit, &TimeEdit::focusIn, this, [this](){
        emit pauseRequest();  
        m_editingTime = true;
    });

    connect(m_timeEdit, &TimeEdit::focusOut, this, [this](){
        m_editingTime = false;
    });

    connect(m_timeEdit, &QLineEdit::textEdited, this, [this](){
        emit setPositionRequested(TimeFormatter::HHMMSSFFToMs(m_timeEdit->text(), m_media_fps, 0.05));
    });

    connect(m_timeEdit, &QLineEdit::returnPressed, [this]() {
        m_timeEdit->clearFocus(); 
        emit setPositionRequested(TimeFormatter::HHMMSSFFToMs(m_timeEdit->text(), m_media_fps, 0.05));
    });
}

void SimpleToolbar::createSlider(){
    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setRange(0,0);
    m_slider->setValue(0);

    m_seekTimer = new QTimer(this);
    m_seekTimer->setSingleShot(true);

    connect(m_slider, &QSlider::sliderPressed, this, &SimpleToolbar::onSliderPressed);

    connect(m_slider, &QSlider::sliderReleased, this, &SimpleToolbar::onSliderReleased);

    connect(m_slider, &QSlider::sliderMoved, this, &SimpleToolbar::onSliderMoved);

    connect(m_seekTimer, &QTimer::timeout, this, [this](){
        emit setPositionRequested(m_slider->value());
    });
}


void SimpleToolbar::createTimeTotBtn()
{
    m_durationBtn = new QPushButton("00:00:00.00", this);
    m_durationBtn->setCheckable(true);
    m_durationBtn->setChecked(false);
    m_durationBtn->setToolTip("Temps total");
    m_durationBtn->setFont(m_timeEdit->font());
    m_durationBtn->setObjectName("durationToggleBtn");
    
    m_durationBtn->setFlat(true); 

    connect(m_durationBtn, &QPushButton::toggled, this, [this](bool checked) {
        m_showRemainingTime = checked;
        
        if (checked) {
            m_durationBtn->setToolTip("Temps restant");
        } else {
            m_durationBtn->setToolTip("Temps total");
        }
        
        updateDurationText(); 
    });
}

void SimpleToolbar::updateDurationText()
{
    if (!m_durationBtn) return;

    if (m_showRemainingTime) {
        int64_t remaining = m_media_duration - m_slider->value();
        if (remaining < 0) remaining = 0;
        m_durationBtn->setText("-" + TimeFormatter::msToHHMMSSFF(remaining, m_media_fps));
    } else {
        m_durationBtn->setText(TimeFormatter::msToHHMMSSFF(m_media_duration, m_media_fps));
    }
}