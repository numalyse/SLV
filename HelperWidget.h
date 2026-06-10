#ifndef HELPERVIDGET_H
#define HELPERVIDGET_H

#include <QDialog>
#include <QScrollArea>
#include <QLabel>
#include <QCheckBox>
#include <QGuiApplication>
#include <QStyleHints>
#include <QLayout>

class CustomHoverLabel : public QLabel
{
    Q_OBJECT

public:
    explicit CustomHoverLabel(const QString &text, QWidget *parent = nullptr) : QLabel(text, parent){
        installEventFilter(this);
        setAttribute(Qt::WA_Hover);
        setWordWrap(true);
    }

signals:
    void clicked();


protected:
    void enterEvent(QEnterEvent* event) override
    {
        setStyleSheet("text-decoration:underline;");
    }
    void leaveEvent(QEvent* event) override
    {
        setStyleSheet("");
    }
    void mousePressEvent(QMouseEvent* event) override{
        emit clicked();
    }
};

class CustomCheckbox : public QWidget
{
    Q_OBJECT
public:
    explicit CustomCheckbox(const QString& text, QWidget *parent = nullptr) : QWidget(parent){
        QHBoxLayout *layout = new QHBoxLayout();
        m_text = new QLabel(text);
        m_checkbox = new QCheckBox();
        layout->addWidget(m_text);
        layout->addStretch();
        layout->addWidget(m_checkbox);
        m_checkbox->setChecked(true);
        QString theme = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark ? "_white" : "";
        setStyleSheet(R"(
            QCheckBox{}
            QCheckBox::indicator{
                height: 10px;
                width: 10px;
            }
            QCheckBox::indicator:checked{
                image: url(:/icons/down_arrow)" + theme + R"();
            }
            QCheckBox::indicator:unchecked{
                image: url(:/icons/right_arrow)" + theme + R"();
            }
        )");
        setContentsMargins(0,0,0,0);
        layout->setContentsMargins(0,0,0,0);
        setLayout(layout);
        connect(m_checkbox, &QCheckBox::clicked, this, &CustomCheckbox::stateChanged);
    }
    void setChecked(){ m_checkbox->setChecked(true); };
    void setUnchecked(){ m_checkbox->setChecked(false); }

signals:
    void stateChanged(const bool state);

private:
    QLabel *m_text = nullptr;
    QCheckBox *m_checkbox = nullptr;

protected:
    void mousePressEvent(QMouseEvent* event) override{
        m_checkbox->click();
        emit stateChanged(m_checkbox->isChecked());
    }
};

class HelperWidget : public QDialog
{
    Q_OBJECT

public:
    explicit HelperWidget(QWidget *parent = nullptr);
    ~HelperWidget();
    void initLayout();
    void animateWidget(QWidget* widgetToAnimate, const bool state);
    void addButtonDescription(QGridLayout* gridLayout, const QString& iconName, const QString& buttonLabel, const QString& buttonDescription, const unsigned int lineIndex);

public slots:
    void setGeneralContent();
    void setClassicContent(int scrollLevel = 0);
    void setMultiviewContent(int scrollLevel = 0);
    void setPlaylistContent(int scrollLevel = 0);
    void setTimelineContent(int scrollLevel = 0);

private:
    int m_imageWidth;
    QWidget *m_mainWidget = nullptr;
    QScrollArea *m_scrollArea = nullptr;
    QScrollArea *m_menuScrollArea = nullptr;
    QWidget *m_sideMenu = nullptr;
    QWidget *m_contentWidget = nullptr;

    CustomCheckbox *m_generalTab = nullptr;
    CustomCheckbox *m_classicTab = nullptr;
    CustomCheckbox *m_multiviewTab = nullptr;
    CustomCheckbox *m_playlistTab = nullptr;
    CustomCheckbox *m_timelineTab = nullptr;
    CustomCheckbox *m_optionsTab = nullptr;
};

#endif // HELPERVIDGET_H