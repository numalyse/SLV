#ifndef TUTORIAL_H
#define TUTORIAL_H

#include "TutoOpacityWidget.h"

#include <QObject>
#include <QWidget>
#include <QVector>
#include <QLabel>

#include <functional>



class Tutorial : public QObject
{
Q_OBJECT

public:
    explicit Tutorial(QWidget *parent = nullptr);
    ~Tutorial();

    void addStep(
        QWidget *toDisplay,
        const QString &text,
        std::function<void()> onStepStart,
        std::function<void()> onStepEnd,
        bool showButtons = true
    );

    /// @brief variante pour un widget qui n'existe pas encore au moment de l'ajout du step :
    /// la fonction widgetProvider est appelé quand le step s'affiche
    void addStep(
        std::function<QWidget*()> widgetProvider,
        const QString &text,
        std::function<void()> onStepStart,
        std::function<void()> onStepEnd,
        bool showButtons = true
    );
    void startTutorial();

    TutoOpacityWidget* getTutoOpacityWidget() const { return m_tutoOpacityWidget; };
public slots:
    void nextStep();
    void previousStep();
    void endTutorial();

    /// @brief helper pour deconnecter tous les connect de la step courante
    void leaveCurrentStep();
    void enterCurrentStep();

    void clearAllConnects();

    void moveTutorialWidgetNextToTarget();
    void toggleButtonsVisibility(bool visible);

    void trackConnect(const QMetaObject::Connection& conn);

private:

    struct TutorialStep
    {
        std::function<QWidget*()> widgetProvider;
        QString text;
        std::function<void()> onStepStart;
        std::function<void()> onStepEnd;
        bool showButtons{true};
        QVector<QMetaObject::Connection> connections;
    };

    // widget pour contenir le texte du tutoriel et les bouton suivant/précédent et skip
    QWidget* m_tutorialWidget = nullptr;
    QLabel* m_tutorialText = nullptr;

    TutoOpacityWidget* m_tutoOpacityWidget = nullptr;

    QVector<TutorialStep> m_steps;
    int m_currentStepIndex{-1};



};

#endif // TUTORIAL_H