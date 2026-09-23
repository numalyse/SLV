#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QPixmap>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QResizeEvent>

class ImageLabel : public QLabel
{
public:
    explicit ImageLabel(const QPixmap &pixmap, QWidget *parent = nullptr)
        : QLabel(parent),
          m_pixmap(pixmap)
    {
        setAlignment(Qt::AlignCenter);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        setMouseTracking(true);
        setCursor(Qt::PointingHandCursor);
    }

    void setWidthRatio(double ratio)
    {
        m_widthRatio = qBound(0.0, ratio, 1.0);
        updateGeometry();
        updatePixmap();
    }

    QSize sizeHint() const override
    {
        if (m_pixmap.isNull())
            return QSize(0, 0);

        int width = 100;

        if (m_widthRatio > 0.0 && parentWidget())
            width = parentWidget()->width() * m_widthRatio;

        width = qMax(width, 1);
        width = qMin(width, m_pixmap.width());

        const int height =
            width * m_pixmap.height() / m_pixmap.width();

        return QSize(width, height);
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QLabel::resizeEvent(event);
        updatePixmap();
    }

    void enterEvent(QEnterEvent *event) override
    {
        m_mouseInside = true;
        update();

        QLabel::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override
    {
        m_mouseInside = false;
        m_loupeActive = false;
        m_zoom = 2.5;

        setCursor(Qt::PointingHandCursor);

        update();

        QLabel::leaveEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        m_mousePosition = event->position().toPoint();
        update();

        QLabel::mouseMoveEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton) {

            m_loupeActive = !m_loupeActive;

            m_mousePosition = event->position().toPoint();

            setCursor(
                m_loupeActive
                    ? Qt::CrossCursor
                    : Qt::PointingHandCursor
            );

            update();

            event->accept();
            return;
        }

        QLabel::mousePressEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override
    {
        if (!m_loupeActive) {
            QLabel::wheelEvent(event);
            return;
        }

        if (event->angleDelta().y() > 0) {
            m_zoom += 0.25;
        } else if (event->angleDelta().y() < 0) {
            m_zoom -= 0.25;
        }

        m_zoom = qBound(2.5, m_zoom, 8.0);

        update();

        event->accept();
    }

    void paintEvent(QPaintEvent *event) override
    {
        QLabel::paintEvent(event);

        if (!m_loupeActive ||
            !m_mouseInside ||
            m_pixmap.isNull())
            return;

        QPainter painter(this);

        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        const int radius = 100;
        const int diameter = radius * 2;

        QPoint center = m_mousePosition + QPoint(70, 70);

        center.setX(
            qBound(radius, center.x(), width() - radius)
        );

        center.setY(
            qBound(radius, center.y(), height() - radius)
        );

        QRect lensRect(
            center.x() - radius,
            center.y() - radius,
            diameter,
            diameter
        );

        QPainterPath clipPath;
        clipPath.addEllipse(lensRect);

        painter.save();
        painter.setClipPath(clipPath);

        QPixmap displayedPixmap = pixmap();

        if (displayedPixmap.isNull()) {
            painter.restore();
            return;
        }

        QRect imageRect = displayedPixmap.rect();
        imageRect.moveCenter(rect().center());

        double xRatio =
            static_cast<double>(
                m_mousePosition.x() - imageRect.left()
            ) / imageRect.width();

        double yRatio =
            static_cast<double>(
                m_mousePosition.y() - imageRect.top()
            ) / imageRect.height();

        double sourceX = xRatio * m_pixmap.width();
        double sourceY = yRatio * m_pixmap.height();

        double sourceWidth =
            diameter / m_zoom *
            m_pixmap.width() /
            displayedPixmap.width();

        double sourceHeight =
            diameter / m_zoom *
            m_pixmap.height() /
            displayedPixmap.height();

        QRectF sourceRect(
            sourceX - sourceWidth / 2,
            sourceY - sourceHeight / 2,
            sourceWidth,
            sourceHeight
        );

        sourceRect.moveLeft(
            qBound(
                0.0,
                sourceRect.left(),
                static_cast<double>(m_pixmap.width())
                    - sourceRect.width()
            )
        );

        sourceRect.moveTop(
            qBound(
                0.0,
                sourceRect.top(),
                static_cast<double>(m_pixmap.height())
                    - sourceRect.height()
            )
        );

        painter.drawPixmap(
            lensRect,
            m_pixmap,
            sourceRect
        );

        painter.restore();

        QPen pen(Qt::white);
        pen.setWidth(3);

        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);

        painter.drawEllipse(lensRect);

        QPen shadowPen(QColor(0, 0, 0, 120));
        shadowPen.setWidth(7);

        painter.setPen(shadowPen);
        painter.drawEllipse(lensRect);
    }

private:
    void updatePixmap()
    {
        if (m_pixmap.isNull())
            return;

        int width = contentsRect().width();

        if (width <= 0)
            return;

        if (m_widthRatio > 0.0 && parentWidget()) {
            width = qMin(
                width,
                static_cast<int>(
                    parentWidget()->contentsRect().width()
                    * m_widthRatio
                )
            );
        }

        width = qMin(width, m_pixmap.width());

        const int height =
            width * m_pixmap.height() /
            m_pixmap.width();

        setPixmap(
            m_pixmap.scaled(
                width,
                height,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
            )
        );
    }

private:
    QPixmap m_pixmap;

    double m_widthRatio = 0.0;
    double m_zoom = 2.5;

    bool m_mouseInside = false;
    bool m_loupeActive = false;

    QPoint m_mousePosition;
};

#endif // IMAGELABEL_H