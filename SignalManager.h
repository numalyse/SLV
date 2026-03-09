#ifndef SIGNALMANAGER_H
#define SIGNALMANAGER_H


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
};



#endif // SIGNALMANAGER_H
