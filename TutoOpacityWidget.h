#ifndef TUTOOPACITYWIDGET_H
#define TUTOOPACITYWIDGET_H

#include "BlackOpacityWidget.h"

class TutoOpacityWidget : public BlackOpacityWidget
{
    Q_OBJECT

public:
    explicit TutoOpacityWidget(QWidget *parent = nullptr, const QRect& windowRect = QRect());
    ~TutoOpacityWidget();
    void setWidgetToShow(QWidget* widgetToShow) { m_widgetToShow = widgetToShow; update(); };

public slots:
    void paintEvent(QPaintEvent *event) override;
    void followParentGeometry();

private:
    QWidget* m_widgetToShow = nullptr;

    // widget pour contenir le texte du tutoriel et les bouton suivant/précédent et skip
    QWidget* m_tutorialWidget = nullptr;
};

#endif // TUTOOPACITYWIDGET_H