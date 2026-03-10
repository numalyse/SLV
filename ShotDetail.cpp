#include "ShotDetail.h"
#include "TimeFormatter.h"
#include "ProjectManager.h"
#include "SignalManager.h"

#include <QLabel>
#include <QVBoxLayout>

ShotDetail::ShotDetail(QWidget *parent) : QWidget(parent)
{
    m_titleLabel = new QLabel(this);
    m_endLabel = new QLabel(this);
    m_startLabel = new QLabel(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_endLabel);
    layout->addWidget(m_startLabel);

    connect(&SignalManager::instance(), &SignalManager::timelineWidgetUpdateShotDetail, this, &ShotDetail::updateShotDetail);

}

void ShotDetail::updateShotDetail(Shot * shotData)
{
    double fps = ProjectManager::instance().projet()->media->fps();
    
    m_titleLabel->setText(shotData->title);
    m_startLabel->setText(TimeFormatter::msToHHMMSSFF(shotData->start, fps));
    m_endLabel->setText(TimeFormatter::msToHHMMSSFF(shotData->end, fps));
}
