#include "ThumbnailWorker.h"

#include <QDebug>

ThumbnailWorker::ThumbnailWorker(QObject *parent) : QThread(parent)
{
}

ThumbnailWorker::~ThumbnailWorker() {
    stop();
}

void ThumbnailWorker::requestThumbnail(int shotId, int64_t msStart, int64_t lenghtMs, const QString& mediaPath, QSize targetSize)
{
    // verrouille le temps de mettre une image dans la queue
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue({shotId, msStart, lenghtMs, mediaPath, targetSize});
    m_condition.wakeOne(); // reveille si on est en train d'attendre que la queue se remplisse
}

void ThumbnailWorker::stop()
{
    m_stop = true;
    m_condition.wakeOne();
    wait();
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
            cap.open(req.videoPath.toStdString(), cv::CAP_FFMPEG);
            if (!cap.isOpened()) {
                qCritical() << "Thumbnail : Impossible de lire la video pour charger des thumbnails";
                return;
            }
            //qDebug() << "Thumbnail : changement de média";
            previousMediaPath = req.videoPath;
            fps = cap.get(cv::CAP_PROP_FPS);
        }


        int64_t offset = std::round((static_cast<double>(m_frameOffset) * 1000.0) / fps);
        double msThumbnail = req.msStart;
        if( req.msStart + req.shotLength > req.msStart + offset){
            msThumbnail = req.msStart + offset;
        }

        cap.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(msThumbnail));
        
        cv::Mat frame;
        if (cap.read(frame)) {

            int origWidth = frame.cols;
            int origHeight = frame.rows;

            cv::Size cvTargetSize(req.targetSize.width(), req.targetSize.height());

            double scaleWidth = static_cast<double>(cvTargetSize.width) / origWidth;
            double scaleHeight = static_cast<double>(cvTargetSize.height) / origHeight;

            double scale = std::min(scaleWidth, scaleHeight);

            cv::Size newSize(
                static_cast<int>(origWidth * scale),
                static_cast<int>(origHeight * scale)
            );

            cv::Mat resized;
            cv::resize(frame, resized, newSize, 0, 0, cv::INTER_AREA);

            QImage img(resized.data, resized.cols, resized.rows, resized.step, QImage::Format_BGR888);
            QImage finalImg = img.copy(); // utiliser .copy() car les données de resized vont être détruites

            //qDebug() << "Thumbnail : prete pour le plan : " <<req.shotId ;
            emit thumbnailReady(req.shotId, finalImg);
        }else {
            qDebug() << "Thumbnail:  Impossible de lire l'image pour le plan : " << req.shotId ;
        }

    }

}
