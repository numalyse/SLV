
#include "ShotDetail.h"
#include "TimeFormatter.h"
#include "ProjectManager.h"
#include "SignalManager.h"
#include "TextManager.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QSizePolicy>


ShotDetail::ShotDetail(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    TextManager& textManager = TextManager::instance();

    m_shotIdForm = new FormLineEditWidget(textManager.get("shot_detail_id_name") , "", false, this);
    m_shotTitle = new FormLineEditWidget(textManager.get("shot_detail_title_name") , "", true, this);
    m_startTime = new FormLineEditWidget(textManager.get("shot_detail_start_time_name") , "", false, this);
    m_endTime = new FormLineEditWidget(textManager.get("shot_detail_end_time_name"), "", false, this);
    m_duration = new FormLineEditWidget(textManager.get("shot_detail_duration_time_name") , "", false, this);
    m_notes = new FormTextEditWidget(textManager.get("shot_detail_note_name") , "", true, this);

    m_notes->setMaximumHeight(250);

    connect(m_notes->textEdit(), &QTextEdit::textChanged, this, [this](){
        if(m_shotData) {
            QString text = m_notes->textEdit()->toPlainText();
            m_shotData->note = text;
        }
    });

    connect(m_shotTitle->lineEdit(), &QLineEdit::textChanged, this, [this](const QString &newTitle){
        if(m_shotData) {
            m_shotData->title = newTitle;
        }
    });


    QFrame* frameButtonsActions = new QFrame(this);
    QGridLayout* frameButtonsLayout = new QGridLayout(frameButtonsActions);

    m_toPrevShotBtn = new ToolbarButton(this, "to_prev_shot_white", "tooltip_to_prev_shot");
    connect(m_toPrevShotBtn, &ToolbarButton::clicked, this, [this](){
        emit goToShotRequested(m_shotId - 1);
    });
    frameButtonsLayout->addWidget(m_toPrevShotBtn, 0, 0);

    m_toNextShotBtn = new ToolbarButton(this, "to_next_shot_white", "tooltip_to_next_shot");
    connect(m_toNextShotBtn, &ToolbarButton::clicked, this, [this](){
        emit goToShotRequested(m_shotId + 1);
    });
    frameButtonsLayout->addWidget(m_toNextShotBtn, 0, 1);

    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(m_shotIdForm);
    m_layout->addWidget(m_shotTitle);
    m_layout->addWidget(m_startTime);
    m_layout->addWidget(m_endTime);
    m_layout->addWidget(m_duration);
    m_layout->addWidget(m_notes);
    m_layout->addWidget(frameButtonsActions);
    m_layout->addStretch();

}

void ShotDetail::updateShotDetail(int shotCount, int shotId, Shot * shotData)
{
    m_shotData = shotData;
    m_shotId = shotId;

    double fps = ProjectManager::instance().projet()->media->fps();
    int64_t duration = shotData->end - shotData->start;

    m_shotIdForm->setText(TextManager::instance().get("shot_detail_id_text") + QString::number(shotId + 1));
    m_shotTitle->setText(shotData->title);
    m_startTime->setText(TimeFormatter::msToHHMMSSFF(shotData->start, fps));
    m_endTime->setText(TimeFormatter::msToHHMMSSFF(shotData->end, fps));
    m_duration->setText(TimeFormatter::msToHHMMSSFF(duration, fps));
    m_notes->setText(shotData->note);

    m_toNextShotBtn->show();
    m_toPrevShotBtn->show();
    if(shotId == 0){
        m_toPrevShotBtn->hide();
    }
    if(shotId == shotCount-1){
        m_toNextShotBtn->hide();
    }


}


