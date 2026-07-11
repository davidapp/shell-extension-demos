#include "ExplorerCommand.h"
#include "common/ShellExtBase.h"
#include <shlwapi.h>   // SHStrDupW
#include <strsafe.h>

FileInfoCommand::FileInfoCommand() : m_cRef(1) { DllAddRef(); }
FileInfoCommand::~FileInfoCommand() { DllRelease(); }

// ---------------- IUnknown ----------------
IFACEMETHODIMP FileInfoCommand::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IExplorerCommand)
        *ppv = static_cast<IExplorerCommand*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) FileInfoCommand::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) FileInfoCommand::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

// ---------------- IExplorerCommand ----------------
// 返回的字符串必须用 CoTaskMemAlloc 分配（SHStrDupW 就是），Shell 负责释放。
IFACEMETHODIMP FileInfoCommand::GetTitle(IShellItemArray*, LPWSTR* ppszName)
{
    return SHStrDupW(L"显示文件信息 (IExplorerCommand)", ppszName);
}
IFACEMETHODIMP FileInfoCommand::GetIcon(IShellItemArray*, LPWSTR* ppszIcon)
{
    *ppszIcon = nullptr;
    return E_NOTIMPL; // 不提供图标（可返回 "dll,-资源ID" 字符串来指定）
}
IFACEMETHODIMP FileInfoCommand::GetToolTip(IShellItemArray*, LPWSTR* ppszInfotip)
{
    *ppszInfotip = nullptr;
    return E_NOTIMPL;
}
IFACEMETHODIMP FileInfoCommand::GetCanonicalName(GUID* pguid)
{
    *pguid = GUID_NULL;
    return S_OK;
}
IFACEMETHODIMP FileInfoCommand::GetState(IShellItemArray*, BOOL, EXPCMDSTATE* pState)
{
    *pState = ECS_ENABLED; // 也可按选中项返回 ECS_DISABLED / ECS_HIDDEN
    return S_OK;
}
IFACEMETHODIMP FileInfoCommand::GetFlags(EXPCMDFLAGS* pFlags)
{
    *pFlags = ECF_DEFAULT; // 有子命令则用 ECF_HASSUBCOMMANDS
    return S_OK;
}
IFACEMETHODIMP FileInfoCommand::EnumSubCommands(IEnumExplorerCommand** ppEnum)
{
    *ppEnum = nullptr;
    return E_NOTIMPL; // 本例无级联子命令
}

IFACEMETHODIMP FileInfoCommand::Invoke(IShellItemArray* items, IBindCtx*)
{
    if (!items) return E_INVALIDARG;

    DWORD count = 0;
    items->GetCount(&count);
    if (count == 0) return S_OK;

    IShellItem* psi = nullptr;
    if (SUCCEEDED(items->GetItemAt(0, &psi)))
    {
        PWSTR path = nullptr;
        if (SUCCEEDED(psi->GetDisplayName(SIGDN_FILESYSPATH, &path)))
        {
            LARGE_INTEGER size = {};
            WIN32_FILE_ATTRIBUTE_DATA fad;
            if (GetFileAttributesExW(path, GetFileExInfoStandard, &fad))
            {
                size.LowPart = fad.nFileSizeLow;
                size.HighPart = static_cast<LONG>(fad.nFileSizeHigh);
            }
            wchar_t msg[MAX_PATH + 128];
            StringCchPrintfW(msg, ARRAYSIZE(msg),
                L"文件（共选中 %lu 个）：\n%s\n\n大小：%lld 字节",
                count, path, size.QuadPart);
            MessageBoxW(nullptr, msg, L"Shell 扩展示例 · IExplorerCommand",
                        MB_OK | MB_ICONINFORMATION);
            CoTaskMemFree(path);
        }
        psi->Release();
    }
    return S_OK;
}
