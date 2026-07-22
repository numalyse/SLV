
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Project/Project.h"

#include "PlayerWidget.h"
#include "Timeline/TimelineWidget.h"
#include "Project/ProjectFileHelper.h"
#include "Project/ProjectExportHelper.h"

#include "AnnotationManager.h"

#include <expected>

#include <QPointer>
#include <QThread>

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

    /// @brief Deletes the current project and, if a single media is provided, creates a new one.
    ///
    /// Called on every update of the GlobalPlayerManager container, this method always
    /// deletes the current project. If @p mediaPaths contains exactly one media, a new
    /// project is created; once the fps and duration are parsed, it is initialized with
    /// one shot.
    ///
    /// @param mediaPaths List of media paths (a new project is only created if it contains a single one).
    void requestProjectCreation(const QStringList& mediaPaths);

    /// @brief Opens a dialog to select a project folder.
    ///
    /// If the user cancels the dialog, returns without doing anything.
    /// Otherwise, forwards the selected path to openProjectFromPath().
    void openProject();

    /// @brief Loads a project from the given path, handling missing files.
    ///
    /// Tries to load the project JSON. If the JSON is not found, prompts the user to
    /// relink it, then attempts to reload. Once loaded, resolves the media link; if the
    /// link is missing or cannot be resolved, prompts the user to relink the media.
    ///
    /// @param folderPath Path to the project folder.
    void openProjectFromPath(const QString& folderPath);

    /// @brief Saves the current project.
    ///
    /// If the project was never created, asks the user for a folder name. Then creates
    /// and writes the JSON, and creates a symbolic link to the media.
    ///
    /// @param ejectMedia If true, ejects the media and empties the current project after saving.
    /// @param isSaveAs If true (call coming from the "Save As" option), forces the creation of a new project folder and a new media link.
    void saveProject(bool ejectMedia, bool isSaveAs = false);

    /// @brief Exports the current project shots or annotations.
    ///
    /// Prompts the user to choose an export format and source, then asks for a
    /// destination path (defaulting to the project folder if it exists, otherwise
    /// to the last export path from the preferences). Returns without doing anything
    /// if there is no timeline, project, or media, or if the user cancels.
    void exportProject();

    void setTimeline(TimelineWidget* tl) { p_timeline = tl;}

    Project* project(){ return m_project;}

    Media* media();
    QString mediaPath();
    QString mediaPathExtension();

    AnnotationManager* annotationManager() {return m_annotationManager; };

    bool needSave() {
        if(!m_project) return false;
        return m_needSave;
    }

    /// @brief Deletes the current project and its media resetting the manager state.
    ///
    /// Deletes the current project and its media and annotations then emits a signal to close and delete the timeline
    /// Does nothing if there is no current project.
    void deleteProject();

    /// @brief Discards the current project and ejects the media.
    void discardAndEject();

public slots:
    void setSaveNeeded();
    void setSaveNotNeeded();

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

    /// @brief owns a pointer to the timeline, so when exporting, retrieve shots data from the timeline
    TimelineWidget* p_timeline = nullptr;

    AnnotationManager* m_annotationManager = nullptr;

    // store thread as members, so when destructor is called we can wait for the threads to end
    // QPointers so automatically set themselves to nullptr when destroyed
    QPointer<QThread> m_exportThread;
    QPointer<QThread> m_fileCpyThread;

    /// @brief Initializes the project with a single shot covering the whole media.
    ///
    /// Waits until both the fps and the duration are parsed before running, and runs
    /// only once. Creates a shot spanning the full media duration,  
    /// emits projectInitialized() so the timeline can be created,
    /// and binds the annotation manager to the project's annotations
    void initProjectShot();

    /// @brief Prompts the user for a location and creates the project folder.
    ///
    /// Opens a save dialog, creates the folder if it doesn't exist, and stores its path
    /// and name in the current project.
    /// 
    /// @return True if the folder was created (or already existed), false if the user cancelled or the creation failed.
    bool createProjectFolder();

    /// @brief Prompts the user to relink a missing media file.
    ///
    /// Shows a warning, asks the user to pick the media, then rebuilds the media link
    /// inside the current project folder (in case the folder itself was moved).
    /// @param projectPath Path to the project folder.
    /// @param projectData Project save data, updated with the new media link.
    /// @return True if the media was relinked, false if the user cancelled.
    bool relinkMedia(const QString& projectPath, ProjectSaveData& projectData);

    /// @brief Prompts the user to relink a missing or invalid project JSON.
    ///
    /// Shows the error, asks the user to pick a JSON file, then renames it to the
    /// expected project JSON path.
    /// @param errorJson Error message describing why the JSON couldn't be loaded.
    /// @param projectPath Path to the project folder.
    /// @return True if the JSON was relinked, false if the user cancelled or the rename failed.
    bool relinkJson(const QString &errorJson, const QString &projectPath);

    /// @brief Returns the message associated with a project file error.
    /// @param error The project file error.
    QString getErrorMessage(ProjectFileError error) const;

    /// @brief Returns the message associated with a project manager error.
    /// @param error The project manager error.
    QString getErrorMessage(ProjectManager::Error error) const;

    /// @brief Checks whether the media is fully loaded and finalizes the project load.
    ///
    /// Once the fps and duration are parsed, if there is no mismatch, requests the media
    /// load and initializes the project. Otherwise prompts the user about the fps/duration
    /// mismatch, removes the media link, and deletes the project resetting its states.
    void checkMediaFullyLoaded();

signals:
    void projectInitialized();
    void projectDeleted();
    void copyDone();
    void copyError();
    void ejectMedia();
    void enableSaveButton();    
    void disableSaveButton();
    void loadMediaProjectRequested(const QStringList );
};

#endif
