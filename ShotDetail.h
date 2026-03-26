#ifndef SHOTDETAIL_H
#define SHOTDETAIL_H

#include "Shot.h"
#include "FormTextEditWidget.h"
#include "FormLineEditWidget.h"
#include "ToolbarButtons/ToolbarButton.h"

#include <QLabel>
#include <QFrame>
#include <QImage>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QSize>


class ShotDetail : public QWidget
{
Q_OBJECT

public:
    explicit ShotDetail(QWidget* parent = nullptr);
    void updateShotDetail(int shotCount, int shotId, Shot *shotData);
    void toggleShotControlButtons(bool);
    void updateTagImage(QImage);
signals:
    void goToShotRequested(int id);
    void updateImageRequested(int idShot, int64_t time, int64_t length, const QString& mediaPath, const QSize& targetSize);

private:
    void infoWidget(const QString &name, const QString &text, bool editable);

    QVBoxLayout* m_layout = nullptr;
    Shot* m_shotData = nullptr;
    QSize m_imageSize {200, 200};
    int m_shotId = -1;
    bool m_buttonDisabled = false;

    FormLineEditWidget* m_shotIdForm = nullptr;
    FormLineEditWidget* m_shotTitle = nullptr;
    FormLineEditWidget* m_startTime = nullptr;
    FormLineEditWidget* m_endTime = nullptr;
    FormLineEditWidget* m_duration = nullptr;
    FormTextEditWidget* m_notes = nullptr;

    QLabel* m_tagImage = nullptr;

    ToolbarButton* m_toPrevShotBtn = nullptr;
    ToolbarButton* m_toNextShotBtn = nullptr;
    ToolbarButton* m_extractBtn = nullptr;
};





#endif // SHOTDETAIL_H