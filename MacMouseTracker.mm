#include "MacMouseTracker.h"
#import <AppKit/AppKit.h>

static id s_monitor = nil;

void MacMouseTracker::install(std::function<void()> onMouseMove) {
    s_monitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskMouseMoved
                                                      handler:^NSEvent*(NSEvent* event) {
        onMouseMove();
        return event;
    }];
}

void MacMouseTracker::uninstall() {
    if (s_monitor) {
        [NSEvent removeMonitor:s_monitor];
        s_monitor = nil;
    }
}
