#ifndef PROJECTFILEHELPER_H
#define PROJECTFILEHELPER_H


#include "Project/Project.h"
#include "Timeline/TimelineWidget.h"

#include <QString>
#include <expected>

struct ProjectSaveData {
    QString mediaName;
    QString mediaAbsolutePath;
    int64_t duration = 0;
    double fps = 0.0;
    QVector<Shot> shots;
};

enum class ProjectFileError {
    FolderNotFound,
    JsonFileNotFound,
    CannotOpenJsonFile,
    JsonParsingError,
    MediaKeyMissing,
    MediaFileNotFound,
    UnexpectedError
};

namespace ProjectFileHelper {
    bool writeJson(const Project* project, TimelineWidget* timeline);
    std::expected<ProjectSaveData, ProjectFileError> loadProject(const QString& projectAbsolutePath);
}


#endif
