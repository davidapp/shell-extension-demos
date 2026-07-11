#include "PropSheetHandler.h"
#include "resource.h"
#include "common/ShellExtBase.h"
#include <shellapi.h>   // DragQueryFileW
#include <prsht.h>      // PROPSHEETPAGE
#include <strsafe.h>

extern HINSTANCE g_hInst;

PropSheetHandler::PropSheetHandler() : m_cRef(1), m_hasFile(false) { m_szFile[0] = L'\0'; DllAddRef(); }
PropSheetHandler::~PropSheetHandler() { DllRelease(); }

IFACEMETHODIMP PropSheetHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IShellExtInit)
        *ppv = static_cast<IShellExtInit*>(this);
    else if (riid == IID_IShellPropSheetExt)
        *ppv = static_cast<IShellPropSheetExt*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) PropSheetHandler::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) PropSheetHandler::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

IFACEMETHODIMP PropSheetHandler::Initialize(PCIDLIST_ABSOLUTE, IDataObject* pdtobj, HKEY)
{
    m_hasFile = false;
    if (!pdtobj) return E_INVALIDARG;
    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg;
    if (FAILED(pdtobj->GetData(&fe, &stg))) return E_INVALIDARG;
    HDROP hDrop = static_cast<HDROP>(GlobalLock(stg.hGlobal));
    if (hDrop && DragQueryFileW(hDrop, 0, m_szFile, ARRAYSIZE(m_szFile)) > 0)
        m_hasFile = true;
    if (hDrop) GlobalUnlock(stg.hGlobal);
    ReleaseStgMedium(&stg);
    return m_hasFile ? S_OK : E_INVALIDARG;
}

// 页对话框过程：初始化时把文件信息填进静态控件。
static INT_PTR CALLBACK PropPageDlgProc(HWND hDlg, UINT msg, WPARAM, LPARAM lParam)
{
    if (msg == WM_INITDIALOG)
    {
        auto* psp = reinterpret_cast<PROPSHEETPAGE*>(lParam);
        PCWSTR path = reinterpret_cast<PCWSTR>(psp->lParam);
        ULONGLONG size = 0;
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExW(path, GetFileExInfoStandard, &fad))
            size = (static_cast<ULONGLONG>(fad.nFileSizeHigh) << 32) | fad.nFileSizeLow;
        wchar_t text[MAX_PATH + 128];
        StringCchPrintfW(text, ARRAYSIZE(text),
            L"这是由 Shell 扩展示例添加的属性页。\n\n路径：\n%s\n\n大小：%llu 字节", path, size);
        SetDlgItemTextW(hDlg, IDC_INFO, text);
        return TRUE;
    }
    return FALSE;
}

// 页的生命周期回调：加载时 DllAddRef 保活；销毁时释放并回收 lParam 里的路径副本。
static UINT CALLBACK PropPageCallback(HWND, UINT uMsg, LPPROPSHEETPAGE ppsp)
{
    if (uMsg == PSPCB_ADDREF) DllAddRef();
    else if (uMsg == PSPCB_RELEASE) { free(reinterpret_cast<void*>(ppsp->lParam)); DllRelease(); }
    return 1;
}

IFACEMETHODIMP PropSheetHandler::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
    if (!m_hasFile) return E_UNEXPECTED;

    PROPSHEETPAGE psp = {};
    psp.dwSize = sizeof(psp);
    psp.dwFlags = PSP_USETITLE | PSP_USECALLBACK;
    psp.hInstance = g_hInst;
    psp.pszTemplate = MAKEINTRESOURCEW(IDD_PROPPAGE);
    psp.pszTitle = L"Shell 示例";                       // 标签页标题（中文在运行时给）
    psp.pfnDlgProc = PropPageDlgProc;
    psp.lParam = reinterpret_cast<LPARAM>(_wcsdup(m_szFile)); // 传给页的数据（路径副本）
    psp.pfnCallback = PropPageCallback;

    HPROPSHEETPAGE hPage = CreatePropertySheetPageW(&psp);
    if (!hPage) { free(reinterpret_cast<void*>(psp.lParam)); return E_OUTOFMEMORY; }
    if (!lpfnAddPage(hPage, lParam)) { DestroyPropertySheetPage(hPage); return E_FAIL; }
    return S_OK;
}

IFACEMETHODIMP PropSheetHandler::ReplacePage(EXPPS, LPFNADDPROPSHEETPAGE, LPARAM) { return E_NOTIMPL; }
