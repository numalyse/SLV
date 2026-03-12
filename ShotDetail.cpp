#include "ShotDetail.h"
#include "TimeFormatter.h"
#include "ProjectManager.h"
#include "SignalManager.h"

#include <QLabel>
#include <QVBoxLayout>

ShotDetail::ShotDetail(QWidget *parent) : QWidget(parent)
{
    m_shotIdLabel = new QLabel(this);
    m_titleLabel = new QLabel(this);
    m_endLabel = new QLabel(this);
    m_startLabel = new QLabel(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_shotIdLabel);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_startLabel);
    layout->addWidget(m_endLabel);

    connect(&SignalManager::instance(), &SignalManager::timelineWidgetUpdateShotDetail, this, &ShotDetail::updateShotDetail);

}

void ShotDetail::updateShotDetail(int shotId, Shot * shotData)
{
    double fps = ProjectManager::instance().projet()->media->fps();
    m_shotIdLabel->setText(QString("Plan n°%1").arg(shotId+1));
    m_titleLabel->setText(shotData->title);
    m_startLabel->setText(TimeFormatter::msToHHMMSSFF(shotData->start, fps));
    m_endLabel->setText(TimeFormatter::msToHHMMSSFF(shotData->end, fps));
}
