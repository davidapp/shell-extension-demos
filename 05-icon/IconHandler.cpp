#include "IconHandler.h"
#include "Guid.h"
#include "common/ShellExtBase.h"
#include <strsafe.h>

IconHandler::IconHandler() : m_cRef(1) { m_szFile[0] = L'\0'; DllAddRef(); }
IconHandler::~IconHandler() { DllRelease(); }

// ---------------- IUnknown ----------------
IFACEMETHODIMP IconHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IPersist || riid == IID_IPersistFile)
        *ppv = static_cast<IPersistFile*>(this);
    else if (riid == IID_IExtractIconW)
        *ppv = static_cast<IExtractIconW*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) IconHandler::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) IconHandler::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

// ---------------- IPersistFile ----------------
IFACEMETHODIMP IconHandler::GetClassID(CLSID* pClassID) { *pClassID = CLSID_DemoIconHandler; return S_OK; }
IFACEMETHODIMP IconHandler::IsDirty() { return S_FALSE; }
IFACEMETHODIMP IconHandler::Load(LPCOLESTR pszFileName, DWORD)
{
    return StringCchCopyW(m_szFile, ARRAYSIZE(m_szFile), pszFileName);
}
IFACEMETHODIMP IconHandler::Save(LPCOLESTR, BOOL) { return E_NOTIMPL; }
IFACEMETHODIMP IconHandler::SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
IFACEMETHODIMP IconHandler::GetCurFile(LPOLESTR*) { return E_NOTIMPL; }

// ---------------- IExtractIconW ----------------
// 返回“图标在哪个文件的第几个”。这里按文件大小从 shell32.dll 里挑不同图标。
IFACEMETHODIMP IconHandler::GetIconLocation(UINT, PWSTR pszIconFile, UINT cchMax,
                                            int* piIndex, UINT* pwFlags)
{
    ULONGLONG size = 0;
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (GetFileAttributesExW(m_szFile, GetFileExInfoStandard, &fad))
        size = (static_cast<ULONGLONG>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;

    wchar_t sysdir[MAX_PATH];
    GetSystemDirectoryW(sysdir, ARRAYSIZE(sysdir));
    StringCchPrintfW(pszIconFile, cchMax, L"%s\\shell32.dll", sysdir);

    // 按大小分三档，用 shell32.dll 里三个不同图标索引（仅演示，图标本身无所谓）。
    if (size < 1024)              *piIndex = 71;   // 空/很小
    else if (size < 1024 * 1024)  *piIndex = 70;   // 中等
    else                          *piIndex = 47;   // 较大

    // GIL_PERINSTANCE：告诉 Shell 图标是“逐文件”的，不能按类型缓存成一个。
    *pwFlags = GIL_PERINSTANCE;
    return S_OK;
}

// 返回 S_FALSE，让 Shell 依据 GetIconLocation 给的“文件+索引”自行加载图标。
IFACEMETHODIMP IconHandler::Extract(PCWSTR, UINT, HICON* phiconLarge, HICON* phiconSmall, UINT)
{
    if (phiconLarge) *phiconLarge = nullptr;
    if (phiconSmall) *phiconSmall = nullptr;
    return S_FALSE;
}
