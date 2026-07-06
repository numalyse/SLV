#ifndef SNAPSHOTPOPUP_H
#define SNAPSHOTPOPUP_H

#include <QFrame>
#include <QTimer>
#include <QPropertyAnimation>

/// @brief Notification popup that appears when a snapshot is taken, showing the file path and timestamp of the snapshot.
class SnapshotPopup : public QFrame
{
    Q_OBJECT

public:
    /// @param anchor widget to anchor the popup to (top-right corner)
    /// @param filePath filepath of the snapshot image
    /// @param vlcTime vlc time of the snapshot (in milliseconds)
    /// @param fps fps of the media, used to format the timestamp
    explicit SnapshotPopup(QWidget* anchor, const QString& filePath, int64_t vlcTime, double fps);
    ~SnapshotPopup();

    /// @brief Displays the popup with a fade-in animation, and starts the auto-close timer
    void showWithFade();

    /// @brief Repositions the popup relative to the anchor widget, and keeps it within the screen bounds
    void reposition();

private:
    void buildUi(const QString& filePath, int64_t vlcTime, double fps);

    /// @brief Fades out the popup then destroys it, called when the auto-close timer times out
    void fadeOutAndClose();

    QWidget* m_anchor = nullptr;
    QTimer* m_closeTimer = nullptr;
    QPropertyAnimation* m_anim = nullptr;

    static constexpr int m_durationMs = 3000;
    static constexpr int m_fadeMs = 150;

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
};

#endif // SNAPSHOTPOPUP_H
