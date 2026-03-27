#ifndef PROJECTEXPORTHANDLER_H
#define PROJECTEXPORTHANDLER_H

#include "Timeline/TimelineWidget.h"
#include "Project/Project.h"

#include <QString>
#include <optional>


enum class ExportType {
    TXT,
    PDF,
    PPTX,
    DOCX,
    MP4,
    SRC,
    TagImage
};

namespace ProjectExportHandler {
    std::optional<ExportType> selectFormatWindow(const QString& extension);
    bool exportProject(const Project* project, TimelineWidget* timeline, ExportType type, const QString& destinationPath);
}


#endif
