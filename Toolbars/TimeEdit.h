#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QLineEdit>

#include <QAction>
class TimeEdit : public QLineEdit
{
Q_OBJECT

public:
    explicit TimeEdit( const QString& txt, QWidget* parent = nullptr, const bool hasAnimations = true);

private:
    static constexpr int s_widthMin = 90;
    static constexpr int s_widthMax = 160;

    QAction* m_action = nullptr;
    QAction* m_copy = nullptr;
    QAction* m_paste = nullptr;
    const bool m_hasAnimations;

protected:
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

signals:
    void focusIn();
    void focusOut();

};


#endif
