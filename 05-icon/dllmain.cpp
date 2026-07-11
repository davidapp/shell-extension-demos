#include <initguid.h>
#include "Guid.h"
#include "IconHandler.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;

// 图标处理器注册在文件类型 ProgID 的 shellex\IconHandler 下。
// .txt 的 ProgID 通常是 txtfile（可用 `ftype txtfile` / `assoc .txt` 查证）。
static PCWSTR const kIconKey = L"txtfile\\shellex\\IconHandler";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { g_hInst = hInst; DisableThreadLibraryCalls(hInst); }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_DemoIconHandler) return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<IconHandler>();
    if (!pFactory) return E_OUTOFMEMORY;
    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

STDAPI DllCanUnloadNow() { return (g_cDllRef == 0) ? S_OK : S_FALSE; }

STDAPI DllRegisterServer()
{
    wchar_t dll[MAX_PATH];
    if (!GetModuleFileNameW(g_hInst, dll, ARRAYSIZE(dll)))
        return HRESULT_FROM_WIN32(GetLastError());
    RegisterInprocServer(CLSID_DemoIconHandler, dll, L"Demo Icon Handler");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoIconHandler, clsidStr, ARRAYSIZE(clsidStr));
    RegWriteString(HKEY_CLASSES_ROOT, kIconKey, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, kIconKey);
    UnregisterInprocServer(CLSID_DemoIconHandler);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
