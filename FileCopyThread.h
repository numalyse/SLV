#ifndef FILECOPYTHREAD_H
#define FILECOPYTHREAD_H


#include <QThread>
#include <QString>

class FileCopyThread : public QThread
{
Q_OBJECT

public:
    explicit FileCopyThread(const QString& srcPath, const QString& dstPath, QObject* parent = nullptr);

    void run() override;

signals:
    void errorOccured(const QString& errorMsg);
    void progress(int);
    void copyFinished(bool success, bool isCancelled = false);

private:
    QString m_src;
    QString m_dst;
    
    const qint64 c_chunkSize = 1024 * 1024 * 50; // 50 Mo

};


#endif