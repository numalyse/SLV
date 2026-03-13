#ifndef SHOTDETAIL_H
#define SHOTDETAIL_H

#include "Shot.h"
#include "FormTextEditWidget.h"
#include "FormLineEditWidget.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>

class ShotDetail : public QWidget
{
Q_OBJECT

public:
    explicit ShotDetail(QWidget* parent = nullptr);
    void updateShotDetail(int shotId, Shot *shotData);

signals:
    void goToShotRequested(int id);

private:
    void infoWidget(const QString &name, const QString &text, bool editable);

    QVBoxLayout* m_layout = nullptr;
    Shot* m_shotData = nullptr;
    int m_shotId = -1;

    FormLineEditWidget* m_shotIdForm = nullptr;
    FormLineEditWidget* m_shotTitle = nullptr;
    FormLineEditWidget* m_startTime = nullptr;
    FormLineEditWidget* m_endTime = nullptr;
    FormLineEditWidget* m_duration = nullptr;
    FormTextEditWidget* m_notes = nullptr;

    ToolbarButton* m_toPrevShotBtn = nullptr;
    ToolbarButton* m_toNextShotBtn = nullptr;
    ToolbarButton* m_extractBtn = nullptr;
};





#endif // SHOTDETAIL_H