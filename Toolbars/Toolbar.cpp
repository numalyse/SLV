#include "Toolbars/Toolbar.h"
#include "Toolbar.h"


void Toolbar::showAnimation() {
    if (m_isFullscreen) {
        m_opacityAnimation->stop();
        m_opacityAnimation->setStartValue(windowOpacity());
        m_opacityAnimation->setEndValue(m_maxFullscreenOpacity);
        m_opacityAnimation->start();
    }
}

void Toolbar::hideAnimation() {
    if (m_isFullscreen) {
        m_opacityAnimation->stop(); 
        m_opacityAnimation->setStartValue(windowOpacity());
        m_opacityAnimation->setEndValue(0);
        m_opacityAnimation->start();
    }
}

void Toolbar::enableButtons()
{
    m_stopBtn->setEnabled(true);
    m_ejectBtn->setEnabled(true);
    m_fullscreenBtn->setEnabled(true);
    m_screenshotBtn->setEnabled(true);
    m_zoomBtn->setEnabled(true);
}

void Toolbar::disableButtons()
{
    m_stopBtn->setEnabled(false);
    m_ejectBtn->setEnabled(false);
    if(m_fullscreenBtn->isChecked()) m_fullscreenBtn->click(); 
    m_fullscreenBtn->setEnabled(false);
    m_screenshotBtn->setEnabled(false);
    m_zoomBtn->setEnabled(false);
}
