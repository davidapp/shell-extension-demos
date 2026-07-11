#include <initguid.h>   // 让本 TU 生成 CLSID 定义
#include "Guid.h"
#include "ExplorerCommand.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>     // SHChangeNotify
#include <olectl.h>     // SELFREG_E_CLASS
#include <strsafe.h>

HINSTANCE g_hInst = nullptr;

// 用 IExplorerCommand 实现的 verb，注册在 HKCR\<type>\shell\<verb> 下，
// 通过 ExplorerCommandHandler 值指向本 CLSID（区别于第 1 章的 ShellEx\ContextMenuHandlers）。
static PCWSTR const kVerbKey = L"*\\shell\\FileInfoExplorerCmd";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_hInst = hInst;
        DisableThreadLibraryCalls(hInst);
    }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_FileInfoExplorerCommand)
        return CLASS_E_CLASSNOTAVAILABLE;

    auto* pFactory = new (std::nothrow) ClassFactory<FileInfoCommand>();
    if (!pFactory) return E_OUTOFMEMORY;
    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow()
{
    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}

STDAPI DllRegisterServer()
{
    wchar_t dll[MAX_PATH];
    if (!GetModuleFileNameW(g_hInst, dll, ARRAYSIZE(dll)))
        return HRESULT_FROM_WIN32(GetLastError());

    RegisterInprocServer(CLSID_FileInfoExplorerCommand, dll,
                         L"FileInfo ExplorerCommand (demo)");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_FileInfoExplorerCommand, clsidStr, ARRAYSIZE(clsidStr));

    // (默认) = 菜单显示名；ExplorerCommandHandler = {CLSID}
    RegWriteString(HKEY_CLASSES_ROOT, kVerbKey, nullptr, L"显示文件信息 (IExplorerCommand)");
    RegWriteString(HKEY_CLASSES_ROOT, kVerbKey, L"ExplorerCommandHandler", clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, kVerbKey);
    UnregisterInprocServer(CLSID_FileInfoExplorerCommand);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
