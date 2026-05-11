#include "Toolbars/TimeEdit.h"
#include "TimeEdit.h"
#include "TimeValidator.h"
#include "PrefManager.h"

#include <QPropertyAnimation>
#include <QGuiApplication>
#include <QClipboard>

TimeEdit::TimeEdit(const QString &txt, QWidget *parent) : QLineEdit(parent)
{
    setText(txt);
    setInputMask("99:99:99.99");
    setValidator(new TimeValidator(this));
    
    setFixedWidth(75);

    m_paste = addAction(QIcon(":/icons/paste_white"), QLineEdit::TrailingPosition);
    m_paste->setVisible(false);
    m_paste->setToolTip(PrefManager::instance().getText("paste"));

    m_copy = addAction(QIcon(":/icons/copy_white"), QLineEdit::TrailingPosition);
    m_copy->setVisible(false);
    m_copy->setToolTip(PrefManager::instance().getText("copy"));

    m_action = addAction(QIcon(":/icons/check_white"), QLineEdit::TrailingPosition);
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

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(150);
    animMin->setStartValue(width());
    animMin->setEndValue(150);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(150);
    animMax->setStartValue(width());
    animMax->setEndValue(150);

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

    m_action->setVisible(false);
    m_copy->setVisible(false);
    m_paste->setVisible(false);

    QPropertyAnimation *animMin = new QPropertyAnimation(this, "minimumWidth");
    animMin->setDuration(150);
    animMin->setStartValue(width());
    animMin->setEndValue(75);
    
    QPropertyAnimation *animMax = new QPropertyAnimation(this, "maximumWidth");
    animMax->setDuration(150);
    animMin->setStartValue(width());
    animMax->setEndValue(75);

    animMin->start(QAbstractAnimation::DeleteWhenStopped);
    animMax->start(QAbstractAnimation::DeleteWhenStopped);
}
