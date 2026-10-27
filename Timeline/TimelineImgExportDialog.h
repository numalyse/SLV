#ifndef TIMELINEIMGEXPORTDIALOG_H
#define TIMELINEIMGEXPORTDIALOG_H

#include <QDialog>
#include <QString>

class QSpinBox;
class QCheckBox;
class QLineEdit;
class QRadioButton;

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
    QPushButton* m_okBtn = nullptr;

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