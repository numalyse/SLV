#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ShortcutHelper.h"

#include <QWidget>
#include <QEnterEvent>
#include <QLayout>
#include <PrefManager.h>
#include <SignalManager.h>


/// @brief Classe abstraite qui sert de base pour les différentes toolbars.
class Toolbar : public QWidget
{
Q_OBJECT

public:

    explicit Toolbar(QWidget* parent = nullptr) : QWidget(parent) {
        m_playPauseBtn = new ToolbarToggleButton(
            this,
            true,
            "pause_white",
            PrefManager::instance().getText("tooltip_pause"),
            "play_white",
            PrefManager::instance().getText("tooltip_play")
        );
        m_playPauseBtn->setButtonState(false);
        m_playPauseBtn->setEnabled(true);

        m_parent = parent;
        
        m_stopBtn = new ToolbarButton(this, "stop_white", PrefManager::instance().getText("tooltip_stop"));
        m_ejectBtn = new ToolbarButton(this, "eject_white", PrefManager::instance().getText("tooltip_eject"));
        m_fullscreenBtn = new ToolbarToggleButton(this, false, "fullscreen_off_white", PrefManager::instance().getText("tooltip_fullscreen"), "fullscreen_white", PrefManager::instance().getText("tooltip_fullscreen"));
        m_screenshotBtn = new ToolbarButton(this, "capture_white", PrefManager::instance().getText("tooltip_capture"));

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::playRequest);
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::pauseRequest);
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequest);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateActivated, this, &Toolbar::enableFullscreenRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableFullscreenRequested);
        connect(m_screenshotBtn, &ToolbarButton::clicked, this, &Toolbar::screenshotRequest);
        connect(&SignalManager::instance(), &SignalManager::playerWidgetSelectFileCanceled, m_playPauseBtn, [this](){ m_playPauseBtn->setButtonState(false); });
        connect(this, &Toolbar::selectFilePlayCanceled, &SignalManager::instance(), &SignalManager::playerWidgetSelectFileCanceled);
    }

    ToolbarToggleButton* playPauseBtn() const { return m_playPauseBtn; }
    ToolbarButton* stopBtn() const { return m_stopBtn; }
    ToolbarButton* ejectBtn() const { return m_ejectBtn; }
    ToolbarToggleButton* fullscreenBtn() const { return m_fullscreenBtn; }
    ToolbarToggleButton* muteBtn() { return m_muteBtn; };

    virtual ~Toolbar() {
        if(m_dynamicFullscreenShortcut) delete m_dynamicFullscreenShortcut;
    };

    /// @brief Met à jour le layout pour afficher l'interface en plein écran
    virtual void setFullscreenUI() {
        m_isFullscreen = true;

        setParent(nullptr);

        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        //adjustSize();
        int targetWidth = width()/2.5;  
        resize(targetWidth, sizeHint().height());
        moveOnTopOfParent();
        show();
        raise();
        QWidget::activateWindow();

        setWindowOpacity(0.01);

    };

    /// @brief Met à jour le layout pour afficher l'interface par défaut
    virtual void setDefaultUI(){
        m_isFullscreen = false;

        if (m_parent) {
            if (m_parent->layout()) m_parent->layout()->addWidget(this); 
        }
        show();
        raise();
    };

    /// @brief Move toolbar on resize when in fullscreen
    virtual void updateFullscreenPosition() {};

    void setTBParent(QWidget* parent){
        m_parent = parent;
        setParent(parent);
    }

public slots:
    virtual void ejectRequested(){
        emit ejectRequest();
    };

    void enableFullscreenRequested(){
        if (m_dynamicFullscreenShortcut) {
            delete m_dynamicFullscreenShortcut;
            m_dynamicFullscreenShortcut = nullptr;
        }

        QString keyString = PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "exit_fullscreen");
        m_dynamicFullscreenShortcut = SLV::createGlobalButtonShortcut(this, keyString, m_fullscreenBtn, false);
        emit enableFullscreenRequest();
    }

    virtual void disableFullscreenRequested(){
        if (m_dynamicFullscreenShortcut) {
            delete m_dynamicFullscreenShortcut;
            m_dynamicFullscreenShortcut = nullptr;
        }
        emit disableFullscreenRequest();
    }

// Les classes filles pourront modifier ces widgets
protected: 
    ToolbarToggleButton* m_playPauseBtn = nullptr;
    ToolbarButton* m_stopBtn = nullptr;
    ToolbarButton* m_ejectBtn = nullptr;
    ToolbarToggleButton* m_fullscreenBtn = nullptr;
    ToolbarButton* m_screenshotBtn = nullptr;
    ToolbarToggleButton* m_muteBtn = nullptr;

    QShortcut* m_dynamicFullscreenShortcut = nullptr;

    bool m_isFullscreen = false;
    QWidget* m_parent = nullptr;

    void moveOnTopOfParent(){
        if (m_parent && m_isFullscreen) {
            QPoint parentGlobalPos = m_parent->mapToGlobal(QPoint(0, 0));
            int posX = parentGlobalPos.x() + (m_parent->width() - this->width()) / 2;
            int posY = parentGlobalPos.y() + m_parent->height() - this->height() - 20;
            move(posX, posY);
        }
    }

    void addEnterFullscreenShortcut(){
        if(m_dynamicFullscreenShortcut){
            delete m_dynamicFullscreenShortcut;
        }
        QString keyString = PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "enter_fullscreen");
        m_dynamicFullscreenShortcut = SLV::createGlobalButtonShortcut(this, keyString, m_fullscreenBtn,  false);
    }

    void enterEvent(QEnterEvent *event) override {
        setWindowOpacity(1.0); 
    }

    void leaveEvent(QEvent *event) override {
        setWindowOpacity(0.01); 
    }

signals:
    void playRequest();
    void pauseRequest();
    void stopRequest();
    void ejectRequest();
    void enableFullscreenRequest();
    void disableFullscreenRequest();
    void enableMuteRequest();
    void screenshotRequest();
    void selectFilePlayCanceled();
};

#endif // TOOLBAR_H
