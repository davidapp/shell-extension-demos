#pragma once
#include <windows.h>
#include <shlobj.h>   // IShellIconOverlayIdentifier

// 图标叠加处理器：给符合条件的文件在图标左下角叠一个小标记
//（OneDrive / Dropbox / TortoiseGit 的同步状态角标就是这一类）。
class OverlayHandler : public IShellIconOverlayIdentifier
{
public:
    OverlayHandler();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IShellIconOverlayIdentifier
    IFACEMETHODIMP GetOverlayInfo(LPWSTR pwszIconFile, int cchMax, int* pIndex, DWORD* pdwFlags) override;
    IFACEMETHODIMP GetPriority(int* pPriority) override;
    IFACEMETHODIMP IsMemberOf(LPCWSTR pwszPath, DWORD dwAttrib) override;

private:
    ~OverlayHandler();
    long m_cRef;
};
