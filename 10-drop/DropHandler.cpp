#include "DropHandler.h"
#include "Guid.h"
#include "common/ShellExtBase.h"
#include <shellapi.h>   // DragQueryFileW
#include <strsafe.h>
#include <string>

DropHandler::DropHandler() : m_cRef(1) { m_szTarget[0] = L'\0'; DllAddRef(); }
DropHandler::~DropHandler() { DllRelease(); }

IFACEMETHODIMP DropHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IPersist || riid == IID_IPersistFile)
        *ppv = static_cast<IPersistFile*>(this);
    else if (riid == IID_IDropTarget)
        *ppv = static_cast<IDropTarget*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) DropHandler::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) DropHandler::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

IFACEMETHODIMP DropHandler::GetClassID(CLSID* p) { *p = CLSID_DemoDropHandler; return S_OK; }
IFACEMETHODIMP DropHandler::IsDirty() { return S_FALSE; }
IFACEMETHODIMP DropHandler::Load(LPCOLESTR pszFileName, DWORD)
{
    return StringCchCopyW(m_szTarget, ARRAYSIZE(m_szTarget), pszFileName);
}
IFACEMETHODIMP DropHandler::Save(LPCOLESTR, BOOL) { return E_NOTIMPL; }
IFACEMETHODIMP DropHandler::SaveCompleted(LPCOLESTR) { return E_NOTIMPL; }
IFACEMETHODIMP DropHandler::GetCurFile(LPOLESTR*) { return E_NOTIMPL; }

// 拖入时表示“可以放”（这里统一用复制效果）。
IFACEMETHODIMP DropHandler::DragEnter(IDataObject*, DWORD, POINTL, DWORD* pdwEffect)
{
    if (pdwEffect) *pdwEffect = DROPEFFECT_COPY;
    return S_OK;
}
IFACEMETHODIMP DropHandler::DragOver(DWORD, POINTL, DWORD* pdwEffect)
{
    if (pdwEffect) *pdwEffect = DROPEFFECT_COPY;
    return S_OK;
}
IFACEMETHODIMP DropHandler::DragLeave() { return S_OK; }

// 松手：从数据对象里取出被拖入的文件列表，弹框展示（真实扩展在此处理它们）。
IFACEMETHODIMP DropHandler::Drop(IDataObject* pDataObj, DWORD, POINTL, DWORD* pdwEffect)
{
    if (pdwEffect) *pdwEffect = DROPEFFECT_COPY;
    if (!pDataObj) return E_INVALIDARG;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg;
    if (SUCCEEDED(pDataObj->GetData(&fe, &stg)))
    {
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stg.hGlobal));
        if (hDrop)
        {
            std::wstring msg = L"拖放目标：\n";
            msg += m_szTarget;
            msg += L"\n\n拖入的文件：\n";
            UINT n = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
            for (UINT i = 0; i < n; ++i)
            {
                wchar_t f[MAX_PATH];
                if (DragQueryFileW(hDrop, i, f, ARRAYSIZE(f)) > 0) { msg += f; msg += L"\n"; }
            }
            GlobalUnlock(stg.hGlobal);
            MessageBoxW(nullptr, msg.c_str(), L"Shell 扩展示例 · 拖放处理器", MB_OK | MB_ICONINFORMATION);
        }
        ReleaseStgMedium(&stg);
    }
    return S_OK;
}
