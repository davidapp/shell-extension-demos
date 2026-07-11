#include "ContextMenuHandler.h"
#include "ModuleRef.h"
#include <shellapi.h>   // HDROP, DragQueryFileW
#include <shlwapi.h>    // StrCmpIA
#include <strsafe.h>

// 本处理器只提供 1 个命令，偏移量固定为 0。
#define IDM_SHOWINFO 0

ContextMenuHandler::ContextMenuHandler() : m_cRef(1), m_hasFile(false)
{
    m_szFile[0] = L'\0';
    DllAddRef();
}

ContextMenuHandler::~ContextMenuHandler() { DllRelease(); }

// ---------------- IUnknown ----------------

IFACEMETHODIMP ContextMenuHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IShellExtInit)
        *ppv = static_cast<IShellExtInit*>(this);
    else if (riid == IID_IContextMenu)
        *ppv = static_cast<IContextMenu*>(this);
    else
    {
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
}

IFACEMETHODIMP_(ULONG) ContextMenuHandler::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) ContextMenuHandler::Release()
{
    long cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0) delete this;
    return cRef;
}

// ---------------- IShellExtInit ----------------
// Shell 在弹出菜单前调用它，把被右键的对象通过 IDataObject 传进来。
// 这里取出其中第一个文件的完整路径。
IFACEMETHODIMP ContextMenuHandler::Initialize(PCIDLIST_ABSOLUTE /*pidlFolder*/,
                                              IDataObject* pdtobj, HKEY /*hkeyProgID*/)
{
    m_hasFile = false;
    if (!pdtobj) return E_INVALIDARG;

    FORMATETC fe = { CF_HDROP, nullptr, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
    STGMEDIUM stg;
    if (FAILED(pdtobj->GetData(&fe, &stg))) return E_INVALIDARG;

    HDROP hDrop = static_cast<HDROP>(GlobalLock(stg.hGlobal));
    if (hDrop)
    {
        // 选了几个文件（这里只取第一个；真实扩展应遍历全部）。
        UINT count = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);
        if (count >= 1 &&
            DragQueryFileW(hDrop, 0, m_szFile, ARRAYSIZE(m_szFile)) > 0)
        {
            m_hasFile = true;
        }
        GlobalUnlock(stg.hGlobal);
    }
    ReleaseStgMedium(&stg);
    return m_hasFile ? S_OK : E_INVALIDARG;
}

// ---------------- IContextMenu ----------------

// 往菜单里插入我们的项。返回值的低 16 位 = 用掉的命令 ID 个数。
IFACEMETHODIMP ContextMenuHandler::QueryContextMenu(HMENU hMenu, UINT indexMenu,
                                                    UINT idCmdFirst, UINT /*idCmdLast*/, UINT uFlags)
{
    // 只请求默认项时不加自定义命令。
    if (uFlags & CMF_DEFAULTONLY)
        return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, 0);

    InsertMenuW(hMenu, indexMenu, MF_BYPOSITION | MF_STRING,
                idCmdFirst + IDM_SHOWINFO, L"显示文件信息(&I)");

    // 用了 1 个命令 ID（偏移 0），故返回 1。
    return MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NULL, IDM_SHOWINFO + 1);
}

// 用户点击菜单项时执行。
IFACEMETHODIMP ContextMenuHandler::InvokeCommand(CMINVOKECOMMANDINFO* pici)
{
    if (!pici) return E_INVALIDARG;

    // 命令既可能以「偏移量」也可能以「字符串 verb」形式传入，两种都要认。
    if (IS_INTRESOURCE(pici->lpVerb))
    {
        if (LOWORD(pici->lpVerb) != IDM_SHOWINFO) return E_INVALIDARG;
    }
    else
    {
        if (StrCmpIA(pici->lpVerb, "showinfo") != 0) return E_INVALIDARG;
    }

    if (!m_hasFile) return E_UNEXPECTED;

    LARGE_INTEGER size = {};
    WIN32_FILE_ATTRIBUTE_DATA fad;
    if (GetFileAttributesExW(m_szFile, GetFileExInfoStandard, &fad))
    {
        size.LowPart = fad.nFileSizeLow;
        size.HighPart = static_cast<LONG>(fad.nFileSizeHigh);
    }

    wchar_t msg[MAX_PATH + 128];
    StringCchPrintfW(msg, ARRAYSIZE(msg),
                     L"文件：\n%s\n\n大小：%lld 字节", m_szFile, size.QuadPart);
    MessageBoxW(pici->hwnd, msg, L"Shell 扩展示例 · 文件信息",
                MB_OK | MB_ICONINFORMATION);
    return S_OK;
}

// 为命令提供 verb 名（供脚本调用）与状态栏帮助文本。
IFACEMETHODIMP ContextMenuHandler::GetCommandString(UINT_PTR idCmd, UINT uType,
                                                    UINT* /*pReserved*/, CHAR* pszName, UINT cchMax)
{
    if (idCmd != IDM_SHOWINFO) return E_INVALIDARG;
    switch (uType)
    {
    case GCS_VERBW:     return StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchMax, L"showinfo");
    case GCS_VERBA:     return StringCchCopyA(pszName, cchMax, "showinfo");
    case GCS_HELPTEXTW: return StringCchCopyW(reinterpret_cast<PWSTR>(pszName), cchMax, L"显示所选文件的路径与大小");
    case GCS_HELPTEXTA: return StringCchCopyA(pszName, cchMax, "Show the path and size of the selected file");
    default:            return S_OK;
    }
}
