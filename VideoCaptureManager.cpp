#include "VideoCaptureManager.h"

void VideoCaptureManager::setMediaPath(const QString& filePath)
{
    m_mediaFile = QFileInfo(filePath);
}

void VideoCaptureManager::initMediaTempDirectory()
{
    if(!m_mediaFile.exists()) return;
    QString newDirPath = m_tempDirectoryPath + '/' + m_mediaFile.baseName();
    if(!QDir(newDirPath).exists()){
        QDir().mkpath(newDirPath);
        m_concatMediaPath = newDirPath;
    }
    else{
        unsigned int ITempDirIndex = 2;
        while(QDir(newDirPath + '_' + QString::number(ITempDirIndex)).exists()){
            ITempDirIndex++;
        }
        m_concatMediaPath = newDirPath + '_' + QString::number(ITempDirIndex);
        QDir().mkpath(m_concatMediaPath);
        m_dirIndex = ITempDirIndex;
    }
    m_concatFile = new QFile(m_concatMediaPath + '/' + m_mediaFile.baseName() + ".txt");
    qDebug() << "TEMP DIRECTORY PATH : " << m_concatMediaPath;
}

void VideoCaptureManager::deleteMediaTempDirectory()
{
    if(!m_mediaFile.exists()) return;
    // Protect against empty paths which would target the current working directory
    if (!m_concatMediaPath.isEmpty()) {
        QDir tempDir(m_concatMediaPath);
        if(tempDir.exists())
            tempDir.removeRecursively();
    } else {
        qWarning() << "deleteMediaTempDirectory: m_concatMediaPath is empty, skipping removal.";
    }

    if (!m_tempDirectoryPath.isEmpty()) {
        QDir concatDir(m_tempDirectoryPath);
        if(concatDir.exists() && concatDir.isEmpty())
            QDir(m_tempDirectoryPath).removeRecursively();
    } else {
        qWarning() << "deleteMediaTempDirectory: m_tempDirectoryPath is empty, skipping removal.";
    }
}

void VideoCaptureManager::startMediaRecording(const int startTime)
{
    if(!m_mediaFile.exists()) return;
    m_startRecordTime = startTime;
    m_concatRecordNumber = 0;
    initMediaTempDirectory();
}

void VideoCaptureManager::mediaCutAndConcat(const int cutTime, const int newTime)
{
    if(!m_mediaFile.exists() || m_startRecordTime == -1) return;

    QString concatVideoName = m_concatMediaPath + '/' + m_mediaFile.baseName() + QString::number(m_concatRecordNumber) + '.' + m_mediaFile.suffix();
    m_concatRecordNumber++;
    SequenceExtractionHelper::extractSequence(m_mediaFile.filePath(), m_startRecordTime, cutTime, concatVideoName);
    if ( m_concatFile->open(QIODevice::ReadWrite | QIODevice::Append) )
    {
        QTextStream stream( m_concatFile );
        stream << "file '" << concatVideoName.replace("\\", "/")<< "'" << Qt::endl;
        m_concatFile->close();
    }
    m_startRecordTime = newTime;
}

void VideoCaptureManager::endMediaRecording(const int endTime, const QString& savePath)
{
    if(!m_mediaFile.exists() || m_startRecordTime == -1) return;

    QString concatVideoName = m_concatRecordNumber != 0 ? m_concatMediaPath + '/' + m_mediaFile.baseName() + QString::number(m_concatRecordNumber) + '.' + m_mediaFile.suffix() : savePath;
    SequenceExtractionHelper *sequenceExtractor = new SequenceExtractionHelper();
    sequenceExtractor->preciseExtractSequence(m_mediaFile.filePath(), m_startRecordTime, endTime, concatVideoName);
    connect(sequenceExtractor, &SequenceExtractionHelper::extractionFinished, this, [this, concatVideoName, savePath](const int exitCode){
        if(exitCode != 1){
            deleteMediaTempDirectory();
            emit recordSegmentFailed();
            return;
        }
        m_startRecordTime = -1;

        if(m_concatRecordNumber == 0) {
            deleteMediaTempDirectory();
            emit recordSegmentDone(savePath);
            return;
        }

        if ( m_concatFile->open(QIODevice::ReadWrite | QIODevice::Append) )
        {
            QTextStream stream( m_concatFile );
            QString strConcat = concatVideoName;
            stream << "file '" << strConcat.replace("\\", "/")<< "'" << Qt::endl;
            m_concatFile->close();
        }
        m_concatRecordNumber = 0;
        QProcess* sequenceConcatenate = SequenceExtractionHelper::concatenateSequences(QFileInfo(*m_concatFile).filePath(), savePath);
        connect(sequenceConcatenate, &QProcess::finished, this, [this, savePath](int exitCode){
            if(exitCode != 0){
                deleteMediaTempDirectory();
                emit recordSegmentFailed();
                return;
            }
            deleteMediaTempDirectory();
            emit recordSegmentDone(savePath);
        });
    });

}
