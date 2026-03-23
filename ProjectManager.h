
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Project.h"
#include "FileCopyThread.h"
#include "PlayerWidget.h"
#include "Timeline/TimelineWidget.h"

#include <External/nlohmann/json.hpp>

class ProjectManager : public QObject
{
Q_OBJECT

public:

    static ProjectManager& instance() {
        static ProjectManager _instance;
        return _instance;
    };

    void createProject(Media *media);

    void loadProject(const QString& projectPath);

    bool saveProject(bool ejectMedia);

    void setTimeline(TimelineWidget* tl) { p_timeline = tl;}

    Project* projet(){ return m_project;}

    bool needSave() { return m_askSave;}

private:
    explicit ProjectManager(QObject* parent = nullptr);
    ~ProjectManager();

    Project* m_project = nullptr;
    TimelineWidget* p_timeline = nullptr;
    bool m_askSave = true; // TODO : true for tests, set to false by default
    FileCopyThread* m_fileCpyThread = nullptr;

    void initProjectShot(int64_t mediaDuration);
    bool createProjectFolder();
    bool copyMedia(const QString &sourcePath, const QString &destPath, bool ejectMediaOnEnd);
    nlohmann::json writeShotsData();
    nlohmann::json writeMediaData();
    bool writeJson();

signals:
    void projectInitialized();
    void projectDeleted();
    void copyDone();
    void copyError();
    void ejectMedia();
};

#endif