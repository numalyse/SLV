#ifndef TIMEEDIT_H
#define TIMEEDIT_H

#include <QLineEdit>

#include <QAction>
#include <cstdint>
#include <limits>
class TimeValidator;
class TimeEdit : public QLineEdit
{
Q_OBJECT

public:
    explicit TimeEdit( const QString& txt, QWidget* parent = nullptr, const bool hasAnimations = true);
    void setFrameRate(double fps);
    void setMaximumTime(int64_t time);
    void normalizeTimecode();

private:
    static constexpr int s_widthMin = 90;
    static constexpr int s_widthMax = 160;

    TimeValidator* m_timeValidator = nullptr;
    double m_fps = 24.0;
    int64_t m_maxTime = std::numeric_limits<int64_t>::max();
    bool m_normalizingTimecode = false;
    QAction* m_action = nullptr;
    QAction* m_copy = nullptr;
    QAction* m_paste = nullptr;
    const bool m_hasAnimations;

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void focusInEvent(QFocusEvent *e) override;
    void focusOutEvent(QFocusEvent *e) override;

signals:
    void focusIn();
    void focusOut();

};


#endif
