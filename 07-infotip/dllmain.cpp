#include <initguid.h>
#include "Guid.h"
#include "QueryInfoHandler.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;

// 信息提示处理器的固定类别 GUID 是 {00021500-...}。注册到 txtfile 便于测试。
static PCWSTR const kInfotipKey =
    L"txtfile\\shellex\\{00021500-0000-0000-C000-000000000046}";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { g_hInst = hInst; DisableThreadLibraryCalls(hInst); }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_DemoInfotipHandler) return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<QueryInfoHandler>();
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
    RegisterInprocServer(CLSID_DemoInfotipHandler, dll, L"Demo Infotip Handler");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoInfotipHandler, clsidStr, ARRAYSIZE(clsidStr));
    RegWriteString(HKEY_CLASSES_ROOT, kInfotipKey, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, kInfotipKey);
    UnregisterInprocServer(CLSID_DemoInfotipHandler);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
