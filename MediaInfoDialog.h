#ifndef MEDIAINFODIALOG_H
#define MEDIAINFODIALOG_H

#include "Media.h"

#include <QDialog>
#include <QFrame>
#include <QLayout>

class MediaInfoDialog : public QDialog
{

private:
    QWidget* m_mediaInfoWidget;

public:
    MediaInfoDialog(const Media& media);
    QFrame* createFrame(const QString& key, const QString& value);
    QHBoxLayout* createSeparator(const QString& separatorName);
    QString mediaTypeToString(const MediaType type) const;

};

#endif // MEDIAINFODIALOG_H
