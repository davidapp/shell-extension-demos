#include <initguid.h>
#include "Guid.h"
#include "PropertyStore.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;

// 属性处理器注册在 HKLM 的属性系统里，按扩展名绑定。用自定义扩展 .shdemo 便于测试，
// 不影响 .txt 等系统已有的属性处理器。
static PCWSTR const kPropHandlerKey =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\PropertySystem\\PropertyHandlers\\.shdemo";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { g_hInst = hInst; DisableThreadLibraryCalls(hInst); }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_DemoPropertyHandler) return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<PropertyStore>();
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
    RegisterInprocServer(CLSID_DemoPropertyHandler, dll, L"Demo Property Handler");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoPropertyHandler, clsidStr, ARRAYSIZE(clsidStr));
    RegWriteString(HKEY_LOCAL_MACHINE, kPropHandlerKey, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_LOCAL_MACHINE, kPropHandlerKey);
    UnregisterInprocServer(CLSID_DemoPropertyHandler);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
