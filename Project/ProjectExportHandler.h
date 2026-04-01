#ifndef PROJECTEXPORTHANDLER_H
#define PROJECTEXPORTHANDLER_H

#include "Timeline/TimelineWidget.h"
#include "Project/Project.h"
#include "Shot.h"

#include <QString>
#include <optional>
#include <QVector>


enum class ExportType {
    TXT,
    PDF,
    PPTX,
    DOCX,
    MP4,
    SRC,
    TagImage,
};

namespace ProjectExportHandler {

    bool exportToTxt( const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToTagImage( const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToPDF( const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportPython(ExportType type ,const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback);
    
    std::optional<ExportType> selectFormatWindow(const QString& extension);
}


#endif
