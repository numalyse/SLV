#include "AnnotationWidget.h"
#include "IconHelper.h"
#include "PrefManager.h"
#include "TimeFormatter.h"
#include "Project/ProjectManager.h"
#include "SignalManager.h"

#include <QMouseEvent>
#include <QGuiApplication>
#include <QStyleHints>
#include <QAbstractTextDocumentLayout>
#include <QtMath>
#include <QApplication>

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

    // column holding the top row and the note editor below it
    QVBoxLayout *contentLayout = new QVBoxLayout();
    contentLayout->setSpacing(4);

    m_topRowLayout = new QHBoxLayout();
    m_topRowLayout->setSpacing(8);

    // for start / end timecodes
    m_timeLayout = new QVBoxLayout();
    m_timeLayout->setSpacing(2);

    m_startLabel = new QLabel("00:00:00.00");
    m_endLabel = new QLabel("00:00:00.00");

    // fixed size so they stay in place when switching between display and edit mode
    for(QLabel* timeLabel : {m_startLabel, m_endLabel}){
        timeLabel->setAlignment(Qt::AlignCenter);
        timeLabel->setFixedSize(90, 20);
    }

    m_startEdit = new QLineEdit();
    m_endEdit = new QLineEdit();
    for(QLineEdit* timeEdit : {m_startEdit, m_endEdit}){
        timeEdit->setInputMask("99:99:99.99");
        timeEdit->setAlignment(Qt::AlignCenter);
        timeEdit->setFixedSize(90, 20);
        timeEdit->hide();
    }

    m_timeLayout->addWidget(m_startLabel);
    m_timeLayout->addWidget(m_endLabel);
    m_timeLayout->addWidget(m_startEdit);
    m_timeLayout->addWidget(m_endEdit);

    m_topRowLayout->addLayout(m_timeLayout);
    // always pinned to the top so the timecodes don't move between modes
    m_topRowLayout->setAlignment(m_timeLayout, Qt::AlignTop);

    // bloc name / note
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(2);

    m_nameLabel = new QLabel();
    m_nameLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    m_noteLabel = new QLabel();
    m_noteLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    m_nameEdit = new QLineEdit();
    m_nameEdit->hide();

    infoLayout->addWidget(m_nameLabel);
    infoLayout->addWidget(m_noteLabel);
    infoLayout->addWidget(m_nameEdit);

    m_topRowLayout->addLayout(infoLayout, 1);
    m_topRowLayout->addSpacing(10);

    m_updateBtn = new ToolbarButton(this, "edit_white", PrefManager::instance().getText("edit"));

    connect(m_updateBtn, &ToolbarButton::clicked, this, [this](){
        if(m_mode != Mode::Edited)
            setMode(Mode::Edited);
    });

    // fixed height so update btn stays in place when switching between display and edit mode
    QWidget* updateBtnContainer = new QWidget(this);
    updateBtnContainer->setFixedHeight(42);
    QVBoxLayout* updateBtnLayout = new QVBoxLayout(updateBtnContainer);
    updateBtnLayout->setContentsMargins(0,0,0,0);
    updateBtnLayout->addStretch();
    updateBtnLayout->addWidget(m_updateBtn);
    updateBtnLayout->addStretch();

    m_topRowLayout->addWidget(updateBtnContainer);
    m_topRowLayout->setAlignment(updateBtnContainer, Qt::AlignTop);

    contentLayout->addLayout(m_topRowLayout);

    m_noteEdit = new QTextEdit();
    m_noteEdit->setFixedHeight(80);
    m_noteEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_noteEdit->hide();
    contentLayout->addWidget(m_noteEdit);

    // grow the note editor with its content, queued so the resize happens later once everything is set
    connect(m_noteEdit->document()->documentLayout(), &QAbstractTextDocumentLayout::documentSizeChanged,
            this, &AnnotationWidget::adjustNoteEditHeight, Qt::QueuedConnection);

    // color / confirm / cancel row 
    QHBoxLayout *editActionsLayout = new QHBoxLayout();
    editActionsLayout->setSpacing(8);

    m_colorBtn = new QPushButton();
    m_colorBtn->setFixedSize(60, 24);
    m_colorBtn->setToolTip(PrefManager::instance().getText("tooltip_color_tool"));
    m_colorBtn->hide();
    connect(m_colorBtn, &QPushButton::clicked, this, &AnnotationWidget::openColorPicker);

    m_confirmBtn = new ToolbarButton(this, "check_white", PrefManager::instance().getText("confirm"));
    m_cancelBtn = new ToolbarButton(this, "cancel_white", PrefManager::instance().getText("cancel_action"));
    m_deleteBtn = new ToolbarButton(this, "delete_white", PrefManager::instance().getText("delete"));

    m_confirmBtn->hide();
    m_cancelBtn->hide();
    m_deleteBtn->hide();

    connect(m_confirmBtn, &QPushButton::released, this, [this](){
        auto* annotManager = ProjectManager::instance().annotationManager();
        if( annotManager && annotManager->hasConflict(editedAnnotation()))
        {
            m_startEdit->setStyleSheet(QString("border: 2px solid tomato; border-radius: 4px;"));
            m_endEdit->setStyleSheet(QString("border: 2px solid tomato; border-radius: 4px;"));
        } else
            applyEdits();
    });
    connect(m_cancelBtn, &QPushButton::released, this, [this]{ setMode(Mode::Minimized); });
    connect(m_deleteBtn, &QPushButton::released, this, [this]{ emit removeAnnotationRequested(m_annotation.id); });

    editActionsLayout->addWidget(m_colorBtn);
    editActionsLayout->addStretch();
    editActionsLayout->addWidget(m_confirmBtn);
    editActionsLayout->addWidget(m_cancelBtn);
    editActionsLayout->addWidget(m_deleteBtn);

    contentLayout->addLayout(editActionsLayout);

    mainLayout->addLayout(contentLayout, 1);

    m_clickTimer = new QTimer(this);
    m_clickTimer->setSingleShot(true);
    m_clickTimer->setInterval(QApplication::doubleClickInterval()); // double click detection speed based on os
    connect(m_clickTimer, &QTimer::timeout, this, [this]{
        emit SignalManager::instance().timelineSetPosition(m_annotation.start);
    });

    initStyle();
    refreshContent();
}

