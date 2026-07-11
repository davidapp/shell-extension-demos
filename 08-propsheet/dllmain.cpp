#include <initguid.h>
#include "Guid.h"
#include "PropSheetHandler.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;   // 供 PropSheetHandler.cpp 用（加载对话框资源）

// 属性表处理器可挂多个，子键名任取、默认值为 CLSID。
static PCWSTR const kKey = L"txtfile\\shellex\\PropertySheetHandlers\\DemoPropSheet";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { g_hInst = hInst; DisableThreadLibraryCalls(hInst); }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_DemoPropSheetHandler) return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<PropSheetHandler>();
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
    RegisterInprocServer(CLSID_DemoPropSheetHandler, dll, L"Demo Property Sheet Handler");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoPropSheetHandler, clsidStr, ARRAYSIZE(clsidStr));
    RegWriteString(HKEY_CLASSES_ROOT, kKey, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, kKey);
    UnregisterInprocServer(CLSID_DemoPropSheetHandler);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
