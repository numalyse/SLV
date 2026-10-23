#ifndef TIMELINEIMGEXPORTDIALOG_H
#define TIMELINEIMGEXPORTDIALOG_H

#include <QDialog>
#include <QString>

class QSpinBox;
class QCheckBox;
class QLineEdit;

struct TimelineImgExportOptions
{
    QString filePath;
    bool currentViewOnly = false; 
    int  width = 4000;
    bool showRuler = true;
    bool showShots = true;
    bool showAudio = true;
    bool showAnnotations = true;
    bool showABMarkers = true;
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
    QLineEdit* m_pathEdit;
    QSpinBox*  m_widthSpin;
    QCheckBox* m_rulerCheck;
    QCheckBox* m_shotsCheck;
    QCheckBox* m_audioCheck;
    QCheckBox* m_annotCheck;
    QCheckBox* m_abCheck;
    QCheckBox* m_cursorCheck;
    QCheckBox* m_transparentCheck;
    QCheckBox* m_currentViewCheck;
};

#endif