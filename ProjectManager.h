
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Project.h"


class ProjectManager : public QObject
{
Q_OBJECT

public:

    static class ProjectManager& instance() {
        static ProjectManager _instance;
        return _instance;
    };

    void createProject(Media *media);

    void deleteProject();

    Project* projet(){ return m_project;}



private:
    ProjectManager(QObject* parent = nullptr);
    ~ProjectManager();

    Project* m_project = nullptr;

    void initProjectShot(int64_t mediaDuration);

signals:
    void projectInitialized();
    void projectDeleted();
};

#endif