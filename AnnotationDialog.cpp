#include "AnnotationDialog.h"
#include "PrefManager.h"
#include "TimeFormatter.h"
#include "Project/ProjectManager.h"

#include <QColorDialog>
#include <QGridLayout>
#include <QLabel>

AnnotationDialog::AnnotationDialog(QWidget* parent, const Annotation& annotation)
    : QDialog{parent}, m_annotation(annotation), m_color(annotation.color)
{
    setWindowTitle(PrefManager::instance().getText("annotation"));

    Media* media = ProjectManager::instance().media();
    m_maxDuration = media ? media->duration() : 0;
    m_fps = (media && media->fps() > 0.0) ? media->fps() : 25.0;

    m_startEdit = new QLineEdit(TimeFormatter::msToHHMMSSFF(m_annotation.start, m_fps));
    m_endEdit = new QLineEdit(TimeFormatter::msToHHMMSSFF(m_annotation.end, m_fps));
    for(QLineEdit* timeEdit : {m_startEdit, m_endEdit}){
        timeEdit->setInputMask("99:99:99.99");
        timeEdit->setAlignment(Qt::AlignCenter);
        timeEdit->setFixedWidth(90);
    }

    m_titleEdit = new QLineEdit(m_annotation.title);

    m_noteEdit = new QTextEdit();
    m_noteEdit->setPlainText(m_annotation.note);
    m_noteEdit->setFixedHeight(80);

    m_colorBtn = new QPushButton();
    m_colorBtn->setFixedSize(60, 24);
    m_colorBtn->setToolTip(PrefManager::instance().getText("tooltip_color_tool"));
    updateColorButton();
    connect(m_colorBtn, &QPushButton::clicked, this, &AnnotationDialog::openColorPicker);

    m_confirmBtn = new QPushButton(PrefManager::instance().getText("confirm"));
    m_cancelBtn = new QPushButton(PrefManager::instance().getText("cancel_action"));
    connect(m_confirmBtn, &QPushButton::released, this, &QDialog::accept);
    connect(m_cancelBtn, &QPushButton::released, this, &QDialog::reject);

    initUiLayout();
}

void AnnotationDialog::initUiLayout()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QGridLayout* formLayout = new QGridLayout();
    formLayout->setHorizontalSpacing(8);
    formLayout->setVerticalSpacing(6);

    auto makeLabel = [](const QString& key){
        QLabel* label = new QLabel("<b>" + PrefManager::instance().getText(key) + " :</b>");
        label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        return label;
    };

    // start / end 
    formLayout->addWidget(makeLabel("shot_detail_start_time_name"), 0, 0);
    formLayout->addWidget(m_startEdit, 0, 1);
    formLayout->addWidget(makeLabel("shot_detail_end_time_name"), 0, 2);
    formLayout->addWidget(m_endEdit, 0, 3);

    // color and title / note 
    formLayout->addWidget(makeLabel("shot_detail_title_name"), 1, 0);
    formLayout->addWidget(m_titleEdit, 1, 1, 1, 2);
    formLayout->addWidget(m_colorBtn, 1, 3, Qt::AlignCenter);

    formLayout->addWidget(makeLabel("shot_detail_note_name"), 2, 0, Qt::AlignRight | Qt::AlignTop);
    formLayout->addWidget(m_noteEdit, 2, 1, 1, 3);

    formLayout->setColumnStretch(1, 1);

    QHBoxLayout* confirmLayout = new QHBoxLayout();
    confirmLayout->addStretch();
    confirmLayout->addWidget(m_confirmBtn);
    confirmLayout->addWidget(m_cancelBtn);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(confirmLayout);
}

void AnnotationDialog::openColorPicker()
{
    QColor color = QColorDialog::getColor(m_color, this, PrefManager::instance().getText("tooltip_color_tool"));
    if(color.isValid()){
        m_color = color;
        updateColorButton();
    }
}

void AnnotationDialog::updateColorButton()
{
    m_colorBtn->setStyleSheet("QPushButton{"
        "   background-color: " + m_color.name() + ";"
        "   border: 1px solid palette(mid);"
        "   border-radius: 4px;"
        "}");
}

Annotation AnnotationDialog::annotation() const
{
    Annotation annotation = m_annotation;
    annotation.title = m_titleEdit->text();
    annotation.note = m_noteEdit->toPlainText();
    annotation.color = m_color;

    annotation.start = TimeFormatter::HHMMSSFFToMs(m_startEdit->text(), m_fps);
    annotation.end = TimeFormatter::HHMMSSFFToMs(m_endEdit->text(), m_fps);

    // clamp times so they do not exceed the media duration
    if(m_maxDuration > 0){
        annotation.start = qMin(annotation.start, m_maxDuration);
        annotation.end = qMin(annotation.end, m_maxDuration);
    }
    if(annotation.end < annotation.start)
        annotation.end = annotation.start;

    return annotation;
}
