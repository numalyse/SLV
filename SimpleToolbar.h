#ifndef SIMPLETOOLBAR_H
#define SIMPLETOOLBAR_H 

#include "Toolbar.h"
#include "ToolbarButton.h"

#include <QLayout>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

/// @brief Toolbar simple, utilisé pour les lecteurs en mode synchronisé.
class SimpleToolbar : public Toolbar
{
Q_OBJECT

public:
    explicit SimpleToolbar(QWidget* parent = nullptr);
    void setFullscreenUI() override;
    void setDefaultUI() override;

private:
    
    QLabel* m_currentTimeLabel = nullptr;
    QLabel* m_durationLabel = nullptr;

    QSlider* m_slider = nullptr;

    ToolbarButton* m_muteBtn = nullptr;
    QSlider* m_soundSlider = nullptr;

    ToolbarButton* m_slowDownBtn = nullptr;
    ToolbarButton* m_speedUpBtn = nullptr;
};

#endif
