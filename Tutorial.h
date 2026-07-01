#ifndef TUTORIAL_H
#define TUTORIAL_H

#include "TutoOpacityWidget.h"

#include <QObject>
#include <QWidget>
#include <QVector>

#include <functional>



class Tutorial : public QObject
{
Q_OBJECT

public:
    explicit Tutorial(QWidget *parent = nullptr);
    ~Tutorial();

    void addStep(QWidget* toDisplay, const QString& text, std::function<void()> onStepStart = nullptr, std::function<void()> onStepEnd = nullptr);
    void startTutorial();

    TutoOpacityWidget* getTutoOpacityWidget() const { return m_tutoOpacityWidget; };
public slots:
    void nextStep();
    void previousStep();
    void endTutorial();
    void moveTutorialWidgetNextToTarget();

private:

    struct TutorialStep
    {
        QWidget* toDisplay;
        QString text;
        std::function<void()> onStepStart;
        std::function<void()> onStepEnd;
    };

    // widget pour contenir le texte du tutoriel et les bouton suivant/précédent et skip
    QWidget* m_tutorialWidget = nullptr;

    TutoOpacityWidget* m_tutoOpacityWidget = nullptr;

    QVector<TutorialStep> m_steps;
    int m_currentStepIndex{-1};



};

#endif // TUTORIAL_H