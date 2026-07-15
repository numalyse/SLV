#include "AnnotationDialog.h"
#include "PrefManager.h"
#include "TimeFormatter.h"
#include "Project/ProjectManager.h"
#include "IconHelper.h"

#include <QColorDialog>
#include <QGridLayout>
#include <QLabel>

AnnotationDialog::AnnotationDialog(QWidget* parent, const Annotation& annotation)
    : QDialog{parent}, m_annotation(annotation), m_color(annotation.color)
{
    setWindowTitle(PrefManager::instance().getText("annotation"));

    Media* media = ProjectManager::instance().media();
    m_maxDuration = media ? media->duration() : 0;
    m_fps = (media && media->fps() > 0.0) ? media->fps() : 1.0;

    m_startEdit = new QLineEdit(TimeFormatter::msToHHMMSSFF(m_annotation.start, m_fps));
    m_endEdit = new QLineEdit(TimeFormatter::msToHHMMSSFF(m_annotation.end, m_fps));
    for(QLineEdit* timeEdit : {m_startEdit, m_endEdit}){
        timeEdit->setInputMask("99:99:99.99");
        timeEdit->setAlignment(Qt::AlignCenter);
        timeEdit->setFixedWidth(90);
    }

    m_nameEdit = new QLineEdit(annotation.name);

    m_noteEdit = new QTextEdit();
    m_noteEdit->setPlainText(m_annotation.note);
    m_noteEdit->setFixedHeight(m_noteHeight);

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

    // start / end / color
    formLayout->addWidget(makeLabel("shot_detail_start_time_name"), 0, 0);
    formLayout->addWidget(m_startEdit, 0, 1);
    formLayout->addWidget(makeLabel("shot_detail_end_time_name"), 0, 2);
    formLayout->addWidget(m_endEdit, 0, 3);

    formLayout->addWidget(makeLabel("tooltip_color_tool"), 0, 4);
    formLayout->addWidget(m_colorBtn, 0, 5, Qt::AlignLeft);

    // note
    formLayout->addWidget(makeLabel("shot_detail_name"), 2, 0, Qt::AlignRight | Qt::AlignTop);
    formLayout->addWidget(m_nameEdit, 2, 1, 1, 5);
    formLayout->addWidget(makeLabel("shot_detail_note_name"), 3, 0, Qt::AlignRight | Qt::AlignTop);
    formLayout->addWidget(m_noteEdit, 3, 1, 1, 5);

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
    IconHelper::execColorPickerMenu(this, m_colorBtn, [this](const QColor& color){
        if(color.isValid()){
            m_color = color;
            updateColorButton();
        }
    });
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
    annotation.name = m_nameEdit->text();
    annotation.note = m_noteEdit->toPlainText();
    annotation.color = m_color;

    annotation.start = TimeFormatter::HHMMSSFFToMs(m_startEdit->text(), m_fps);
    annotation.end = TimeFormatter::HHMMSSFFToMs(m_endEdit->text(), m_fps);

    return annotation;
}
