
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Project.h"
#include "FileCopyThread.h"
#include "PlayerWidget.h"
#include "Timeline/TimelineWidget.h"

#include <External/nlohmann/json.hpp>

#include <expected>

struct ProjectSaveData {
    QString mediaName;
    QString mediaAbsolutePath;
    int64_t duration = 0;
    double fps = 0.0;
    QVector<Shot> shots;
};

class ProjectManager : public QObject
{
Q_OBJECT

public:

    enum class Error {
        FolderNotFound,
        JsonFileNotFound,
        CannotOpenJsonFile,
        JsonParsingError,
        MediaKeyMissing,
        MediaFileNotFound,
        UnexpectedError,
        MismatchDuration,
        MismatchFPS,
    };
    Q_ENUM(Error)


    static ProjectManager& instance() {
        static ProjectManager _instance;
        return _instance;
    };

    void requestProjectCreation(const QStringList& mediaPaths);

    void openProject();

    void saveProject(bool ejectMedia);

    void setTimeline(TimelineWidget* tl) { p_timeline = tl;}

    Project* projet(){ return m_project;}

    bool needSave() {
        if(!m_project) return false;
        return m_needSave;
    }

    void deleteProject();

    void discardAndEject();

public slots:
    void setSaveNeeded();

signals : 
    void loadMediaProjectRequested(const QStringList );

private:
    explicit ProjectManager(QObject* parent = nullptr);
    ~ProjectManager();

    Project* m_project = nullptr;
    TimelineWidget* p_timeline = nullptr;
    bool m_needSave = false;
    bool m_isDurationParsed = false;
    bool m_isFpsParsed = false;

    FileCopyThread* m_fileCpyThread = nullptr;

    void initProjectShot();
    bool createProjectFolder();
    void deleteFolder(const QString &projectFolderPath);
    bool copyMedia(const QString &sourcePath, const QString &destPath, bool ejectMediaOnEnd);
    nlohmann::json writeShotsData();
    nlohmann::json writeMediaData();
    bool writeJson();
    QString getErrorMessage(Error error) const;
    std::expected<ProjectSaveData, Error> loadProject(const QString& projectPath);
    void checkMediaFullyLoaded();

signals:
    void projectInitialized();
    void projectDeleted();
    void copyDone();
    void copyError();
    void ejectMedia();
};

#endif