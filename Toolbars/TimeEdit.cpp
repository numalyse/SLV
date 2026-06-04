#include "Toolbars/TimeEdit.h"
#include "TimeEdit.h"
#include "TimeValidator.h"
#include "PrefManager.h"

#include <QPropertyAnimation>
#include <QGuiApplication>
#include <QClipboard>
#include <QStyleHints>

TimeEdit::TimeEdit(const QString &txt, QWidget *parent) : QLineEdit(parent)
{
    setText(txt);
    setInputMask("99:99:99.99");
    setValidator(new TimeValidator(this));
    setFocusPolicy(Qt::ClickFocus);
    
    setFixedWidth(TimeEdit::s_widthMin);
    setAlignment(Qt::AlignCenter);

    QString userTheme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";

    m_paste = addAction(QIcon(":/icons/paste"+userTheme), QLineEdit::TrailingPosition);
    m_paste->setVisible(false);
    m_paste->setToolTip(PrefManager::instance().getText("paste"));

    m_copy = addAction(QIcon(":/icons/copy"+userTheme), QLineEdit::TrailingPosition);
    m_copy->setVisible(false);
    m_copy->setToolTip(PrefManager::instance().getText("copy"));

    m_action = addAction(QIcon(":/icons/check"+userTheme), QLineEdit::TrailingPosition);
    m_action->setVisible(false);
    m_action->setToolTip(PrefManager::instance().getText("confirm"));

    connect(m_action, &QAction::triggered, this, [this](){
        emit returnPressed();
    });

    connect(m_copy, &QAction::triggered, this, [this](){
        QGuiApplication::clipboard()->setText(text());
        emit returnPressed();
    });

    connect(m_paste, &QAction::triggered, this, [this](){
        QString pastedText = QGuiApplication::clipboard()->text();
        QRegularExpression re("^\\d{2}:\\d{2}:\\d{2}\\.\\d{2}$");
        if(re.match(pastedText).hasMatch()){
            setText(pastedText);
            emit returnPressed();
        }
    });

}

void TimeEdit::focusInEvent(QFocusEvent *e)
{
    QLineEdit::focusInEvent(e);
    emit focusIn(); 
    setAlignment(Qt::AlignLeft);

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(TimeEdit::s_widthMax);
    animMin->setStartValue(width());
    animMin->setEndValue(TimeEdit::s_widthMax);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(TimeEdit::s_widthMax);
    animMax->setStartValue(width());
    animMax->setEndValue(TimeEdit::s_widthMax);

    connect(animMax, &QPropertyAnimation::finished, this, [this](){
        if (this->hasFocus()) {
            m_action->setVisible(true);
            m_copy->setVisible(true);
            m_paste->setVisible(true);
        }
    });

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}

void TimeEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();
    setAlignment(Qt::AlignCenter);

    m_action->setVisible(false);
    m_copy->setVisible(false);
    m_paste->setVisible(false);

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(TimeEdit::s_widthMax);
    animMin->setStartValue(width());
    animMin->setEndValue(TimeEdit::s_widthMin);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(TimeEdit::s_widthMax);
    animMax->setStartValue(width());
    animMax->setEndValue(TimeEdit::s_widthMin);

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}
