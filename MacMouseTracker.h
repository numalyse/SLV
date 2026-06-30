#pragma once

#ifdef __APPLE__
#include <functional>

namespace MacMouseTracker {
    void install(std::function<void()> onMouseMove);
    void uninstall();
}
#endif
