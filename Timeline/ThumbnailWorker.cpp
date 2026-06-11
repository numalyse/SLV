#include "ThumbnailWorker.h"

#include <QDebug>
#include <QFile>

ThumbnailWorker::ThumbnailWorker(QObject *parent) : QThread(parent)
{
}

ThumbnailWorker::~ThumbnailWorker() {
    stop();
}

void ThumbnailWorker::requestThumbnail(int requestId, int64_t msStart, int64_t lengthMs, const QString& mediaPath, QSize targetSize, double sar)
{
    // verrouille le temps de mettre une image dans la queue
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue({requestId, msStart, lengthMs, mediaPath, targetSize, sar});
    m_condition.wakeOne(); // reveille si on est en train d'attendre que la queue se remplisse
}

void ThumbnailWorker::stop()
{
    m_stop = true;
    m_condition.wakeOne();
    wait();
}

void ThumbnailWorker::clearQueue()
{
    QMutexLocker locker(&m_mutex);
    m_queue.clear();
}

void ThumbnailWorker::keepNQueue(const int n)
{
    QMutexLocker locker(&m_mutex);
    while (m_queue.size() > n) {
        m_queue.dequeue();
    }
}

void ThumbnailWorker::run()
{
    cv::VideoCapture cap;
    QString previousMediaPath = "";
    double fps {};

    while(!m_stop){
        ThumbnailRequest req;

        { // scope du mutex, on veut verrrouiller que quand on retire un element de la queue 
            QMutexLocker locker(&m_mutex);

            while (m_queue.isEmpty() && !m_stop) {
                m_condition.wait(&m_mutex); // attend qu'on ajoute un élément
            }
            
            if (m_stop) break; // si on a été réveillé pour arrêter le thread
            
            req = m_queue.dequeue();
        }

        if (req.videoPath != previousMediaPath){
            cap.release();
            // try to open the requested media and provide better diagnostics
            if (!QFile::exists(req.videoPath)){
                qCritical() << "Thumbnail: fichier introuvable:" << req.videoPath;
            }

            // try opening with a portable path encoding and try macOS AVFoundation first,
            // then let OpenCV pick the backend, finally fallback to FFmpeg
            std::string pathUtf8 = req.videoPath.toUtf8().constData();
            bool opened = false;

            opened = cap.open(pathUtf8, cv::CAP_AVFOUNDATION);
            if (!opened) {
                qWarning() << "Thumbnail: cap.open(CAP_AVFOUNDATION) failed, essayer CAP_ANY...";
                opened = cap.open(pathUtf8, cv::CAP_ANY);
            }
            if (!opened) {
                qWarning() << "Thumbnail: cap.open(CAP_ANY) failed, essayer CAP_FFMPEG...";
                opened = cap.open(pathUtf8, cv::CAP_FFMPEG);
            }

            if (!opened) {
                qCritical() << "Thumbnail: Impossible d'ouvrir la video pour charger des thumbnails:" << req.videoPath
                            << "(exists:" << QFile::exists(req.videoPath) << ")";
                continue;
            } else {
                qDebug() << "Thumbnail: media opened with path:" << req.videoPath;
            }
            //qDebug() << "Thumbnail : changement de média";
            previousMediaPath = req.videoPath;
            fps = cap.get(cv::CAP_PROP_FPS);
        }


        int64_t offsetMs = std::round((static_cast<double>(m_frameOffset) * 1000.0) / fps);
        double msThumbnail = req.msStart;
        if( req.msStart + req.shotLength > req.msStart + offsetMs){ // si la durée du plan est > à l'offset, on peut se decaler de l'offset pour éviter d'avoir une miniature du premier frame du plan
            msThumbnail = req.msStart + offsetMs;
        }

        cap.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(msThumbnail));
        
        cv::Mat frame;

        if (!cap.read(frame) || frame.empty()) {
            qWarning() << "[ThumbnailWorker] Impossible de lire la frame au timestamp :" << msThumbnail;
            continue; 
        }

        int origWidth = frame.cols;
        int origHeight = frame.rows;

        double adjustedWidth = static_cast<double>(origWidth) * req.sar; // prend en compte le pixel aspect ratio pour compenser les pixels rectangulaires

        cv::Size cvTargetSize(req.targetSize.width(), req.targetSize.height());

        double scaleWidth = static_cast<double>(cvTargetSize.width) / adjustedWidth;
        double scaleHeight = static_cast<double>(cvTargetSize.height) / origHeight;

        // On prend la plus petite échelle pour que l'image rentre entièrement (KeepAspectRatio)
        double scale = std::min(scaleWidth, scaleHeight);

        cv::Size newSize(
            static_cast<int>(adjustedWidth * scale),
            static_cast<int>(origHeight * scale)
        );

        cv::Mat resized;
        cv::resize(frame, resized, newSize, 0, 0, cv::INTER_NEAREST);

        QImage img(resized.data, resized.cols, resized.rows, resized.step, QImage::Format_BGR888);

        emit thumbnailReady(req.requestId, img.copy()); // copy because img does a shallow copy of the cv::mat

    }

}
