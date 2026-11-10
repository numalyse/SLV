#ifndef SHOTITEM_H
#define SHOTITEM_H

#include "Shot.h"
#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QPointer>
#include <functional>

class QGraphicsSceneHoverEvent;
class QWidget;

class ShotItem : public QGraphicsItem
{
public:
    explicit ShotItem(Shot shot, double width, QGraphicsItem* parent = nullptr);
    ~ShotItem();

    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

    QColor getTagImageColor(const QPixmap &pixmap) const;
    void setThumbnail(const QPixmap &pixmap);
    void setTagImage(const QPixmap& pixmap);
    void clearTagImage();
    void setTagImageRequestHandler(std::function<void()> handler);
    void setHoverTooltipText(const QString& text);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setWidth(double width);
    double width(){return m_widthMs;}
    Shot& shot(){return m_shot;};

    bool isSelected() {return m_selected;}
    void setSelected(bool state);

    void updateTextPosition();

    void setSelectedNumber(int number) { 
        m_selectedNumber = number;
        m_selectionText->setPlainText(QString::number(m_selectedNumber));
        updateTextPosition();
        update();
    }

    void setShowThumbnail(bool show);
    bool showThumbnail() const { return m_showThumbnail; }
    void setShowBorder(bool show);
    bool showBorder() const { return m_showBorder; }

    int type() const override { return SLV::TypeShotItem; }

protected:
    // so child class (audioShot) can override getters and have its own s_height / s_topMargin 
    virtual double height() const { return s_height; }
    virtual double topMargin() const { return s_topMargin; }

private:
    void requestTagImageIfNeeded();
    void updateToolTip();

    Shot m_shot;
    QPixmap m_pixmap;
    QPixmap m_tagImage;
    QPointer<QWidget> m_toolTipWidget;
    std::function<void()> m_tagImageRequestHandler;
    QString m_hoverTooltipTextValue;
    QGraphicsRectItem* m_selectionBox = nullptr;
    QGraphicsTextItem* m_selectionText = nullptr;
    double m_widthMs{};
    int m_selectedNumber = 1;
    bool m_selected = false;
    bool m_isHovered = false;
    bool m_tagImageRequested = false;
    bool m_showThumbnail = true;
    bool m_showBorder = true;
    bool m_selectionNeedUpdate = false;

    static constexpr double s_height = 40.0;
    static constexpr double s_topMargin = 50.0;
    static constexpr int s_minSizeForImage = 100;

    QColor m_baseColor;
};




#endif // SHOTITEM_H
