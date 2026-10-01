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
    m_pendingExtractions = 0;
    m_extractionFailed = false;
    m_allLaunched = false;

    // initMediaTempDirectory();
}

void VideoCaptureManager::mediaCutAndConcat(const int cutTime, const int newTime)
{
    if(!m_mediaFile.exists() || m_startRecordTime == -1) return;

    QString concatVideoName = m_concatMediaPath + '/' + m_mediaFile.baseName() + QString::number(m_concatRecordNumber) + '.' + m_mediaFile.suffix();
    m_concatRecordNumber++;
    SequenceExtractionHelper* sequenceExtraction = new SequenceExtractionHelper(m_mediaFile.filePath(), m_startRecordTime, cutTime);

    connect(sequenceExtraction, &SequenceExtractionHelper::extractionFinished, this, [this, sequenceExtraction](int code){
        if (code != 1) m_extractionFailed = true;
        --m_pendingExtractions;
        sequenceExtraction->deleteLater();
        tryFinishConcat();
    });

    sequenceExtraction->extractSequence(m_mediaFile.filePath(), m_startRecordTime, cutTime, concatVideoName, SequenceExtractionHelper::ExtractionType::Reencode);

    if ( m_concatFile->open(QIODevice::ReadWrite | QIODevice::Append) )
    {
        QTextStream stream( m_concatFile );
        stream << "file '" << concatVideoName.replace("\\", "/")<< "'" << Qt::endl;
        m_concatFile->close();
    }
    m_startRecordTime = newTime;
}

void VideoCaptureManager::tryFinishConcat()
{
    if (!m_allLaunched || m_pendingExtractions > 0) return;   // attendre TOUS les segments

    m_startRecordTime = -1;
    m_allLaunched = false;

    if (m_extractionFailed) {
        deleteMediaTempDirectory();
        emit recordSegmentFailed();
        return;
    }

    emit extractionProgress(95);

    QProcess* concat = SequenceExtractionHelper::concatenateSequences(
                           QFileInfo(*m_concatFile).filePath(), m_savePath);
    connect(concat, &QProcess::finished, this, [this](int code){
        deleteMediaTempDirectory();
        if (code != 0) emit recordSegmentFailed();
        else           emit recordSegmentDone(m_savePath);
    });
}

void VideoCaptureManager::trackSegmentProgress(SequenceExtractionHelper* seq, int duration)
{
    const int idx = m_segProgress.size();
    m_segProgress.append(0.0);
    m_segDuration.append(qMax(duration, 1));

    connect(seq, &SequenceExtractionHelper::progressStep, this, [this, idx](float p){
        m_segProgress[idx] = p;
        emitGlobalProgress();
    });
    connect(seq, &SequenceExtractionHelper::extractionFinished, this, [this, idx](int){
        m_segProgress[idx] = 100.0;
        emitGlobalProgress();
    });
}

void VideoCaptureManager::emitGlobalProgress()
{
    double weighted = 0, total = 0;
    for (int i = 0; i < m_segProgress.size(); ++i) {
        weighted += m_segProgress[i] * m_segDuration[i];
        total    += m_segDuration[i];
    }
    
    emit extractionProgress(total > 0 ? int(weighted / total * 0.95) : 0);
}

// void VideoCaptureManager::endMediaRecording(const int endTime, const QString& savePath)
// {
//     if(!m_mediaFile.exists() || m_startRecordTime == -1) return;

//     if(endTime < m_startRecordTime){
//         emit recordSegmentFailed();
//         return;
//     }

//     const bool isConcatMode = (m_concatRecordNumber != 0);
//     const QString concatVideoName = isConcatMode
//         ? m_concatMediaPath + '/' + m_mediaFile.baseName() + QString::number(m_concatRecordNumber) + '.' + m_mediaFile.suffix()
//         : savePath;

//     SequenceExtractionHelper *sequenceExtractor = new SequenceExtractionHelper(m_mediaFile.filePath(), m_startRecordTime, endTime);
//     sequenceExtractor->extractSequence(m_mediaFile.filePath(), m_startRecordTime, endTime, concatVideoName, SequenceExtractionHelper::ExtractionType::Reencode);

