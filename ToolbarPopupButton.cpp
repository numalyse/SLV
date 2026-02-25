#include "ToolbarPopupButton.h"

#include <QtAssert>
#include <QWidget>
#include <QTimer>
#include <QEvent>

/// @brief Créer un bouton qui affiche une popup lors du clique. Si le bouton est cliqué pendant que la popup est affichée, la popup est cachée.
/// @param parent 
/// @param widgetToDisplay QWidget* à afficher dans la popup lors du clique, il sera détruit par ce widget
/// @param iconName QString Nom de l'icone
/// @param toolTipText QString Texte du tooltip
ToolbarPopupButton::ToolbarPopupButton(QWidget *parent, QWidget *widgetToDisplay, const QString &iconName, const QString &toolTipText) : ToolbarButton(parent, iconName, toolTipText)
{
    Q_ASSERT(widgetToDisplay != nullptr);

    widgetToDisplay->setParent(nullptr);
    m_widgetToDisplay = widgetToDisplay;
    m_widgetToDisplay->setWindowFlags(Qt::Popup | Qt::FramelessWindowHint);// Le widget ne sera plus dans une fenêtre séparé et sera au dessus des autres widgets
    
    m_widgetToDisplay->installEventFilter(this);
    m_widgetToDisplay->hide();

    connect(this, &QPushButton::clicked, this, &ToolbarPopupButton::displayPopup);    
}

/// @brief Slot privé qui affiche la popup au dessus du bouton, sauf si la popup est déjà ouverte
/// (cela aura pour effet de cacher la popup)
void ToolbarPopupButton::displayPopup(){
    if (m_blockNextShow){
        m_blockNextShow = false;
        return;
    }

    moveWidgetOnTop();
    m_widgetToDisplay->show();
}

/// @brief Déplace le widget popup centré au dessus du bouton, offset sur la hauteur fixé a 10 px
void ToolbarPopupButton::moveWidgetOnTop()
{
    m_widgetToDisplay->adjustSize(); // Oblige Qt à calculer la taille réelle du widget en fonction de son contenu (sinon lors du premier enter, la position est incorrect)

    QPoint globalPos = mapToGlobal(QPoint(0, 0));

    int width = this->width();
    int height = this->height();

    int height_offset = 10;

    int x = globalPos.x() + (width/2) - (m_widgetToDisplay->width()/2);
    int y = globalPos.y() - m_widgetToDisplay->height() + height_offset;

    m_widgetToDisplay->move(x, y);
}


/// @brief Detecte si l'on à recliqué sur le bouton pendant que la popup était ouverte. Si c'est le cas m_blockNextShow = true
/// @param watched QWidget* de la popup
/// @param event QEvent::Hide
/// @return 
bool ToolbarPopupButton::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_widgetToDisplay) {
        if (event->type() == QEvent::Hide ) {
            QPoint mousePos = QCursor::pos(); 
            // Check si la souris est contenu dans le bouton lorsqu'on hide, 
            // si True : c'est un clique sur le bouton pendant que la popup était visible. Ce flag va empecher à la pop up de se réafficher 
            if (this->rect().contains(this->mapFromGlobal(mousePos))) {
                m_blockNextShow = true;
            }
        }
        
    }

    return ToolbarButton::eventFilter(watched, event);
}