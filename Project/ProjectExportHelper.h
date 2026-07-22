#ifndef PROJECTEXPORTHELPER_H
#define PROJECTEXPORTHELPER_H

#include "Timeline/TimelineWidget.h"
#include "Project/Project.h"
#include "Shot.h"
#include "Annotation.h"

#include <QString>
#include <optional>
#include <QVector>


enum class ExportType {
    TXT,
    PDF,
    PPTX,
    DOCX,
    CSV,
    MP4,
    SRC,
    TagImage,
};

enum class ExportSource {
    Shots,
    Annotations,
};

/// @brief User choices in the export selection window
struct ExportSelection {
    ExportType type;
    ExportSource source;
};

/// @brief Element that can contain an annotation or a shot
struct ExportItem {
    QString title;
    int64_t start;
    int64_t end;
    int64_t imageTime; // tag image for shot or start of an annotation
    QString imgTxt; // will store imgTxt of a shot or the note of an annotation
    QString soundTxt; // will store soundTxt of a shot or an empty string for an annotation 
};

/// @brief When starting to export, fills this struct, based on the export source.
/// ex : imgTxt when source is shots = label imgTxt,  when source is annotations =  label for note
struct ExportLabels {
    QString item; 
    QString count;       
    QString title;      
    QString startTime;
    QString endTime;
    QString duration;
    QString imgTxt;      
    QString soundTxt;  
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
            case ExportType::CSV:      return "export_type_csv";
            case ExportType::MP4:      return "export_type_mp4";
            case ExportType::SRC:      return "export_type_src";
            case ExportType::TagImage: return "export_type_tagimage"; 
            default:                   return "export_type_unknown"; 
        }
    }

    inline QString getExportExtensionString(ExportType type) {
        switch (type) {
        case ExportType::TXT:      return "txt";
        case ExportType::PDF:      return "pdf";
        case ExportType::PPTX:     return "pptx";
        case ExportType::DOCX:     return "docx";
        case ExportType::CSV:      return "csv";
        case ExportType::MP4:      return "mp4";
        case ExportType::SRC:      return "src";
        case ExportType::TagImage: return "tagframe";
        default:                   return "unknown";
        }
    }
}


namespace ProjectExportHelper {

    /// @brief fills the struct ExportLabels based on the source
    ExportLabels makeExportLabels(ExportSource source);

    QVector<ExportItem> fromShots(const QVector<Shot>& shots);
    QVector<ExportItem> fromAnnotations(const QVector<Annotation>& annotations);

    bool exportToTxt( const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToTagImage( const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToPDF( const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportToCSV( const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, const QString &dstPath, std::function<bool(int)> progressCallback = nullptr);
    bool exportPython(ExportType type ,const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar, const QString &dstPath, std::function<bool(int)> progressCallback);
    bool exportVideo(ExportType type ,const QVector<ExportItem> &items, const ExportLabels &labels, double fps, int64_t duration, const QString &mediaPath, double sar,const QString &dstPath, std::function<bool(int)> progressCallback);

    /// @brief Opens a dialog to select format of export and the source to export shots or annotations, if no annotations in this project, cannot select annotations (disabled)
    std::optional<ExportSelection> selectFormatWindow(const MediaType mediaType, const QString& extension, bool hasAnnotations);
}


#endif
