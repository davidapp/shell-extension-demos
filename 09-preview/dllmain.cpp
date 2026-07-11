#include <initguid.h>
#include "Guid.h"
#include "PreviewHandler.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;

// 64 位预览宿主 prevhost.exe 的 AppID（系统已为它注册 DllSurrogate）。
static PCWSTR const kPreviewAppId = L"{534A1E02-D58F-44f0-B58B-36CBED287C7C}";
// 预览处理器的固定类别 GUID。绑到自定义扩展 .myp 便于测试。
static PCWSTR const kExtKey = L".myp\\shellex\\{8895b1c6-b41f-4c1c-a562-0d564250836f}";
static PCWSTR const kPreviewHandlersKey =
    L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\PreviewHandlers";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH) { g_hInst = hInst; DisableThreadLibraryCalls(hInst); }
    return TRUE;
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_DemoPreviewHandler) return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<PreviewHandler>();
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
    RegisterInprocServer(CLSID_DemoPreviewHandler, dll, L"Demo Preview Handler");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoPreviewHandler, clsidStr, ARRAYSIZE(clsidStr));

    // 让本组件在 prevhost.exe 里被隔离托管：CLSID 键加 AppID 值。
    wchar_t clsidKey[96];
    StringCchPrintfW(clsidKey, ARRAYSIZE(clsidKey), L"CLSID\\%s", clsidStr);
    RegWriteString(HKEY_CLASSES_ROOT, clsidKey, L"AppID", kPreviewAppId);

    // 列入系统预览处理器清单（值名 = CLSID，值 = 显示名）。
    RegWriteString(HKEY_LOCAL_MACHINE, kPreviewHandlersKey, clsidStr, L"Demo Preview Handler");

    // 绑定到 .myp。
    RegWriteString(HKEY_CLASSES_ROOT, kExtKey, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoPreviewHandler, clsidStr, ARRAYSIZE(clsidStr));

    RegDeleteTreeW(HKEY_CLASSES_ROOT, kExtKey);
    RegDeleteKeyValueW(HKEY_LOCAL_MACHINE, kPreviewHandlersKey, clsidStr);
    UnregisterInprocServer(CLSID_DemoPreviewHandler);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
