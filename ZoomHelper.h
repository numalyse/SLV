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

    /// @brief 16 bits pour chaque coord du zoom dans un entier 64 bits (pas de vidéo avec un résolution > 2^16 donc safe)
    QString getZoomStr(){
        const quint64 key = (quint64(quint16(x1)) << 48)
                           | (quint64(quint16(y1)) << 32)
                           | (quint64(quint16(x2)) << 16)
                           | quint64(quint16(y2));
        return QString("_z_%1").arg(key, 0, 36);
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

        double factor = (n < 0.0) ? 0.9 : (1.0 / 0.9);

        double centerX = (fx1 + fx2) / 2.0;
        double centerY = (fy1 + fy2) / 2.0;

        if(centerPos != QPointF(-1, -1)){
            centerX = centerPos.x() * (fx1 - fx2) + fx2;
            centerY = centerPos.y() * (fy1 - fy2) + fy2;
        }

        double newWidth  = (fx1 - fx2) * factor;
        double newHeight = (fy1 - fy2) * factor;

        // Bloquer seulement le zoom IN
        if (factor < 1.0 && (newWidth < 10.0 || newHeight < 10.0)) {
            return getZoomArg();
        }

        // Ramener les dimensions dans les bornes vidéo en conservant le ratio
        if (newWidth > maxWidth || newHeight > maxHeight) {
            const double ar = double(maxWidth) / double(maxHeight);
            if (newWidth / maxWidth >= newHeight / maxHeight) {
                newWidth  = maxWidth;
                newHeight = maxWidth / ar;
            } else {
                newHeight = maxHeight;
                newWidth  = maxHeight * ar;
            }
        }

        // Position relative du centre dans la région courante (0‥1)
        const double relX = (centerX - fx2) / (fx1 - fx2);
        const double relY = (centerY - fy2) / (fy1 - fy2);

        // Appliquer la nouvelle taille centrée sur le même point
        double newFx2 = centerX - relX * newWidth;
        double newFx1 = newFx2 + newWidth;
        double newFy2 = centerY - relY * newHeight;
        double newFy1 = newFy2 + newHeight;

        // Glisser la fenêtre dans les bornes sans la découper (préserve le ratio)
        if (newFx1 > maxWidth) { 
            newFx1 = maxWidth;  
            newFx2 = maxWidth  - newWidth;  
        }
        if (newFx2 < 0) { 
            newFx2 = 0;          
            newFx1 = newWidth;             
        }
        if (newFy1 > maxHeight) {
            newFy1 = maxHeight; 
            newFy2 = maxHeight - newHeight; 
        }
        if (newFy2 < 0) { 
            newFy2 = 0;          
            newFy1 = newHeight;            
        }

        fx2 = newFx2; fx1 = newFx1;
        fy2 = newFy2; fy1 = newFy1;

        x2 = std::lround(fx2); x1 = std::lround(fx1);
        y2 = std::lround(fy2); y1 = std::lround(fy1);

        return getZoomArg();
    }

    double getZoomPercent() const {
        double initialWidth = maxWidth;
        double currentWidth = fx1 - fx2;

        if (currentWidth <= 0.0)
            return 100.0;

        return (initialWidth / currentWidth) * 100.0;
    }

};

#endif // ZOOMHELPER_y1