void AnnotationWidget::initStyle()
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    setFixedHeight(50);
    setAttribute(Qt::WA_StyledBackground, true);
    setContentsMargins(0,0,0,0);
    setStyleSheet("AnnotationWidget{border-style: solid; border: 1px solid " + m_palbtnColorStr + "; border-radius: 4px;}");

    m_updateBtn->setFixedSize(24,24);
    m_updateBtn->setMaximumWidth(20);

    m_deleteBtn->setStyleSheet("QPushButton{"
        "   background-color: rgba(0,0,0,0);"
        "   border: none;"
        "}"
        "QPushButton:hover{"
        "   background-color: tomato;"
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

    // keep the edit fields in sync with the annotation
    if (m_mode == Mode::Edited) {
        m_startEdit->setText(start);
        m_endEdit->setText(end);
        m_nameEdit->setText(m_annotation.name);
        m_noteEdit->setPlainText(m_annotation.note);
        m_editColor = m_annotation.color;
        // remove red borders (from when a conflict occurs)
        m_startEdit->setStyleSheet(QString());
        m_endEdit->setStyleSheet(QString());
        updateColorButton();
    }
}

void AnnotationWidget::setMode(Mode mode)
{

    m_mode = mode;
    bool isEditMode = (m_mode == Mode::Edited);
    
    if (isEditMode)
        refreshContent(); // populates the edit fields since m_mode is already Edited

    m_startLabel->setVisible(!isEditMode);
    m_endLabel->setVisible(!isEditMode);
    m_nameLabel->setVisible(!isEditMode);
    m_noteLabel->setVisible(!isEditMode);

    m_startEdit->setVisible(isEditMode);
    m_endEdit->setVisible(isEditMode);
    m_nameEdit->setVisible(isEditMode);
    m_noteEdit->setVisible(isEditMode);
    m_colorBtn->setVisible(isEditMode);
    m_confirmBtn->setVisible(isEditMode);
    m_cancelBtn->setVisible(isEditMode);
    m_deleteBtn->setVisible(isEditMode);

    if (isEditMode || m_mode == Mode::Extended) {
        // release the fixed height
        setMinimumHeight(50);
        setMaximumHeight(QWIDGETSIZE_MAX);
        adjustNoteEditHeight();
    } else {
        setFixedHeight(50);
    }
}

Annotation AnnotationWidget::editedAnnotation() const
{
    Annotation edited = m_annotation;
    double fps = currentFps();
    edited.start = TimeFormatter::HHMMSSFFToMs(m_startEdit->text(), fps);
    edited.end = TimeFormatter::HHMMSSFFToMs(m_endEdit->text(), fps);
    edited.name = m_nameEdit->text();
    edited.note = m_noteEdit->toPlainText();
    edited.color = m_editColor;
    return edited;
}

void AnnotationWidget::applyEdits()
{
    Annotation updated = editedAnnotation();
    setMode(Mode::Minimized);
    emit updateAnnotationRequested(updated);
}

void AnnotationWidget::openColorPicker()
{
    IconHelper::execColorPickerMenu(this, m_colorBtn, [this](const QColor& color){
        m_editColor = color;
        updateColorButton();
    });
}

void AnnotationWidget::updateColorButton()
{
    m_colorBtn->setStyleSheet("QPushButton{"
        "   background-color: " + m_editColor.name() + ";"
        "   border: 1px solid palette(mid);"
        "   border-radius: 4px;"
        "}");
}

void AnnotationWidget::adjustNoteEditHeight()
{
    if (m_mode == Mode::Minimized)
        m_noteLabel->setWordWrap(false);
    else if (m_mode == Mode::Edited){
        int chrome = m_noteEdit->height() - m_noteEdit->viewport()->height();
        int docHeight = qCeil(m_noteEdit->document()->size().height());
        m_noteEdit->setFixedHeight(qMax(80, docHeight + chrome));
    }else {
        m_noteLabel->setWordWrap(true);
    }

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
    return 1.0;
}

void AnnotationWidget::enterEvent(QEnterEvent *event)
{
    setStyleSheet("AnnotationWidget{border-style: solid; border: 2px solid " + m_palbtnColorStr + "; border-radius: 3px;}");
}

void AnnotationWidget::leaveEvent(QEvent *event)
{
    setStyleSheet("AnnotationWidget{border-style: solid; border: 1px solid " + m_palbtnColorStr + "; border-radius: 4px;}");
}

void AnnotationWidget::mouseReleaseEvent(QMouseEvent *event)
{    
    if(m_ignoreNextRelease){
        m_ignoreNextRelease = false;
        return;
    }

    m_clickTimer->start();
}

void AnnotationWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    // prevent mouse release event from happening
    m_clickTimer->stop();
    m_ignoreNextRelease = true;
    
    if (m_mode == Mode::Minimized){
        setMode(Mode::Extended);
    }else if (m_mode == Mode::Extended){
        setMode(Mode::Minimized);
    }
}

