#ifndef TOOLBAR_H
#define TOOLBAR_H 

#include "ToolbarButtons/ToolbarButton.h"
#include "ToolbarButtons/ToolbarToggleButton.h"
#include "ShortcutHelper.h"

#include <QWidget>
#include <QMessageBox>
#include <QEnterEvent>
#include <QLayout>
#include <QPainter>
#include <QTimer>
#include <QPropertyAnimation>
#include <QStyleHints>
#include <QGuiApplication>
#include <QColor>
#include "PrefManager.h"
#include "SignalManager.h"

/// @brief Classe abstraite qui sert de base pour les différentes toolbars.
class Toolbar : public QWidget
{
Q_OBJECT

public:

    explicit Toolbar(QWidget* parent = nullptr) : QWidget(parent) {
        setAttribute(Qt::WA_TranslucentBackground);
        m_playPauseBtn = new ToolbarToggleButton(
            this,
            true,
            "pause_white",
            PrefManager::instance().getText("tooltip_pause") + "<br><i>("
            + PrefManager::instance().getText("tooltip_shortcut")
            + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "play_pause") + ")</i>",
            "play_white",
            PrefManager::instance().getText("tooltip_play") + "<br><i>("
            + PrefManager::instance().getText("tooltip_shortcut")
            + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "play_pause") + ")</i>"
        );
        m_playPauseBtn->setButtonState(false);
        m_playPauseBtn->setEnabled(true);
        m_playPauseBtn->setFixedSize(35, 35);
        m_playPauseBtn->setIconSize(QSize(25, 25));

        m_parent = parent;
        
        m_stopBtn = new ToolbarButton(this, "stop_white", PrefManager::instance().getText("tooltip_stop") 
        + "<br>"
        + PrefManager::instance().getText("tooltip_stop_info_global")
        + "<br><i>("
        + PrefManager::instance().getText("tooltip_shortcut")
        + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "stop") + ")</i>");
        m_stopBtn->setIconSize(QSize(24, 24));

        m_ejectBtn = new ToolbarButton(this, "eject_white", PrefManager::instance().getText("tooltip_eject"));
        m_fullscreenBtn = new ToolbarToggleButton(this, false, "fullscreen_off_white",
        PrefManager::instance().getText("tooltip_fullscreen") + "<br><i>("
        + PrefManager::instance().getText("tooltip_shortcut")
        + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "exit_fullscreen") + ")</i>",
        "fullscreen_white",
        PrefManager::instance().getText("tooltip_fullscreen") + "<br><i>("
        + PrefManager::instance().getText("tooltip_shortcut")
        + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "enter_fullscreen") + ")</i>");
        m_screenshotBtn = new ToolbarButton(this, "capture_white", PrefManager::instance().getText("tooltip_capture") + "<br><i>("
        + PrefManager::instance().getText("tooltip_shortcut")
        + PrefManager::instance().getPref("Shortcuts", "CommonToolbar", "screenshot") + ")</i>");
        m_zoomBtn = new ToolbarToggleButton(
            this,
            false,
            "zoom_white",
            PrefManager::instance().getText("tooltip_zoom_on"),
            "zoom_white",
            PrefManager::instance().getText("tooltip_zoom_off")
        );
        m_zoomBtn->setToggledIconFrame(true);


        m_opacityAnimation = new QPropertyAnimation(this, "windowOpacity", this);
        m_opacityAnimation->setDuration(200);
        m_maxFullscreenOpacity = 1.0; 

        connect(m_playPauseBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::playRequest);
        connect(m_playPauseBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::pauseRequest);
        connect(m_stopBtn, &ToolbarButton::clicked, this, &Toolbar::stopRequest);
        connect(m_ejectBtn, &ToolbarButton::clicked, this, &Toolbar::ejectRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateActivated, this, &Toolbar::enableFullscreenRequested);
        connect(m_fullscreenBtn,&ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableFullscreenRequested);
        connect(m_screenshotBtn, &ToolbarButton::clicked, this, &Toolbar::screenshotRequest);
        connect(&SignalManager::instance(), &SignalManager::playerWidgetSelectFileCanceled, m_playPauseBtn, [this](){ m_playPauseBtn->setButtonState(false); });
        connect(this, &Toolbar::selectFilePlayCanceled, &SignalManager::instance(), &SignalManager::playerWidgetSelectFileCanceled);
        connect(m_zoomBtn, &ToolbarToggleButton::stateActivated, this, &Toolbar::enableZoomMode);
        connect(m_zoomBtn, &ToolbarToggleButton::stateDeactivated, this, &Toolbar::disableZoomMode);
    }

    ToolbarToggleButton* playPauseBtn() const { return m_playPauseBtn; }
    ToolbarButton* stopBtn() const { return m_stopBtn; }
    ToolbarButton* ejectBtn() const { return m_ejectBtn; }
    ToolbarToggleButton* fullscreenBtn() const { return m_fullscreenBtn; }
    ToolbarToggleButton* muteBtn() { return m_muteBtn; };
    ToolbarToggleButton* zoomBtn() { return m_zoomBtn; };

    QVariant m_maxFullscreenOpacity = 1.0;

    virtual ~Toolbar() {
        if(m_dynamicFullscreenShortcut) delete m_dynamicFullscreenShortcut;
    };

    /// @brief Met à jour le layout pour afficher l'interface en plein écran
    virtual void setFullscreenUI(int bottomMargin = 40) {
        m_isFullscreen = true;
        setParent(nullptr);
        setAttribute(Qt::WA_TranslucentBackground);
        setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);

        adjustSize();
        show();
        raise();
        QWidget::activateWindow();
        setWindowOpacity(0);

        QTimer::singleShot(0, this, [this, bottomMargin]() {
            moveOnTopOfParent(bottomMargin);
        });
    }

    /// @brief Met à jour le layout pour afficher l'interface par défaut
    virtual void setDefaultUI(){
        m_isFullscreen = false;

        if (m_parent) {
            if (m_parent->layout()) m_parent->layout()->addWidget(this); 
        }

        QTimer::singleShot(0, this, [this]() {
            if (!m_isReplacedByAdvanced) {
                show();
            }
        });
    };

    /// @brief Move toolbar on resize when in fullscreen
    virtual void updateFullscreenPosition() {};

    void setTBParent(QWidget* parent){
        m_parent = parent;
        setParent(parent);
    }

    void setReplacedByAdvanced(bool replaced) {
        m_isReplacedByAdvanced = replaced;
        if (replaced) {
            hide();
        }
    }

    void showAnimation();
    void hideAnimation();

    virtual void enableButtons();
    virtual void disableButtons();

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
    ToolbarToggleButton* m_zoomBtn = nullptr;
    bool m_firstTimeDialog = false;
    bool m_isReplacedByAdvanced = false; 

    QShortcut* m_dynamicFullscreenShortcut = nullptr;

    QPropertyAnimation* m_opacityAnimation = nullptr;

    bool m_isFullscreen = false;
    QWidget* m_parent = nullptr;

    void moveOnTopOfParent(int bottomMargin){
        if (m_parent && m_isFullscreen) {
            QPoint parentGlobalPos = m_parent->mapToGlobal(QPoint(0, 0));
            int posX = parentGlobalPos.x() + (m_parent->width() - this->width()) / 2;
            int posY = parentGlobalPos.y() + m_parent->height() - this->height() - bottomMargin;

            QScreen* screen = nullptr;
            if(m_parent){
                screen = m_parent->screen();
            }
        
            if (screen) {
                QRect geo = screen->geometry();
                posX = qMax(geo.left(), qMin(posX, geo.right() - width()));
                posY = qMax(geo.top(), qMin(posY, geo.bottom() - height()));
            }

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
        /*

        */
        QWidget::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        /*

        */
        QWidget::leaveEvent(event);
    }

    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::NoPen);
        if(m_isFullscreen){
            if(QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark){
                painter.setBrush(QColor(30, 30, 30, 255));
            } else {
                QColor colorBtn = qApp->palette().color(QPalette::Window);
                painter.setBrush(colorBtn);
            }
            painter.drawRoundedRect(rect(), 12, 12);
        }
        else painter.drawRect(rect()); // pas de bords arrondies en mode normal
        QWidget::paintEvent(event);
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
    void enableZoomMode();
    void disableZoomMode();
};

#endif // TOOLBAR_H