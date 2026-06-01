#ifndef FILEFORMATMANAGER_H
#define FILEFORMATMANAGER_H

#include <QJsonObject>

class FileFormatManager
{
public:
    static FileFormatManager& instance(){
        static FileFormatManager _instance;
        return _instance;
    }

    const QJsonObject getSupportedFormatsJson(){ return m_formatsJson; }

    const bool isFormatAccepted(const QString& format);

    const QString getOpenFileDialogFilters();

    /// @brief get all formats for a file type
    /// @param type a file type (containers_video, containers_audio, image, etc.)
    /// @returns QString for file dialogs : (*.mp4 *.mov...)
    const QString getFormats(const QString&);

    // /// @brief get all video container formats (mp4, mov, etc.)
    // /// @returns QString for file dialogs : (*.mp4 *.mov...)
    // const QString getVideoContainerFormats();

    // /// @brief get all audio container formats (mp3, wav, etc.)
    // /// @returns QString for file dialogs : (*.mp3 *.wav...)
    // const QString getAudioContainerFormats();

    // /// @brief get all image formats (png, jpeg, etc.)
    // /// @returns QString for file dialogs : (*.png *.jpeg...)
    // const QString getImageFormats();

    // /// @brief get all subtitles formats (srt, ass, etc.)
    // /// @returns QString for file dialogs : (*.srt *.ass...)
    // const QString getSubtitlesFormats();

    // // Pas sûr de l'utilité pour les suivants

    // /// @brief get all streaming formats (https, ftp, etc.)
    // /// @returns QString for file dialogs : (*.https *.ftp...)
    // const QString getStreamingFormats();

    // /// @brief get all video container formats (mpeg4, av1, etc.)
    // /// @returns QString for file dialogs : (*.mpeg4 *.av1...)
    // const QString getVideoCodecFormats();

    // /// @brief get all video container formats (mp3, eac3, etc.)
    // /// @returns QString for file dialogs : (*.mp3 *.eac3...)
    // const QString getAudioCodecFormats();

    /// @brief get a list of all formats for a file type
    /// @param type a file type (containers_video, containers_audio, image, etc.)
    /// @returns QStringList : {"mp4", "mov"...}
    const QStringList getFormatsList(const QString&);

    // /// @brief get a list of all video container formats (mp4, mov, etc.)
    // /// @returns QStringList : {"mp4", "mov"...}
    // const QStringList getVideoContainerFormatsList();

    // /// @brief get a list of all audio container formats (mp3, wav, etc.)
    // /// @returns QStringList : {"mp3", "wav"...}
    // const QStringList getAudioContainerFormatsList();

    // /// @brief get a list of all image formats (png, jpeg, etc.)
    // /// @returns QStringList : {"png", "jpeg"...}
    // const QStringList getImageFormatsList();

    // /// @brief get a list of all subtitles formats (srt, ass, etc.)
    // /// @returns QStringList : {"srt", "ass"...}
    // const QStringList getSubtitlesFormatsList();


    // const QStringList getStreamingFormatsList();
    // const QStringList getVideoCodecFormatsList();
    // const QStringList getAudioCodecFormatsList();

private:
    FileFormatManager();
    QJsonObject m_formatsJson;
};

#endif // FILEFORMATMANAGER_H
