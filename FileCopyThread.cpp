#include "FileCopyThread.h"

#include <QFile>

FileCopyThread::FileCopyThread(const QString &srcPath, const QString &dstPath, QObject *parent)
: QThread(parent), m_src{srcPath}, m_dst{dstPath}
{
}

void FileCopyThread::run()
{

    QFile srcFile(m_src);
    QFile dstFile(m_dst);

    if( ! srcFile.open(QIODevice::ReadOnly)){
        emit errorOccured("Impossible d'ouvrir le fichier source");
        emit copyFinished(false);
        return;
    }
    if( ! dstFile.open(QIODevice::WriteOnly)){
        srcFile.close();
        emit errorOccured("Impossible d'ouvrir le fichier de destination");
        emit copyFinished(false);
        return;
    }

    qint64 totalSize = srcFile.size();
    qint64 copiedSize = 0;

    emit progress(0);

    int lastPercent = -1;
    while ( !srcFile.atEnd())
    {   
        if(isInterruptionRequested()){
            srcFile.close();
            dstFile.close();
            QFile::remove(m_dst);
            emit copyFinished(false);
            return;
        }

        QByteArray buffer = srcFile.read(c_chunkSize);
        qint64 written = dstFile.write(buffer);
        if (written < 0) {
            srcFile.close();
            dstFile.close();
            QFile::remove(m_dst);
            emit copyFinished(false);
            return;
        }

        copiedSize += written;

        if (totalSize > 0) {
            int currentPercent = (copiedSize * 100) / totalSize;
            if (currentPercent > lastPercent) {
                emit progress(currentPercent);
                lastPercent = currentPercent;
            }
        }

    }
    
    srcFile.close();
    dstFile.close();
    emit copyFinished(true);
}
