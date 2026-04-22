#ifndef ZOOMHELPER_y1
#define ZOOMHELPER_y1

#include <QString>
#include <QDebug>
#include <QPoint>

class ZoomHelper{

private:
    int maxWidth;
    int maxHeight;
    int x1, y1, x2, y2;
    double fx1, fy1, fx2, fy2;
    double scale;

public:
    explicit inline ZoomHelper(const int width = 0, const int height = 0){
        maxWidth = width;
        maxHeight = height;
        x1 = width;
        y1 = height;
        x2 = 0;
        y2 = 0;
        fx1 = x1;
        fy1 = y1;
        fx2 = x2;
        fy2 = y2;
        scale = 1.0;
    }

    QString getZoomArg(){
        return QString("%1x%2+%3+%4").arg(x1).arg(y1).arg(x2).arg(y2);
    }

    void clampValues(){
        x1 = std::clamp(x1, 0, maxWidth);
        y1 = std::clamp(y1, 0, maxHeight);
        x2 = std::clamp(x2, 0, x1);
        y2 = std::clamp(y2, 0, y1);
        fx1 = std::clamp(fx1, 0.0, double(maxWidth));
        fy1 = std::clamp(fy1, 0.0, double(maxHeight));
        fx2 = std::clamp(fx2, 0.0, double(fx1));
        fy2 = std::clamp(fy2, 0.0, double(fy1));
    }

    QString moveRight(const int n){
        x1 += n;
        x2 += n;
        clampValues();
        return getZoomArg();
    }

    QString moveLeft(const int n){
        x1 -= n;
        x2 -= n;
        clampValues();
        return getZoomArg();
    }

    QString moveUp(const int n){
        y1 -= n;
        y2 -= n;
        clampValues();
        return getZoomArg();
    }

    QString moveDown(const int n){
        y1 += n;
        y2 += n;
        clampValues();
        return getZoomArg();
    }

    void move(const QPoint& deltaPos)
    {

        int newX1Pos = x1 - deltaPos.x();
        int newX2Pos = x2 - deltaPos.x();
        int newY1Pos = y1 - deltaPos.y();
        int newY2Pos = y2 - deltaPos.y();

        newX1Pos = std::clamp(newX1Pos, 0, maxWidth);
        newX2Pos = std::clamp(newX2Pos, 0, maxWidth);
        newY1Pos = std::clamp(newY1Pos, 0, maxHeight);
        newY2Pos = std::clamp(newY2Pos, 0, maxHeight);

        if(std::abs(newX1Pos - newX2Pos) == (std::abs(x1 - x2))){
            x1 = newX1Pos;
            x2 = newX2Pos;
            fx1 = x1;
            fx2 = x2;
        }
        if(std::abs(newY1Pos - newY2Pos) == (std::abs(y1 - y2))){
            y1 = newY1Pos;
            y2 = newY2Pos;
            fy1 = y1;
            fy2 = y2;
        }
    }

    QString zoom(const double n, const QPointF& centerPos = QPointF(-1, -1)){

        double factor;
        if(n < 0.0)
            factor = 0.9;
        else
            factor = 1.0 / 0.9;

        double centerX = (fx1 + fx2) / 2.0;
        double centerY = (fy1 + fy2) / 2.0;

        if(centerPos != QPointF(-1, -1)){
            centerX = centerPos.x() * std::abs(x1 - x2) + x2;
            centerY = centerPos.y() * std::abs(y1 - y2) + y2;
        }

        double tempX1 = centerX + (fx1 - centerX) * factor;
        double tempY1 = centerY + (fy1 - centerY) * factor;
        double tempX2 = centerX + (fx2 - centerX) * factor;
        double tempY2 = centerY + (fy2 - centerY) * factor;

        double width = fx1 - fx2;
        double height = fy1 - fy2;

        // Bloquer seulement le zoom IN
        if (factor < 1.0 && (width < 10.0 || height < 10.0)) {
            return getZoomArg();
        }

        fx1 = tempX1;
        fy1 = tempY1;
        fx2 = tempX2;
        fy2 = tempY2;

        x1 = std::lround(fx1);
        y1 = std::lround(fy1);
        x2 = std::lround(fx2);
        y2 = std::lround(fy2);

        clampValues();

        return getZoomArg();
    }

};

#endif // ZOOMHELPER_y1
