#ifndef NAVPANELCONTENTBASE_H
#define NAVPANELCONTENTBASE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QScrollArea>

class NavPanelContentBase : public QWidget
{
    Q_OBJECT
public:
    explicit NavPanelContentBase(QWidget *parent = nullptr);

protected:
    QVBoxLayout* headerLayout() const  { return m_headerLayout; }
    QVBoxLayout* scrollLayout() const { return m_scrollLayout; }
    QVBoxLayout* footerLayout() const { return m_footerLayout; }

private:
    QVBoxLayout *m_headerLayout;
    QVBoxLayout *m_scrollLayout;
    QVBoxLayout *m_footerLayout;
};

#endif // NAVPANELCONTENTBASE_H