
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Project/Project.h"

#include "PlayerWidget.h"
#include "Timeline/TimelineWidget.h"
#include "Project/ProjectFileHelper.h"
#include "Project/ProjectExportHelper.h"

#include <expected>

class ProjectManager : public QObject
{
Q_OBJECT

public:

    enum class Error {
        MismatchDuration,
        MismatchFPS,
        UnexpectedError
    };
    Q_ENUM(Error)

    static ProjectManager& instance() {
        static ProjectManager _instance;
        return _instance;
    };

    void requestProjectCreation(const QStringList& mediaPaths);

    void openProject();
    void openProjectFromPath(const QString&);

    void saveProject(bool ejectMedia, bool isSaveAs = false);
    void exportProject();

    void setTimeline(TimelineWidget* tl) { p_timeline = tl;}

    Project* projet(){ return m_project;}

    Media* media();
    QString mediaPath();
    QString mediaPathExtension();

    bool needSave() {
        if(!m_project) return false;
        return m_needSave;
    }

    void deleteProject();

    void discardAndEject();

public slots:
    void setSaveNeeded();
    void setSaveNotNeeded();

signals : 
    void loadMediaProjectRequested(const QStringList );

private:
    explicit ProjectManager(QObject* parent = nullptr);
    ~ProjectManager();

    Project* m_project = nullptr;
    bool m_needSave = false;
    bool m_isDurationParsed = false;
    bool m_durationError = false;
    bool m_isFpsParsed = false;
    bool m_fpsError = false;    
    bool m_projectInitialized = false;

    TimelineWidget* p_timeline = nullptr;

    void initProjectShot();
    bool createProjectFolder();
    void deleteFolder(const QString &projectFolderPath);
    bool copyMedia(const QString& sourcePath, const QString& destPath, const QString& projectPath, bool ejectMediaAfterSave);

    bool relinkMedia(const QString& projectPath, ProjectSaveData& projectData);

    QString getErrorMessage(ProjectFileError error) const;
    QString getErrorMessage(ProjectManager::Error error) const;

    void checkMediaFullyLoaded();

signals:
    void projectInitialized();
    void projectDeleted();
    void copyDone();
    void copyError();
    void ejectMedia();
    void enableSaveButton();    
    void disableSaveButton();
};

#endif
