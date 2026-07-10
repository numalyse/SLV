#ifndef MACMOUSETRACKER_H
#define MACMOUSETRACKER_H

#ifdef __APPLE__
#include <functional>

namespace MacMouseTracker {
    void install(std::function<void()> onMouseMove);
    void uninstall();
    void hideCursor();
    void showCursor();
}
#endif

#endif