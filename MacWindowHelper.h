#pragma once

#ifdef __APPLE__
class QWidget;

/// @brief Helpers pour attacher une fenêtre Qt (Qt::Tool, etc.) comme child window native (NSWindow)
/// de la fenêtre principale. Un child window est déplacé par le window server en même temps que
/// son parent pendant un drag, ce qui évite le lag du repositionnement manuel via moveEvent.
namespace MacWindowHelper {
    /// @brief Attache la NSWindow de popup comme child window de la fenêtre top-level de parent.
    /// À appeler après show() (la fenêtre native doit exister).
    void attachAsChildWindow(QWidget* popup, QWidget* parent);

    /// @brief Détache la NSWindow de popup de son parent. À appeler avant hide()/destruction.
    void detachFromParentWindow(QWidget* popup);
}
#endif
