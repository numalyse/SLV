#ifndef SHOTITEM_H
#define SHOTITEM_H

#include "Shot.h"
#include "Timeline/ItemTypes.h"

#include <QGraphicsItem>
#include <QPixmap>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>

class ShotItem : public QGraphicsItem
{
public:
    explicit ShotItem(Shot shot, double width, double height = 40, double topMargin = 30, QGraphicsItem* parent = nullptr);
    ~ShotItem();

    void setThumbnail(const QPixmap &pixmap);

    QRectF boundingRect() const override;
    void paint(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    void setWidth(double width);
    double width(){return m_width;}
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

    int type() const override { return SLV::TypeShotItem; }

private:
    Shot m_shot;
    QPixmap m_pixmap;
    QGraphicsRectItem* m_selectionBox = nullptr;
    QGraphicsTextItem* m_selectionText = nullptr;
    double m_width{};
    double m_height{};
    double m_topMargin{};
    int m_selectedNumber = 1;
    bool m_selected = false;
    bool m_selectionNeedUpdate = false;

    constexpr static int s_minSizeForImage{100};
};




#endif // SHOTITEM_H
