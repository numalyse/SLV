#include "WinSymLink.h"

#ifdef _WIN32

#include <windows.h>
#include <shobjidl.h>
#include <objbase.h>

#include <string>

QString WinSymLink::findTarget(const QString& linkPath)
{
    QString target;

    // Qt already initializes COM on the gui thread; S_FALSE just means
    // it was initialized before, which still needs a matching CoUninitialize
    HRESULT initResult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool mustUninitialize = (initResult == S_OK || initResult == S_FALSE);

    IShellLinkW* shellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
                                  IID_IShellLinkW, reinterpret_cast<void**>(&shellLink));
    if (SUCCEEDED(hr)) {
        IPersistFile* persistFile = nullptr;
        hr = shellLink->QueryInterface(IID_IPersistFile, reinterpret_cast<void**>(&persistFile));
        if (SUCCEEDED(hr)) {
            const std::wstring wLinkPath = linkPath.toStdWString();
            hr = persistFile->Load(wLinkPath.c_str(), STGM_READ);
            if (SUCCEEDED(hr)) {
                // makes windows search for the target if it moved (link tracking)
                hr = shellLink->Resolve(nullptr, SLR_NO_UI);
                if (SUCCEEDED(hr)) {
                    wchar_t resolvedPath[MAX_PATH] = {};
                    if (SUCCEEDED(shellLink->GetPath(resolvedPath, MAX_PATH, nullptr, 0))) {
                        target = QString::fromWCharArray(resolvedPath);
                    }

                    // persist the updated target so future loads don't need to search again
                    if (persistFile->IsDirty() == S_OK) {
                        persistFile->Save(wLinkPath.c_str(), TRUE);
                    }
                }
            }
            persistFile->Release();
        }
        shellLink->Release();
    }

    if (mustUninitialize) {
        CoUninitialize();
    }

    return target;
}

#endif
