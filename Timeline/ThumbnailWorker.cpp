#include "ThumbnailWorker.h"
#include "VideoCaptureHelper.h"

#include <QDebug>
#include <QFile>

ThumbnailWorker::ThumbnailWorker(QObject *parent) : QThread(parent)
{
}

ThumbnailWorker::~ThumbnailWorker() {
    stop();
}

void ThumbnailWorker::requestThumbnail(Requester requester, int requestId, int64_t msStart, int64_t lengthMs, const QString& mediaPath, QSize targetSize, double sar)
{
    // verrouille le temps de mettre une image dans la queue
    QMutexLocker locker(&m_mutex);
    QQueue<ThumbnailRequest>& queue = (requester == Requester::ShotDetail ? m_priorityQueue : 
                                       requester == Requester::Color ? m_colorQueue : m_queue);

    // une requete en attente avec le meme requester + id est obsolete (ex : temps modifié avant qu'elle soit traitée), on la remplace
    queue.removeIf([requester, requestId](const ThumbnailRequest& pending){
        return pending.requester == requester && pending.requestId == requestId;
    });

    queue.enqueue({requester, requestId, msStart, lengthMs, mediaPath, targetSize, sar});

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

void ThumbnailWorker::releaseOpenCvCap()
{
    QMutexLocker locker(&m_mutex);
    m_releaseCap = true;
    m_condition.wakeOne(); 
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

            while (m_queue.isEmpty() && m_priorityQueue.isEmpty() && m_colorQueue.isEmpty() && !m_stop && !m_releaseCap) {
                m_condition.wait(&m_mutex); // attend qu'on ajoute un élément / qu'on stop / qu'on release
            }
            
            if (m_stop) break; // si on a été réveillé pour arrêter le thread
            
            if(m_releaseCap){ // release la capture opencv puis attend une requete
                m_queue.clear();
                m_priorityQueue.clear();
                m_colorQueue.clear();
                cap.release();
                previousMediaPath = "";
                m_releaseCap = false;
                continue;
            }

            req = (!m_priorityQueue.empty()) ? m_priorityQueue.dequeue() : 
                  (!m_colorQueue.empty())    ? m_colorQueue.dequeue() : m_queue.dequeue();


        }

        if (req.videoPath != previousMediaPath){
            cap.release();
            // try to open the requested media and provide better diagnostics
            if (!QFile::exists(req.videoPath)){
                qCritical() << "Thumbnail: fichier introuvable:" << req.videoPath;
            }

            bool opened = SLV::openVideoCapture(cap, req.videoPath, "Thumbnail");

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

        SLV::seekToMs(cap, static_cast<double>(msThumbnail), fps);
        
        cv::Mat frame;

        if (!cap.read(frame) || frame.empty()) {
            qWarning() << "[ThumbnailWorker] Impossible de lire la frame au timestamp :" << msThumbnail;
            continue; 
        }

        int origWidth = frame.cols;
        int origHeight = frame.rows;

        double adjustedWidth = (req.sar > 0) ? static_cast<double>(origWidth) * req.sar : origWidth; // prend en compte le pixel aspect ratio pour compenser les pixels rectangulaires

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

        if(req.requester == Requester::Color){
            emit colorReady(req.requester, req.requestId, getImgColor(img));
        }else {
            emit thumbnailReady(req.requester, req.requestId, img.copy()); // copy because img does a shallow copy of the cv::mat otherwise
        }
    }

}



