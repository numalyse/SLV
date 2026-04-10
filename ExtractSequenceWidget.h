#ifndef EXTRACTSEQUENCEWIDGET_H
#define EXTRACTSEQUENCEWIDGET_H

#include "Media.h"
#include "SequenceExtractionHelper.h"
#include "TimeEditor.h"
#include "Timeline/ThumbnailWorker.h"
#include "PrefManager.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QFileDialog>
#include <QTimer>

class ExtractSequenceWidget : public QDialog
{
    Q_OBJECT
public:
    explicit ExtractSequenceWidget(const Media& media, QWidget *parent = nullptr, int timeStart = 0);

    void createButtons();
    void initUiLayout();
    void requestStartFrameDisplay();
    void requestEndFrameDisplay();

public slots:
    void onStartTimeChanged(const int newTime);
    void onEndTimeChanged(const int newTime);
    void onThumbnailReady(int requestId, const QImage& image);
    void confirmExtraction();

private:
    const Media& m_media;
    int m_startTime;
    int m_endTime;
    bool m_isExec;
    TimeEditor* m_startTimeEditor;
    TimeEditor* m_endTimeEditor;
    QLabel* m_startFrameDisplay;
    QLabel* m_endFrameDisplay;
    ThumbnailWorker* m_thumbnailWorker;
    QTimer* m_thumbnailStartTimer;
    QTimer* m_thumbnailEndTimer;
    int m_thumbnailPendingTime;
    QPushButton* m_okButton;
    QPushButton* m_cancelButton;

signals:

};

#endif // EXTRACTSEQUENCEWIDGET_H
