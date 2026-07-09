#include "MacWindowHelper.h"

#include <QWidget>

#import <AppKit/AppKit.h>

void MacWindowHelper::attachAsChildWindow(QWidget* popup, QWidget* parent)
{
    if (!popup || !parent) return;

    NSView* popupView  = reinterpret_cast<NSView*>(popup->winId());
    NSView* parentView = reinterpret_cast<NSView*>(parent->window()->winId());

    NSWindow* popupWin  = [popupView window];
    NSWindow* parentWin = [parentView window];

    if (popupWin && parentWin && popupWin != parentWin && [popupWin parentWindow] != parentWin)
        [parentWin addChildWindow:popupWin ordered:NSWindowAbove];
}

void MacWindowHelper::detachFromParentWindow(QWidget* popup)
{
    if (!popup || !popup->testAttribute(Qt::WA_WState_Created)) return;

    NSWindow* popupWin = [reinterpret_cast<NSView*>(popup->winId()) window];

    if (popupWin && [popupWin parentWindow])
        [[popupWin parentWindow] removeChildWindow:popupWin];
}
