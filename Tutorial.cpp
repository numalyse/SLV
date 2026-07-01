#include "Tutorial.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>


Tutorial::Tutorial(QWidget *parent) : QObject(parent) {
    m_tutoOpacityWidget = new TutoOpacityWidget(parent);

    m_tutorialWidget = new QWidget(m_tutoOpacityWidget);
    m_tutorialWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);


    QVBoxLayout* layout = new QVBoxLayout(m_tutorialWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    QLabel* tutorialText = new QLabel("Tutorial Step Text", m_tutorialWidget);
    tutorialText->setWordWrap(true);
    QPushButton* nextButton = new QPushButton("Next", m_tutorialWidget);
    connect(nextButton, &QPushButton::clicked, this, &Tutorial::nextStep);
    QPushButton* prevButton = new QPushButton("Previous", m_tutorialWidget);
    connect(prevButton, &QPushButton::clicked, this, &Tutorial::previousStep);
    QPushButton* skipButton = new QPushButton("Skip", m_tutorialWidget);
    connect(skipButton, &QPushButton::clicked, this, &Tutorial::endTutorial);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(prevButton);
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(skipButton);
    layout->addWidget(tutorialText);
    layout->addLayout(buttonLayout);

    m_steps.append({parent, "First Step", nullptr, nullptr});

}

Tutorial::~Tutorial() {}

void Tutorial::addStep(QWidget *toDisplay, const QString &text, std::function<void()> onStepStart, std::function<void()> onStepEnd)
{
    m_steps.append({toDisplay, text, onStepStart, onStepEnd});
}

void Tutorial::startTutorial()
{
    if(m_steps.isEmpty())
        return;

    m_currentStepIndex = -1;
    m_tutoOpacityWidget->show();
    nextStep();
}

void Tutorial::nextStep()
{
    if(m_currentStepIndex + 1 >= m_steps.size())
        return;

    if(m_currentStepIndex >= 0)
    {
        const auto& previousStep = m_steps[m_currentStepIndex];
        if(previousStep.onStepEnd)
            previousStep.onStepEnd();
    }

    m_currentStepIndex++;

    const auto& step = m_steps[m_currentStepIndex];
    if(step.onStepStart)
        step.onStepStart();

    moveTutorialWidgetNextToTarget();
}

void Tutorial::previousStep()
{
    if(m_currentStepIndex <= 0)
        return;

    const auto& currentStep = m_steps[m_currentStepIndex];
    if(currentStep.onStepEnd)
        currentStep.onStepEnd();

    m_currentStepIndex--;

    const auto& step = m_steps[m_currentStepIndex];
    if(step.onStepStart)
        step.onStepStart();

    moveTutorialWidgetNextToTarget();
}

void Tutorial::endTutorial()
{
    if(m_currentStepIndex >= 0)
    {
        const auto& currentStep = m_steps[m_currentStepIndex];
        if(currentStep.onStepEnd)
            currentStep.onStepEnd();
    }

    m_currentStepIndex = -1;

    if(m_tutorialWidget)
        m_tutorialWidget->hide();
    if(m_tutoOpacityWidget)
        m_tutoOpacityWidget->hide();
}

void Tutorial::moveTutorialWidgetNextToTarget()
{
    if(m_currentStepIndex < 0 || m_currentStepIndex >= m_steps.size())
        return;

    const auto& step = m_steps[m_currentStepIndex];

    if(m_tutorialWidget && step.toDisplay) {
        QPoint globalPos = step.toDisplay->mapToGlobal(QPoint(0, 0));
        m_tutorialWidget->move(globalPos.x() + step.toDisplay->width() + 10, globalPos.y());
        m_tutorialWidget->show();
    }
}

