#include "MacSymLink.h"

#import <Foundation/Foundation.h>

bool MacSymLink::create(const QString& targetPath, const QString& linkPath)
{
    @autoreleasepool {
        NSURL* targetUrl = [NSURL fileURLWithPath:targetPath.toNSString()];
        NSURL* linkUrl = [NSURL fileURLWithPath:linkPath.toNSString()];

        NSError* error = nil;
        NSData* bookmark = [targetUrl bookmarkDataWithOptions:NSURLBookmarkCreationSuitableForBookmarkFile
                               includingResourceValuesForKeys:nil
                                                relativeToURL:nil
                                                        error:&error];
        if (!bookmark) {
            NSLog(@"[MacSymLink] failed to create bookmark data: %@", error);
            return false;
        }

        BOOL ok = [NSURL writeBookmarkData:bookmark
                                     toURL:linkUrl
                                   options:NSURLBookmarkCreationSuitableForBookmarkFile
                                     error:&error];
        if (!ok) {
            NSLog(@"[MacSymLink] failed to write alias file: %@", error);
        }
        return ok;
    }
}

QString MacSymLink::findTarget(const QString& linkPath)
{
    @autoreleasepool {
        NSURL* linkUrl = [NSURL fileURLWithPath:linkPath.toNSString()];

        NSError* error = nil;
        NSData* bookmark = [NSURL bookmarkDataWithContentsOfURL:linkUrl error:&error];
        if (!bookmark) {
            NSLog(@"[MacSymLink] failed to read alias file: %@", error);
            return QString();
        }

        BOOL isStale = NO;
        NSURL* targetUrl = [NSURL URLByResolvingBookmarkData:bookmark
                                                     options:NSURLBookmarkResolutionWithoutUI
                                                             | NSURLBookmarkResolutionWithoutMounting
                                               relativeToURL:nil
                                         bookmarkDataIsStale:&isStale
                                                       error:&error];
        if (!targetUrl) {
            NSLog(@"[MacSymLink] failed to resolve alias: %@", error);
            return QString();
        }

        QString targetPath = QString::fromNSString(targetUrl.path);

        // The target moved since the alias was written: rewrite the alias
        // so it points directly at the new location for future loads.
        if (isStale) {
            create(targetPath, linkPath);
        }

        return targetPath;
    }
}
