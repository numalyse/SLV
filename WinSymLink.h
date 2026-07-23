#ifndef WINSYMLINK_H
#define WINSYMLINK_H

#ifdef _WIN32
#include <QString>

namespace WinSymLink {
    // Resolves a .lnk shortcut and returns the target's absolute path,
    // or an empty string on failure. Unlike QFile::symLinkTarget which only
    // reads the path stored in the .lnk, this calls IShellLink::Resolve so
    // windows searches for the target if it was moved or renamed
    // (same mechanism the explorer uses on double-click).
    QString findTarget(const QString& linkPath);
}
#endif

#endif
