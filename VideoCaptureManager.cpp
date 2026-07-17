#include "VideoCaptureManager.h"

void VideoCaptureManager::setMediaPath(const QString& filePath)
{
    m_mediaFile = QFileInfo(filePath);
}

void VideoCaptureManager::initMediaTempDirectory()
{
    if(!m_mediaFile.exists()) return;

    deleteMediaTempDirectory();

    m_tempDir = new QTemporaryDir(QDir::tempPath() + "/SLV_" + m_mediaFile.baseName() + "_XXXXXX");
    if(!m_tempDir->isValid()){
        qWarning() << "initMediaTempDirectory: failed to create temp directory:" << m_tempDir->errorString();
        delete m_tempDir;
        m_tempDir = nullptr;
        return;
    }

    m_concatMediaPath = m_tempDir->path();
    m_concatFile = new QFile(m_concatMediaPath + '/' + m_mediaFile.baseName() + ".txt");
    qDebug() << "TEMP DIRECTORY PATH : " << m_concatMediaPath;
}

void VideoCaptureManager::deleteMediaTempDirectory()
{
    if(m_concatFile){
        delete m_concatFile;
        m_concatFile = nullptr;
    }
    if(m_tempDir){
        delete m_tempDir;
        m_tempDir = nullptr;
    }
    m_concatMediaPath.clear();
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
    SequenceExtractionHelper* sequenceExtraction = new SequenceExtractionHelper(m_mediaFile.filePath(), m_startRecordTime, cutTime);
    sequenceExtraction->extractSequence(m_mediaFile.filePath(), m_startRecordTime, cutTime, concatVideoName);
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
    SequenceExtractionHelper *sequenceExtractor = new SequenceExtractionHelper(m_mediaFile.filePath(), m_startRecordTime, endTime);
    sequenceExtractor->extractSequenceLossless(concatVideoName);
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
