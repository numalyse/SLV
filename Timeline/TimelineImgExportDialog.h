#ifndef TIMELINEIMGEXPORTDIALOG_H
#define TIMELINEIMGEXPORTDIALOG_H

#include <QDialog>
#include <QString>

class QSpinBox;
class QCheckBox;
class QLineEdit;
class QRadioButton;
class QGraphicsItem;
class QGraphicsView;
class AnnotationItem;
class AudioShotItem;
class AudioVisualizerItem;
class QGraphicsLineItem;
class QGraphicsSimpleTextItem;
class ShotItem;

struct TimelineImgExportOptions
{
    QString filePath;
    bool currentViewOnly = false; 
    int  width = 1920;
    bool showRuler = true;
    bool showAnnotations = true;
    bool showShots = true;    
    bool showThumbnails = true;    
    bool showShotBorders = true;
    bool showAudio = true;
    bool showABMarkers = false;
    bool showCursor = false;
    bool transparentBackground = false;
};

class TimelineImgExportDialog : public QDialog
{
    Q_OBJECT
public:
    explicit TimelineImgExportDialog(const QString& defaultPath, QWidget* parent = nullptr);

    TimelineImgExportOptions options() const;

private slots:
    void browse();

private:
    void setOptions(const TimelineImgExportOptions& o);
    void updateDependencies();
    QGraphicsView* createPreview();
    void updatePreview();
    QPushButton* m_okBtn = nullptr;
    QGraphicsView* m_previewView = nullptr;
    QGraphicsLineItem* m_previewRulerLine = nullptr;
    QGraphicsSimpleTextItem* m_previewRulerStart = nullptr;
    QGraphicsSimpleTextItem* m_previewRulerEnd = nullptr;
    AnnotationItem* m_previewAnnotation = nullptr;
    ShotItem* m_previewFirstShot = nullptr;
    ShotItem* m_previewSecondShot = nullptr;
    AudioShotItem* m_previewFirstAudioShot = nullptr;
    AudioShotItem* m_previewSecondAudioShot = nullptr;
    AudioVisualizerItem* m_previewAudioVisualizer = nullptr;

    QLineEdit* m_pathEdit;
    QSpinBox*  m_widthSpin;
    QCheckBox* m_rulerCheck;
    QCheckBox* m_shotsCheck;
    QCheckBox* m_thumbnailsCheck;
    QCheckBox* m_shotBordersCheck;
    QCheckBox* m_audioCheck;
    QCheckBox* m_annotCheck;
    QCheckBox* m_abCheck;
    QCheckBox* m_cursorCheck;
    QCheckBox* m_transparentCheck;
    QRadioButton* m_currentViewRadio = nullptr;
    QRadioButton* m_fullViewRadio = nullptr;
};

#endif