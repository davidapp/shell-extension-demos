#pragma once
#include <windows.h>
#include <shlobj.h>   // IExtractIconW, GIL_*

// 逐文件图标处理器：根据文件大小让同类文件显示不同图标。
//   IPersistFile   —— Shell 通过 Load 把文件路径给我们（图标处理器的经典初始化方式）；
//   IExtractIconW  —— GetIconLocation 返回“图标在哪”，Extract 可选地亲自产出 HICON。
class IconHandler : public IPersistFile, public IExtractIconW
{
public:
    IconHandler();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IPersist / IPersistFile
    IFACEMETHODIMP GetClassID(CLSID* pClassID) override;
    IFACEMETHODIMP IsDirty() override;
    IFACEMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode) override;
    IFACEMETHODIMP Save(LPCOLESTR, BOOL) override;
    IFACEMETHODIMP SaveCompleted(LPCOLESTR) override;
    IFACEMETHODIMP GetCurFile(LPOLESTR*) override;

    // IExtractIconW
    IFACEMETHODIMP GetIconLocation(UINT uFlags, PWSTR pszIconFile, UINT cchMax,
                                   int* piIndex, UINT* pwFlags) override;
    IFACEMETHODIMP Extract(PCWSTR pszFile, UINT nIconIndex,
                           HICON* phiconLarge, HICON* phiconSmall, UINT nIconSize) override;

private:
    ~IconHandler();
    long m_cRef;
    wchar_t m_szFile[MAX_PATH];
};
