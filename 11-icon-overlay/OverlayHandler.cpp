#include "OverlayHandler.h"
#include "Guid.h"
#include "common/ShellExtBase.h"
#include <shlwapi.h>    // StrStrIW
#include <strsafe.h>

OverlayHandler::OverlayHandler() : m_cRef(1) { DllAddRef(); }
OverlayHandler::~OverlayHandler() { DllRelease(); }

IFACEMETHODIMP OverlayHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IShellIconOverlayIdentifier)
        *ppv = static_cast<IShellIconOverlayIdentifier*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) OverlayHandler::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) OverlayHandler::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

// 告诉资源管理器：叠加图标从哪个文件的第几个图标取。
// 演示用系统 shell32.dll 里的图标占位；真实扩展应指向自带的 .ico。
IFACEMETHODIMP OverlayHandler::GetOverlayInfo(LPWSTR pwszIconFile, int cchMax, int* pIndex, DWORD* pdwFlags)
{
    UINT n = GetSystemDirectoryW(pwszIconFile, cchMax);
    if (n == 0 || (int)n >= cchMax) return E_FAIL;
    HRESULT hr = StringCchCatW(pwszIconFile, cchMax, L"\\shell32.dll");
    if (FAILED(hr)) return hr;
    *pIndex = 0;                                    // 占位图标索引
    *pdwFlags = ISIOI_ICONFILE | ISIOI_ICONINDEX;   // 上面两项都有效
    return S_OK;
}

// 优先级 0（最高）~ 100（最低）。多个叠加命中同一文件时按此排序。
IFACEMETHODIMP OverlayHandler::GetPriority(int* pPriority)
{
    if (!pPriority) return E_POINTER;
    *pPriority = 0;
    return S_OK;
}

// 核心判定：这个文件要不要叠标记？S_OK = 叠，S_FALSE = 不叠。
// 演示规则：文件名里含 “demo”（不分大小写）就叠。
IFACEMETHODIMP OverlayHandler::IsMemberOf(LPCWSTR pwszPath, DWORD /*dwAttrib*/)
{
    if (!pwszPath) return E_INVALIDARG;
    return (StrStrIW(pwszPath, L"demo") != nullptr) ? S_OK : S_FALSE;
}
