#ifndef ICONHELPER_H
#define ICONHELPER_H

#include "PrefManager.h"

#include <QIcon>
#include <QColor>
#include <QPixmap>
#include <QPainter>
#include <QPen>
#include <QMenu>
#include <QVector>
#include <QPair>

#include <functional>

namespace IconHelper
{

    /// @brief Creates a round icon to preview color
    /// @param color color of icon, if color is invalid => transparent icon
    /// @param sizePen diameter
    /// @param outlineActivated black outline
    /// @return
    inline QIcon genIconPreviewColor(QColor color, int sizePen, bool outlineActivated = false)
    {
        if (!color.isValid())
            color = QColor{0, 0, 0, 0};

        QPixmap pixmap(30, 30);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(color);

        if (outlineActivated) {
            sizePen = sizePen-2;
            QPen pen(Qt::black);
            pen.setWidth(2);
            painter.setPen(pen);
        } else {
            painter.setPen(Qt::NoPen);
        }

        int x = (pixmap.width() - sizePen) / 2;
        int y = (pixmap.height() - sizePen) / 2;
        painter.drawEllipse(x, y, sizePen, sizePen);

        return QIcon(pixmap);
    }

    inline QIcon genIconPreviewColor(QColor color, bool outlineActivated = false)
    {
        return genIconPreviewColor(color, 30, outlineActivated);
    }

    /// @brief Named colors shared by the color pickers
    inline const QVector<QPair<QString, QColor>>& colorPalette()
    {
        static const QVector<QPair<QString, QColor>> palette = {
            {PrefManager::instance().getText("white"), QColor{255, 255, 255, 255}}, // Blanc
            {PrefManager::instance().getText("black"), QColor{0, 0, 0, 255}},       // Noir
            {PrefManager::instance().getText("red"), QColor{229, 0, 0, 255}},       // Rouge
            {PrefManager::instance().getText("orange"), QColor{255, 141, 0, 255}},  // Orange
            {PrefManager::instance().getText("yellow"), QColor{255, 238, 0, 255}},  // Jaune
            {PrefManager::instance().getText("green"), QColor{0, 129, 33, 255}},    // Vert
            {PrefManager::instance().getText("blue"), QColor{0, 76, 255, 255}},     // Bleu
            {PrefManager::instance().getText("purple"), QColor{118, 1, 136, 255}},  // Violet
        };
        return palette;
    }

    /// @brief Opens a color picker menu below anchor
    /// @param parent parent of the menu
    /// @param anchor widget under which the menu is shown
    /// @param onColorSelected called when the color is selected
    inline void execColorPickerMenu(QWidget* parent, QWidget* anchor,
                                    const std::function<void(const QColor&)>& onColorSelected)
    {
        QMenu menu(parent);

        auto addColorAction = [&menu, &onColorSelected](const QString& colorName, const QColor& color){
            bool outlineActivated = color == QColor{255, 255, 255, 255};
            QAction* action = menu.addAction(genIconPreviewColor(color, outlineActivated), colorName);
            QObject::connect(action, &QAction::triggered, &menu, [&onColorSelected, color]() {
                onColorSelected(color);
            });
        };

        for (const auto& [colorName, color] : colorPalette())
            addColorAction(colorName, color);

        menu.exec(anchor->mapToGlobal(QPoint(0, anchor->height())));
    }

} // namespace IconHelper

#endif // ICONHELPER_H
