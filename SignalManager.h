#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H

#include "Shot.h"
#include <QObject.h>

class SignalManager: public QObject
{
Q_OBJECT
public:

    static SignalManager &instance() {
        static SignalManager _instance;
        return _instance;
    };

private:
    SignalManager(QObject* parent = nullptr) : QObject(parent){};

signals:
    void mediaWidgetMediaFinished();
    void timelineWidgetUpdateShotDetail(int, Shot*);
    void extensionToolbarDisplayShotDetail();
    void displayPlaylist();
    void simpleToolbarUpdateCursorPosition(int);
    void timelineSetPosition(int64_t);
};



#endif // SIGNALMANAGER_H
