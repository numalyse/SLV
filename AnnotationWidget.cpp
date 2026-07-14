#include "AnnotationWidget.h"
#include "PrefManager.h"
#include "TimeFormatter.h"
#include "Project/ProjectManager.h"
#include <QMenu>
#include <QMouseEvent>
#include <QGuiApplication>
#include <QStyleHints>

AnnotationWidget::AnnotationWidget(QWidget *parent, const Annotation &annotation)
    : QWidget{parent}, m_annotation(annotation)
{
    m_isDarkMode = QGuiApplication::styleHints()->colorScheme() == Qt::ColorScheme::Dark;

#ifdef Q_OS_MAC
    QColor colorBtn = qApp->palette().color(QPalette::Button);
    QColor enhancedColor = m_isDarkMode ? colorBtn.lighter(150) : colorBtn.darker(150);
    m_palbtnColor = m_isDarkMode ? enhancedColor : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? QString(enhancedColor.name()) : "palette(mid)";
#else
    m_palbtnColor = m_isDarkMode ? "palette(button)" : "palette(mid)";
    m_palbtnColorStr = m_isDarkMode ? "palette(button)" : "palette(mid)";
#endif

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(6,4,6,4);
    mainLayout->setSpacing(8);

    m_colorBar = new QFrame();
    m_colorBar->setFixedWidth(6);
    mainLayout->addWidget(m_colorBar);

    // for start / end timecodes
    QVBoxLayout *timeLayout = new QVBoxLayout();
    timeLayout->setSpacing(2);

    m_startLabel = new QLabel("00:00:00.00");
    m_endLabel = new QLabel("00:00:00.00");

    timeLayout->addWidget(m_startLabel);
    timeLayout->addWidget(m_endLabel);

    mainLayout->addLayout(timeLayout);

    // bloc titre / note
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);

    m_nameLabel = new QLabel();
    m_nameLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    m_noteLabel = new QLabel();
    m_noteLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
    
    infoLayout->addWidget(m_nameLabel);
    infoLayout->addWidget(m_noteLabel);

    mainLayout->addLayout(infoLayout, 1);
    mainLayout->addSpacing(10);

    m_menuBtn = new ToolbarButton(this, "", "");
    mainLayout->addWidget(m_menuBtn);

    initStyle();
    refreshContent();

    connect(m_menuBtn, &ToolbarButton::clicked, this, &AnnotationWidget::openMenu);
}

void AnnotationWidget::initStyle()
{
    setFixedHeight(50);
    setAttribute(Qt::WA_StyledBackground, true);
    setContentsMargins(0,0,0,0);
    setStyleSheet("AnnotationWidget{border-style: solid; border: 1px solid " + m_palbtnColorStr + "; border-radius: 4px;}");

    m_menuBtn->setFixedSize(24,24);
    m_menuBtn->setMaximumWidth(20);
    m_menuBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "QPushButton:hover{"
        "   background-color: " + m_palbtnColorStr + ";"
        "   border: 1px solid " + m_palbtnColorStr + ";"
        "   border-radius: 4px;"
        "}");
}

void AnnotationWidget::refreshContent()
{
    m_colorBar->setStyleSheet("background-color: " + m_annotation.color.name() + "; border-radius: 2px;");

    double fps = currentFps();
    QString start = TimeFormatter::msToHHMMSSFF(m_annotation.start, fps);
    QString end = TimeFormatter::msToHHMMSSFF(m_annotation.end, fps);
    m_startLabel->setText(start);
    m_endLabel->setText(end);
    
    m_nameLabel->setText(m_annotation.name);
    m_noteLabel->setText(m_annotation.note);
    m_noteLabel->setToolTip(m_annotation.note);
}

void AnnotationWidget::updateAnnotation(const Annotation &annotation)
{
    m_annotation = annotation;
    refreshContent();
}

double AnnotationWidget::currentFps() const
{
    Media* media = ProjectManager::instance().media();
    if (media && media->fps() > 0.0)
        return media->fps();
    return 25.0;
}

void AnnotationWidget::openMenu()
{
    QMenu menu(this);
    QAction *editAction = menu.addAction(PrefManager::instance().getText("edit"));
    QAction *removeAction = menu.addAction(PrefManager::instance().getText("remove"));

    QAction *chosenAction = menu.exec(m_menuBtn->mapToGlobal(QPoint(0, m_menuBtn->height())));
    if (chosenAction == editAction)
        emit editAnnotationRequested(m_annotation.id);
    else if (chosenAction == removeAction)
        emit removeAnnotationRequested(m_annotation.id);
}


void AnnotationWidget::enterEvent(QEnterEvent *event)
{
    setStyleSheet("AnnotationWidget{border-style: solid; border: 2px solid " + m_palbtnColorStr + "; border-radius: 3px;}");
}

void AnnotationWidget::leaveEvent(QEvent *event)
{
    setStyleSheet("AnnotationWidget{border-style: solid; border: 1px solid " + m_palbtnColorStr + "; border-radius: 4px;}");
}

