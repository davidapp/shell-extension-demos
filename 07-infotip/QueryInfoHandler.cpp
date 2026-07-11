#include "QueryInfoHandler.h"
#include "Guid.h"
#include "common/ShellExtBase.h"
#include <shlwapi.h>   // SHStrDupW
#include <strsafe.h>

QueryInfoHandler::QueryInfoHandler() : m_cRef(1) { m_szFile[0] = L'\0'; DllAddRef(); }
QueryInfoHandler::~QueryInfoHandler() { DllRelease(); }

IFACEMETHODIMP QueryInfoHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IPersist || riid == IID_IPersistFile)
        *ppv = static_cast<IPersistFile*>(this);
    else if (riid == IID_IQueryInfo)
        *ppv = static_cast<IQueryInfo*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) QueryInfoHandler::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) QueryInfoHandler::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

IFACEMETHODIMP QueryInfoHandler::GetClassID(CLSID* p) { *p = CLSID_DemoInfotipHandler; return S_OK; }
IFACEMETHODIMP QueryInfoHandler::IsDirty() { return S_FALSE; }
IFACEMETHODIMP QueryInfoHandler::Load(LPCOLESTR pszFileName, DWORD)
{
    return StringCchCopyW(m_szFile, ARRAYSIZE(m_szFile), pszFileName);
}
IFACEMETHODIMP QueryInfoHandler::Save(LPCOLESTR, BOOL) { return E_NOTIMPL; }
IFACEMETHODIMP QueryInfoHandler::SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
IFACEMETHODIMP QueryInfoHandler::GetCurFile(LPOLESTR*) { return E_NOTIMPL; }

// 返回悬停提示文字（用 CoTaskMemAlloc 分配，Shell 释放）。
IFACEMETHODIMP QueryInfoHandler::GetInfoTip(DWORD, PWSTR* ppwszTip)
{
    if (!ppwszTip) return E_POINTER;

    ULONGLONG size = 0;
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (GetFileAttributesExW(m_szFile, GetFileExInfoStandard, &fad))
        size = (static_cast<ULONGLONG>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;

    wchar_t tip[MAX_PATH + 128];
    StringCchPrintfW(tip, ARRAYSIZE(tip),
        L"Shell 扩展示例\n路径：%s\n大小：%llu 字节", m_szFile, size);
    return SHStrDupW(tip, ppwszTip);
}

IFACEMETHODIMP QueryInfoHandler::GetInfoFlags(DWORD*) { return E_NOTIMPL; }
