#include <initguid.h>
#include "Guid.h"
#include "OverlayHandler.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;

// 叠加处理器登记在 HKLM 的 ShellIconOverlayIdentifiers 下。
// 名字前置一个空格：该键按名字排序，只有前 15 个生效，空格能抢到靠前的槽位
//（TortoiseGit / Dropbox 都用这招）。
static PCWSTR const kOverlayRoot =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ShellIconOverlayIdentifiers";
static PCWSTR const kOverlayName = L" DaiwDemoOverlay";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { g_hInst = hInst; DisableThreadLibraryCalls(hInst); }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_DemoOverlay) return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<OverlayHandler>();
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
    RegisterInprocServer(CLSID_DemoOverlay, dll, L"Demo Icon Overlay");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoOverlay, clsidStr, ARRAYSIZE(clsidStr));

    wchar_t key[512];
    StringCchPrintfW(key, ARRAYSIZE(key), L"%s\\%s", kOverlayRoot, kOverlayName);
    RegWriteString(HKEY_LOCAL_MACHINE, key, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    wchar_t key[512];
    StringCchPrintfW(key, ARRAYSIZE(key), L"%s\\%s", kOverlayRoot, kOverlayName);
    RegDeleteTreeW(HKEY_LOCAL_MACHINE, key);
    UnregisterInprocServer(CLSID_DemoOverlay);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
