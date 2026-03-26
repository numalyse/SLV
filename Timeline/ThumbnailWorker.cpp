#include "ThumbnailWorker.h"

#include <QDebug>

ThumbnailWorker::ThumbnailWorker(const QString &mediaPath, QObject *parent) : QThread(parent), m_videoPath{mediaPath}
{
}

ThumbnailWorker::~ThumbnailWorker() {
    stop();
}

void ThumbnailWorker::requestThumbnail(int shotId, int64_t msStart, int64_t lenghtMs)
{
    // verrouille le temps de mettre une image dans la queue
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue({shotId, msStart, lenghtMs});
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
    cv::VideoCapture cap(m_videoPath.toStdString(), cv::CAP_FFMPEG);
 
    if (!cap.isOpened()) {
        qCritical() << "Impossible de lire la video pour charger des thumbnails";
        return;
    }

    int frameCount = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));
    double fps = cap.get(cv::CAP_PROP_FPS); 
    qDebug() << "Total frames: " << frameCount << ", FPS: " << fps;

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


        int64_t offset = std::round((static_cast<double>(m_frameOffset) * 1000.0) / fps);
        double msStart = req.msStart;
        if( req.msStart + req.shotLength > req.msStart + offset){
            msStart = req.msStart + offset;
        }

        cap.set(cv::CAP_PROP_POS_MSEC, static_cast<double>(msStart));
        
        cv::Mat frame;
        if (cap.read(frame)) {

            int origWidth = frame.cols;
            int origHeight = frame.rows;

            double scaleWidth = static_cast<double>(m_targetSize.width) / origWidth;
            double scaleHeight = static_cast<double>(m_targetSize.height) / origHeight;

            double scale = std::min(scaleWidth, scaleHeight);

            cv::Size newSize(
                static_cast<int>(origWidth * scale),
                static_cast<int>(origHeight * scale)
            );

            cv::Mat resized;
            cv::resize(frame, resized, newSize, 0, 0, cv::INTER_AREA);

            cv::cvtColor(resized, resized, cv::COLOR_BGR2RGB);

            QImage img(resized.data, resized.cols, resized.rows, resized.step, QImage::Format_RGB888);
            QImage finalImg = img.copy(); // utiliser .copy() car les données de resized vont être détruites

            emit thumbnailReady(req.shotId, finalImg);
        }

    }

}
