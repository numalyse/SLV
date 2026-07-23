#ifndef EXTRACTSEQUENCEWIDGET_H
#define EXTRACTSEQUENCEWIDGET_H

#include "Media.h"
#include "SequenceExtractionHelper.h"
#include "TimeEditor.h"
#include "Timeline/ThumbnailWorker.h"
#include "PrefManager.h"
#include "AspectRatioPixmapLabel.h"

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QFileDialog>
#include <QTimer>
#include <QComboBox>

class ExtractSequenceWidget : public QDialog
{
    Q_OBJECT
public:
    explicit ExtractSequenceWidget(const Media& media, QWidget *parent = nullptr, int startTime = 0, int endTime = -1, int audioTrack = 0);

    void createButtons();
    void initUiLayout();
    void requestStartFrameDisplay();
    void requestEndFrameDisplay();

public slots:
    void onStartTimeChanged(const int newTime);
    void onEndTimeChanged(const int newTime);
    void onThumbnailReady(ThumbnailWorker::Requester requester, int requestId, const QImage& image);
    void confirmExtraction(SequenceExtractionHelper::ExtractionType type);

private:
    const Media& m_media;
    int m_startTime;
    int m_endTime;
    bool m_isExec;
    int m_audioTrack;
    TimeEditor* m_startTimeEditor;
    TimeEditor* m_endTimeEditor;
    AspectRatioPixmapLabel* m_startFrameDisplay;
    AspectRatioPixmapLabel* m_endFrameDisplay;
    double m_thumbnailWidth;
    double m_thumbnailHeight;
    ThumbnailWorker* m_thumbnailWorker;
    QTimer* m_thumbnailStartTimer;
    QTimer* m_thumbnailEndTimer;
    int m_thumbnailPendingTime;
    QPushButton* m_okButton;
    QComboBox* m_methodChoice;
    QPushButton* m_cancelButton;
    QLabel* m_warningMsg;
    QString m_selectedPath;

signals:

};

#endif // EXTRACTSEQUENCEWIDGET_H
