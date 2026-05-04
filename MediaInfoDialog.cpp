#include "MediaInfoDialog.h"
#include "PrefManager.h"
#include "TimeFormatter.h"

#include <QScrollArea>
#include <QPushButton>
#include <QFormLayout>
#include <QLabel>

MediaInfoDialog::MediaInfoDialog(const Media& media)
{

    setWindowTitle(PrefManager::instance().getText("media_info_dialog_title"));
    QVBoxLayout* mainLayout = new QVBoxLayout;
    QScrollArea* scrollArea = new QScrollArea();
    QWidget* mediaInfoWidget = new QWidget();
    QVBoxLayout* mediaInfoLayout = new QVBoxLayout();

    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    mediaInfoLayout->addLayout(createSeparator(PrefManager::instance().getText("media_info_section_general")));

    mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_info_title"), media.fileName()));
    mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_info_path"), media.filePath()));
    mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_info_format"), media.fileExtension()));
    mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_info_type"), mediaTypeToString(media.type())));

    if(media.type() != Image){

        mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_info_duration"), TimeFormatter::msToHHMMSSFF(media.duration(), media.fps())));

        libvlc_track_type_t previousType = libvlc_track_unknown;
        int typeCount = 0;
        for(size_t ITrack = 0; ITrack < media.tracks().size(); ++ITrack){
            MediaTrackInfo track = media.tracks().at(ITrack);
            if(track._type == previousType) typeCount++;
            else{
                previousType = track._type;
                typeCount = 1;
            }
            QString countStr = typeCount > 1 ? " " + QString::number(typeCount) : "";
            switch(track._type){

            case libvlc_track_unknown:
                break;

            case libvlc_track_audio:

                mediaInfoLayout->addLayout(createSeparator(PrefManager::instance().getText("media_info_section_audio") + countStr));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_description"), track._description));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_language"), track._language));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_codec"), libvlc_media_get_codec_description(track._type, track._codec)));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_channels"), QString::number(track._channels)));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_rate"), QString::number(track._rate)));
                break;

            case libvlc_track_video:
                mediaInfoLayout->addLayout(createSeparator(PrefManager::instance().getText("media_info_section_video") + countStr));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_description"), track._description));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_dimensions"), QString::number(media.width()) + "x" + QString::number(media.height())));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_fps"), QString::number(media.fps())));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_codec"), libvlc_media_get_codec_description(track._type, track._codec)));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_sar"), QString::number(track._sarNum) + "x" + QString::number(track._sarDen)));
                break;

            case libvlc_track_text:
                mediaInfoLayout->addLayout(createSeparator(PrefManager::instance().getText("media_info_section_subtitle") + countStr));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_description"), track._description));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_language"), track._language));
                mediaInfoLayout->addWidget(createFrame(PrefManager::instance().getText("media_track_info_encoding"), track._encoding));
                break;
            }

        }

    }

    mediaInfoLayout->addLayout(createSeparator(PrefManager::instance().getText("media_info_section_metadata")));
    for(int IMeta = libvlc_meta_Title; IMeta < libvlc_meta_DiscTotal; ++IMeta){
        libvlc_meta_t meta = static_cast<libvlc_meta_t>(IMeta);
        mediaInfoLayout->addWidget(createFrame(media.metaToString(meta), media.metaData().value(meta)));
    }

    mediaInfoWidget->setLayout(mediaInfoLayout);
    scrollArea->setWidget(mediaInfoWidget);
    mainLayout->addWidget(scrollArea);

    QPushButton* closeBtn = new QPushButton(PrefManager::instance().getText("close"));
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    mainLayout->addWidget(closeBtn, 0, Qt::AlignRight);

    setLayout(mainLayout);
    setFixedWidth(500);

    exec();
}

QFrame* MediaInfoDialog::createFrame(const QString& key, const QString& value)
{
    QFrame* frameRes = new QFrame();
    frameRes->setStyleSheet("border: none; background-color: palette(base); padding: 1px; border-radius: 5px;");
    QFormLayout* frameLayout = new QFormLayout(frameRes);
    frameLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);

    QLabel* nameLabel = new QLabel(key);
    nameLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    nameLabel->setStyleSheet("font-weight: bold;");

    QLabel* valueLabel = new QLabel(value);
    valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    valueLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    valueLabel->setWordWrap(true);
    valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    frameLayout->setWidget(0, QFormLayout::LabelRole, nameLabel);
    frameLayout->setWidget(0, QFormLayout::FieldRole, valueLabel);

    return frameRes;
}

QHBoxLayout* MediaInfoDialog::createSeparator(const QString& separatorName)
{
    QHBoxLayout* layout = new QHBoxLayout();
    layout->addWidget(new QLabel(separatorName));
    QFrame* separationLine = new QFrame();
    separationLine->setFrameShape(QFrame::HLine);
    layout->addWidget(separationLine, 1);
    return layout;
}

QString MediaInfoDialog::mediaTypeToString(const MediaType type) const
{
    switch(type){
    case Video:
        return PrefManager::instance().getText("media_type_video");
    case Image:
        return PrefManager::instance().getText("media_type_image");
    case Audio:
        return PrefManager::instance().getText("media_type_audio");
    case Unknown:
        return PrefManager::instance().getText("media_type_unknown");
    }
}
