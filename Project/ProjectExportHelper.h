#ifndef PROJECTEXPORTHELPER_H
#define PROJECTEXPORTHELPER_H

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


namespace SLV {
    /**
     * @brief Convertit un ExportType en chaîne de caractères formatée.
     * * @param type La valeur de l'énumération ExportType.
     * @return QString Le texte formaté sous la forme "export_type_nomduformat".
     */
    inline QString getExportTypeString(ExportType type) {
        switch (type) {
            case ExportType::TXT:      return "export_type_txt";
            case ExportType::PDF:      return "export_type_pdf";
            case ExportType::PPTX:     return "export_type_pptx";
            case ExportType::DOCX:     return "export_type_docx";
            case ExportType::MP4:      return "export_type_mp4";
            case ExportType::SRC:      return "export_type_src";
            case ExportType::TagImage: return "export_type_tagimage"; 
            default:                   return "export_type_unknown"; 
        }
    }
}


namespace ProjectExportHelper {

    bool exportToTxt( const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToTagImage( const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToPDF( const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportPython(ExportType type ,const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback);
    bool exportVideo(ExportType type ,const QVector<Shot> &shots, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback);

    std::optional<ExportType> selectFormatWindow(const QString& extension);
}


#endif
