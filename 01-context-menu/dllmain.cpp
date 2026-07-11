#include <initguid.h>   // 必须在包含 Guid.h 之前，让本 TU 生成 CLSID 的“定义”
#include "Guid.h"
#include "ClassFactory.h"
#include "ModuleRef.h"

#include <windows.h>
#include <shlobj.h>     // SHChangeNotify
#include <olectl.h>     // SELFREG_E_CLASS
#include <strsafe.h>
#include <new>

long g_cDllRef = 0;
HINSTANCE g_hInst = nullptr;

// COM 服务器名（注册表里给人看的友好名）与右键处理器子键名。
static PCWSTR const kFriendlyName = L"FileInfo Context Menu (demo)";
static PCWSTR const kHandlerName = L"FileInfoDemo";

BOOL APIENTRY DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        g_hInst = hInst;
        DisableThreadLibraryCalls(hInst);
    }
    return TRUE;
}

// COM 向我们索取类工厂。
STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void** ppv)
{
    if (rclsid != CLSID_FileInfoContextMenu)
        return CLASS_E_CLASSNOTAVAILABLE;

    ClassFactory* pFactory = new (std::nothrow) ClassFactory();
    if (!pFactory) return E_OUTOFMEMORY;

    HRESULT hr = pFactory->QueryInterface(riid, ppv);
    pFactory->Release();
    return hr;
}

// 没有存活对象时返回 S_OK，允许系统卸载本 DLL。
STDAPI DllCanUnloadNow()
{
    return (g_cDllRef == 0) ? S_OK : S_FALSE;
}

// ------------- 注册辅助 -------------

static LONG WriteString(HKEY root, PCWSTR subkey, PCWSTR name, PCWSTR value)
{
    HKEY hKey;
    LONG r = RegCreateKeyExW(root, subkey, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
    if (r != ERROR_SUCCESS) return r;
    const DWORD cb = static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t));
    r = RegSetValueExW(hKey, name, 0, REG_SZ, reinterpret_cast<const BYTE*>(value), cb);
    RegCloseKey(hKey);
    return r;
}

STDAPI DllRegisterServer()
{
    wchar_t dllPath[MAX_PATH];
    if (!GetModuleFileNameW(g_hInst, dllPath, ARRAYSIZE(dllPath)))
        return HRESULT_FROM_WIN32(GetLastError());

    wchar_t clsid[64];
    StringFromGUID2(CLSID_FileInfoContextMenu, clsid, ARRAYSIZE(clsid));

    wchar_t key[160];

    // HKCR\CLSID\{guid}  (Default) = 友好名
    StringCchPrintfW(key, ARRAYSIZE(key), L"CLSID\\%s", clsid);
    if (WriteString(HKEY_CLASSES_ROOT, key, nullptr, kFriendlyName) != ERROR_SUCCESS)
        return SELFREG_E_CLASS;

    // HKCR\CLSID\{guid}\InProcServer32  (Default) = DLL 路径, ThreadingModel = Apartment
    StringCchPrintfW(key, ARRAYSIZE(key), L"CLSID\\%s\\InProcServer32", clsid);
    WriteString(HKEY_CLASSES_ROOT, key, nullptr, dllPath);
    WriteString(HKEY_CLASSES_ROOT, key, L"ThreadingModel", L"Apartment");

    // HKCR\*\shellex\ContextMenuHandlers\FileInfoDemo  (Default) = {guid}
    // 注册到 "*" 表示对所有文件生效（便于测试；真实项目请注册到具体文件类型的 ProgID 下）。
    StringCchPrintfW(key, ARRAYSIZE(key),
                     L"*\\shellex\\ContextMenuHandlers\\%s", kHandlerName);
    WriteString(HKEY_CLASSES_ROOT, key, nullptr, clsid);

    // 通知系统关联已变化，立即生效，无需重启。
    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}

STDAPI DllUnregisterServer()
{
    wchar_t clsid[64];
    StringFromGUID2(CLSID_FileInfoContextMenu, clsid, ARRAYSIZE(clsid));

    wchar_t key[160];

    StringCchPrintfW(key, ARRAYSIZE(key),
                     L"*\\shellex\\ContextMenuHandlers\\%s", kHandlerName);
    RegDeleteTreeW(HKEY_CLASSES_ROOT, key);

    StringCchPrintfW(key, ARRAYSIZE(key), L"CLSID\\%s", clsid);
    RegDeleteTreeW(HKEY_CLASSES_ROOT, key);

    SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);
    return S_OK;
}
