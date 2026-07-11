#include <initguid.h>
#include "Guid.h"
#include "ThumbnailProvider.h"
#include "common/ShellExtBase.h"

#include <shlobj.h>     // SHChangeNotify
#include <strsafe.h>
#include <new>

HINSTANCE g_hInst = nullptr;

// 缩略图处理器的固定子键名（{E357FCCD-...} 是系统定义的“Thumbnail image handler”类别）。
// 注册到 .txt 便于测试（右键 .txt 的缩略图会由我们生成）。
static PCWSTR const kThumbKey =
    L".txt\\shellex\\{E357FCCD-A995-4576-B01F-234630154E96}";

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
    if (rclsid != CLSID_DemoThumbnailProvider)
        return CLASS_E_CLASSNOTAVAILABLE;
    auto* pFactory = new (std::nothrow) ClassFactory<ThumbnailProvider>();
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

    RegisterInprocServer(CLSID_DemoThumbnailProvider, dll, L"Demo Thumbnail Provider");

    wchar_t clsidStr[64];
    StringFromGUID2(CLSID_DemoThumbnailProvider, clsidStr, ARRAYSIZE(clsidStr));
    RegWriteString(HKEY_CLASSES_ROOT, kThumbKey, nullptr, clsidStr);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    RegDeleteTreeW(HKEY_CLASSES_ROOT, kThumbKey);
    UnregisterInprocServer(CLSID_DemoThumbnailProvider);
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
