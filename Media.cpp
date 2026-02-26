#include "Media.h"
#include <qurl.h>


Media::Media(const QString &filePath){

    libvlc_instance_t *tempVlc;
    m_filePath = filePath;
    QFile f(m_filePath);
    qDebug() << "Fichier existe ?" << f.exists();

    QUrl url = QUrl::fromLocalFile(m_filePath);
    QByteArray urlBytes = url.toString(QUrl::FullyEncoded).toUtf8();

    libvlc_media_t *media = libvlc_media_new_location(tempVlc, urlBytes.constData());

    QFileInfo *fileInfo = new QFileInfo(filePath);
    m_name = fileInfo->baseName();
    m_duration = libvlc_media_get_duration(media);

    libvlc_media_release(media);
    libvlc_release(tempVlc);
}