//     connect(sequenceExtractor, &SequenceExtractionHelper::extractionFinished, this,
//             [this, savePath, concatVideoName, isConcatMode](const int exitCode){

//         if(exitCode != 1){
//             emit recordSegmentFailed();
//             return;
//         }

//         // Simple record mode
//         if(!isConcatMode){
//             m_startRecordTime = -1;
//             emit recordSegmentDone(savePath);
//             return;
//         }

//         // Extraction shots mode
//         if ( m_concatFile->open(QIODevice::ReadWrite | QIODevice::Append) )
//         {
//             QTextStream stream( m_concatFile );
//             QString strConcat = concatVideoName;
//             stream << "file '" << strConcat.replace("\\", "/") << "'" << Qt::endl;
//             m_concatFile->close();
//         }
//         m_concatRecordNumber = 0;

//         QProcess* sequenceConcatenate = SequenceExtractionHelper::concatenateSequences(QFileInfo(*m_concatFile).filePath(), savePath);
//         connect(sequenceConcatenate, &QProcess::finished, this, [this, savePath](int exitCode){
//             m_startRecordTime = -1;
//             if(exitCode != 0){
//                 deleteMediaTempDirectory();
//                 emit recordSegmentFailed();
//                 return;
//             }
//             deleteMediaTempDirectory();
//             emit recordSegmentDone(savePath);
//         });
//     });
//}

void VideoCaptureManager::endMediaRecording(const int endTime, const QString& savePath)
{
    if(!m_mediaFile.exists() || m_startRecordTime == -1) return;

    const bool isConcatMode = (m_concatRecordNumber != 0);

    if(endTime < m_startRecordTime){
        m_startRecordTime = -1;
        m_concatRecordNumber = 0;
        if(isConcatMode) deleteMediaTempDirectory();
        emit recordSegmentFailed();
        return;
    }

    const QString concatVideoName = isConcatMode
        ? m_concatMediaPath + '/' + m_mediaFile.baseName() + QString::number(m_concatRecordNumber) + '.' + m_mediaFile.suffix()
        : savePath;

    auto* sequenceExtractor = new SequenceExtractionHelper(m_mediaFile.filePath(), m_startRecordTime, endTime);

    // --- Simple record mode : un seul segment, pas de concat ---
    if(!isConcatMode){
        connect(sequenceExtractor, &SequenceExtractionHelper::extractionFinished, this,
                [this, sequenceExtractor, savePath](const int exitCode){
            sequenceExtractor->deleteLater();
            m_startRecordTime = -1;
            if(exitCode != 1){
                emit recordSegmentFailed();
                return;
            }
            emit recordSegmentDone(savePath);
        });
        sequenceExtractor->extractSequence(m_mediaFile.filePath(), m_startRecordTime, endTime,
                                           concatVideoName, SequenceExtractionHelper::ExtractionType::Reencode);
        return;
    }

    // --- Extraction shots mode : ajouter le dernier segment au fichier concat ---
    if(m_concatFile->open(QIODevice::ReadWrite | QIODevice::Append)){
        QTextStream stream(m_concatFile);
        QString strConcat = concatVideoName;
        stream << "file '" << strConcat.replace("\\", "/") << "'" << Qt::endl;
        m_concatFile->close();
    }
    m_concatRecordNumber = 0;

    m_savePath = savePath;
    m_allLaunched = true;          // plus aucun segment ne sera lancé après celui-ci
    ++m_pendingExtractions;

    connect(sequenceExtractor, &SequenceExtractionHelper::extractionFinished, this,
            [this, sequenceExtractor](const int exitCode){
        if(exitCode != 1) m_extractionFailed = true;
        --m_pendingExtractions;
        sequenceExtractor->deleteLater();
        tryFinishConcat();         // ne concatène que quand TOUS les segments sont finis
    });

    sequenceExtractor->extractSequence(m_mediaFile.filePath(), m_startRecordTime, endTime,
                                       concatVideoName, SequenceExtractionHelper::ExtractionType::Reencode);


}