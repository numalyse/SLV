#ifndef MACSYMLINK_H
#define MACSYMLINK_H

#ifdef __APPLE__
#include <QString>

namespace MacSymLink {
    // Creates a Finder alias file at linkPath pointing to targetPath.
    // Unlike a POSIX symlink, the alias keeps working if the target
    // is moved or renamed on the same volume (macOS tracks it by file id).
    bool create(const QString& targetPath, const QString& linkPath);

    // Resolves the alias file and returns the target's absolute path,
    // or an empty string if the alias can't be read or the target is gone.
    QString findTarget(const QString& linkPath);
}
#endif

#endif
