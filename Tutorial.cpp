#include "Tutorial.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QGuiApplication>
#include <QScreen>

Tutorial::Tutorial(QWidget *parent) : QObject(parent) {
    m_tutoOpacityWidget = new TutoOpacityWidget(parent);

    m_tutorialWidget = new QWidget(m_tutoOpacityWidget);
    m_tutorialWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::Tool);


    QVBoxLayout* layout = new QVBoxLayout(m_tutorialWidget);
    layout->setContentsMargins(10, 10, 10, 10);
    m_tutorialText = new QLabel("Tutorial Step Text", m_tutorialWidget);
    m_tutorialText->setWordWrap(true);
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
    layout->addWidget(m_tutorialText);
    layout->addLayout(buttonLayout);
}

Tutorial::~Tutorial() {}

void Tutorial::addStep(
    QWidget *toDisplay,
    const QString &text,
    std::function<void()> onStepStart,
    std::function<void()> onStepEnd,
    bool showButtons
)
{
    addStep([toDisplay]() -> QWidget* { return toDisplay; }, text, std::move(onStepStart), std::move(onStepEnd), showButtons);
}

void Tutorial::addStep(
    std::function<QWidget*()> widgetProvider,
    const QString &text,
    std::function<void()> onStepStart,
    std::function<void()> onStepEnd,
    bool showButtons
)
{
    m_steps.append({std::move(widgetProvider), text, std::move(onStepStart), std::move(onStepEnd), showButtons});
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
    if(m_currentStepIndex + 1 >= m_steps.size()){
        endTutorial();
        return;
    }

    if(m_currentStepIndex >= 0)
    {
        leaveCurrentStep();
    }

    m_currentStepIndex++;
    enterCurrentStep();
}

void Tutorial::previousStep()
{
    if(m_currentStepIndex <= 0)
        return;

    leaveCurrentStep();
    m_currentStepIndex--;
    enterCurrentStep();
}

void Tutorial::endTutorial()
{

    clearAllConnects();
    m_steps.clear();

    if(m_tutorialWidget){
        m_tutorialWidget->deleteLater();
        m_tutorialWidget = nullptr;
    }

    if(m_tutoOpacityWidget){
        m_tutoOpacityWidget->deleteLater();
        m_tutoOpacityWidget = nullptr;
    }

}

void Tutorial::moveTutorialWidgetNextToTarget()
{
    if(m_currentStepIndex < 0 || m_currentStepIndex >= m_steps.size())
        return;

    const auto& step = m_steps[m_currentStepIndex];
    QWidget* target = step.widgetProvider ? step.widgetProvider() : nullptr;

    if(m_tutorialWidget && target) {
        m_tutorialWidget->adjustSize();

        QPoint globalPos = target->mapToGlobal(QPoint(0, 0));
        m_tutorialWidget->move(globalPos.x(), globalPos.y() - m_tutorialWidget->height() - 10); // above target

        // clamp la position du tutorial widget dans la fenetre du widget à montrer
        QWidget* win = target->window();
        QScreen* scr = QGuiApplication::screenAt(win->geometry().center());
        if (!scr) scr = QGuiApplication::primaryScreen();

        QRect available = scr->availableGeometry();
        QRect frame = m_tutorialWidget->frameGeometry();

        int w = frame.width();
        int h = frame.height();

        int maxX = qMax(available.left(), available.right()  - w);
        int maxY = qMax(available.top(),  available.bottom() - h);

        int x = qBound(available.left(), frame.left(), maxX);
        int y = qBound(available.top(),  frame.top(),  maxY);

        m_tutorialWidget->move(x, y);
        m_tutorialWidget->show();
    }
}

void Tutorial::toggleButtonsVisibility(bool visible)
{
    if(m_tutorialWidget){
        for(auto* child : m_tutorialWidget->findChildren<QPushButton*>()){
            child->setVisible(visible);
        }
    }
}

void Tutorial::trackConnect(const QMetaObject::Connection& conn)
{
    if(m_currentStepIndex < 0 || m_currentStepIndex >= m_steps.size())
        return;

    auto& step = m_steps[m_currentStepIndex];
    step.connections.append(conn);
}

void Tutorial::leaveCurrentStep()
{
    if(m_currentStepIndex < 0 || m_currentStepIndex >= m_steps.size())
        return;

    auto& step = m_steps[m_currentStepIndex];
    if(step.onStepEnd)
        step.onStepEnd();

    for(auto& conn : step.connections)
        QObject::disconnect(conn);
        
    step.connections.clear();
}

void Tutorial::enterCurrentStep()
{
    if(m_currentStepIndex < 0 || m_currentStepIndex >= m_steps.size())
        return;

    const auto& step = m_steps[m_currentStepIndex];
    if(step.onStepStart)
        step.onStepStart();

    QTimer::singleShot(0, this, [this]() {
        if(!m_tutoOpacityWidget || m_currentStepIndex < 0 || m_currentStepIndex >= m_steps.size())
            return;
        const auto& currentStep = m_steps[m_currentStepIndex];
        m_tutoOpacityWidget->setWidgetToShow(currentStep.widgetProvider ? currentStep.widgetProvider() : nullptr);
        moveTutorialWidgetNextToTarget();
    });

    m_tutorialText->setText(m_steps[m_currentStepIndex].text);

    if(m_steps[m_currentStepIndex].showButtons){
        toggleButtonsVisibility(true);
    }else {
        toggleButtonsVisibility(false);
    } 
}


void Tutorial::clearAllConnects()
{
    if(m_steps.size() == 0){
        return;
    }

    for(auto& step : m_steps){
        for(auto& conn : step.connections)
            QObject::disconnect(conn);
    }
}