QColor ThumbnailWorker::getImgColor(const QImage &img) const
{

    if (img.isNull())
        return QColor();

    // Parameters for the K-means algorithm
    constexpr int BLACK_THRESHOLD = 25;
    constexpr int NB_STEP = 32;
    constexpr int K = 5;
    constexpr int ITERATIONS = 10;

    const int width = img.width();
    const int height = img.height();

    // Check for black borders (horizontal and vertical) and crop them

    auto isBlackRow = [&](int y)
    {
        int black = 0;
        int samples = 0;

        for (int x = 0; x < width; x += 4)
        {
            QColor c = img.pixelColor(x, y);

            if (c.red() < BLACK_THRESHOLD &&
                c.green() < BLACK_THRESHOLD &&
                c.blue() < BLACK_THRESHOLD)
            {
                black++;
            }

            samples++;
        }

        return black > samples * 0.8;
    };

    auto isBlackColumn = [&](int x)
    {
        int black = 0;
        int samples = 0;

        for (int y = 0; y < height; y += 4)
        {
            QColor c = img.pixelColor(x, y);

            if (c.red() < BLACK_THRESHOLD &&
                c.green() < BLACK_THRESHOLD &&
                c.blue() < BLACK_THRESHOLD)
            {
                black++;
            }

            samples++;
        }

        return black > samples * 0.8;
    };

    int cropTop = 0;
    while (cropTop < height / 4 && isBlackRow(cropTop))
        cropTop++;

    int cropBottom = height - 1;
    while (cropBottom > height * 3 / 4 && isBlackRow(cropBottom))
        cropBottom--;

    if (cropBottom - cropTop < height / 2)
    {
        cropTop = 0;
        cropBottom = height - 1;
    }

    int cropLeft = 0;
    while (cropLeft < width / 4 && isBlackColumn(cropLeft))
        cropLeft++;

    int cropRight = width - 1;
    while (cropRight > width * 3 / 4 && isBlackColumn(cropRight))
        cropRight--;

    if (cropRight - cropLeft < width / 2)
    {
        cropLeft = 0;
        cropRight = width - 1;
    }

    const int usableWidth = cropRight - cropLeft + 1;
    const int usableHeight = cropBottom - cropTop + 1;

    const int stepX = qMax(1, usableWidth / NB_STEP);
    const int stepY = qMax(1, usableHeight / NB_STEP);

    QVector<QColor> samples;

    for (int y = cropTop; y <= cropBottom; y += stepY)
    {
        for (int x = cropLeft; x <= cropRight; x += stepX)
        {
            samples.push_back(img.pixelColor(x, y));
        }
    }

    if (samples.isEmpty())
        return QColor();

    // K-means clustering to find the dominant color

    QVector<QColor> centers;

    for (int i = 0; i < K; ++i)
        centers.push_back(samples[i * samples.size() / K]);

    QVector<int> assignment(samples.size());

    for (int iter = 0; iter < ITERATIONS; ++iter)
    {
        for (int i = 0; i < samples.size(); ++i)
        {
            int bestCluster = 0;
            int bestDistance = INT_MAX;

            for (int c = 0; c < K; ++c)
            {
                int dr = samples[i].red() - centers[c].red();
                int dg = samples[i].green() - centers[c].green();
                int db = samples[i].blue() - centers[c].blue();

                int dist = dr * dr + dg * dg + db * db;

                if (dist < bestDistance)
                {
                    bestDistance = dist;
                    bestCluster = c;
                }
            }

            assignment[i] = bestCluster;
        }

        QVector<int> count(K, 0);
        QVector<int> sumR(K, 0);
        QVector<int> sumG(K, 0);
        QVector<int> sumB(K, 0);

        for (int i = 0; i < samples.size(); ++i)
        {
            int c = assignment[i];

            count[c]++;
            sumR[c] += samples[i].red();
            sumG[c] += samples[i].green();
            sumB[c] += samples[i].blue();
        }

        for (int c = 0; c < K; ++c)
        {
            if (count[c] == 0)
                continue;

            centers[c] = QColor(
                sumR[c] / count[c],
                sumG[c] / count[c],
                sumB[c] / count[c]);
        }
    }

    // Find the cluster with the most samples

    QVector<int> count(K, 0);

    for (int c : assignment)
        count[c]++;

    int bestCluster = 0;

    for (int c = 1; c < K; ++c)
    {
        if (count[c] > count[bestCluster])
            bestCluster = c;
    }

    return centers[bestCluster];
}