#ifndef PLAYLISTLOADER_H
#define PLAYLISTLOADER_H

#include <QString>
#include <QFileDialog>

#include "PrefManager.h"
#include "PlaylistItem.h"

namespace SLV {

/// @brief Save current playlist into a new .xspf file compatible with VLC
void savePlaylist(const QVector<PlaylistItem *>& items, const QVector<unsigned int>& sortOrder = {})
{
    QString savePath = QFileDialog::getSaveFileName(nullptr, PrefManager::instance().getText("tooltip_save_playlist"), PrefManager::instance().getPref("Paths", "lp_extract_sequence"),
                                                    PrefManager::instance().getText("file_playlist") + "(*.xspf)");
    // savePath += ".xspf";
    QFile *playlistFile = new QFile(savePath);
    if ( playlistFile->open(QIODevice::ReadWrite | QIODevice::Append) )
    {
        QTextStream stream(playlistFile);
        stream << R"(<?xml version="1.0" encoding="UTF-8"?>)" << "\n"
               << R"(<playlist version="1" xmlns="http://xspf.org/ns/0/">)" << "\n \n"
               << "\t" << R"(<title>Playlist</title>)" << "\n \n"
               << "\t" << R"(<trackList>)" << "\n \n";

        for(size_t IPItem = 0; IPItem < items.size(); ++IPItem){
            unsigned int index = sortOrder.empty() ? IPItem : sortOrder[IPItem];
            stream << "\t \t" << R"(<track>)" << "\n"
                   << "\t \t \t" << R"(<location>file:///)" + items[index]->getPath().toUtf8() + R"(</location>)" << "\n"
                   << "\t \t \t" << R"(<duration>)" + QString::number(items[index]->getDuration()) + R"(</duration>)" << "\n"
                   << "\t \t" << R"(</track>)" << "\n \n";
        }

        stream << "\t" << R"(</trackList>)" << "\n"
               << R"(</playlist>)";

        playlistFile->close();
    }
}

QStringList loadPlaylist(const QString& loadPath)
{   
    QFile *playlistFile = new QFile(loadPath);
    if( !playlistFile->open(QIODevice::ReadOnly) ){
        qDebug() << "Error reading playlist file";
        return {};
    }

    QTextStream stream(playlistFile);
    QString xspfText = stream.readAll();

    // parse the XSPF file

    QRegularExpression re("<location>(?<path>.+)</location>");
    QRegularExpressionMatchIterator i = re.globalMatch(xspfText);
    QStringList pathsList;

    while(i.hasNext()){
        QRegularExpressionMatch match = i.next();
        QString path = match.captured("path");
        path = QUrl(path).toLocalFile();
        #if defined(Q_OS_MAC)
            path.remove(0, 1);
        #endif
        qDebug() << "Loaded path : " << path;
        pathsList << path;
    }

    return pathsList;
}

}

#endif // PLAYLISTLOADER_H
