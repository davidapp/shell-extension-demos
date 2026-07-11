#pragma once
// 从第 2 章起复用的 COM 地基：DLL 引用计数、通用类工厂模板、注册表小工具。
// 第 1 章（01-context-menu）把这些摊开手写了一遍；这里抽成公共头，之后每个示例只写“处理器本体”。
#include <windows.h>
#include <unknwn.h>
#include <objbase.h>   // StringFromGUID2
#include <strsafe.h>
#include <new>

// ---- DLL 级引用计数（C++17 内联变量，跨编译单元唯一实例）----
inline long g_cDllRef = 0;
inline void DllAddRef() { InterlockedIncrement(&g_cDllRef); }
inline void DllRelease() { InterlockedDecrement(&g_cDllRef); }

// ---- 通用类工厂：THandler 需可默认构造并实现 IUnknown（初始引用计数 1）----
template <class THandler>
class ClassFactory : public IClassFactory
{
public:
    ClassFactory() : m_cRef(1) { DllAddRef(); }

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override
    {
        if (!ppv) return E_POINTER;
        if (riid == IID_IUnknown || riid == IID_IClassFactory)
        {
            *ppv = static_cast<IClassFactory*>(this);
            AddRef();
            return S_OK;
        }
        *ppv = nullptr;
        return E_NOINTERFACE;
    }
    IFACEMETHODIMP_(ULONG) AddRef() override { return InterlockedIncrement(&m_cRef); }
    IFACEMETHODIMP_(ULONG) Release() override
    {
        long c = InterlockedDecrement(&m_cRef);
        if (c == 0) delete this;
        return c;
    }
    IFACEMETHODIMP CreateInstance(IUnknown* outer, REFIID riid, void** ppv) override
    {
        if (outer) return CLASS_E_NOAGGREGATION;
        THandler* p = new (std::nothrow) THandler();
        if (!p) return E_OUTOFMEMORY;
        HRESULT hr = p->QueryInterface(riid, ppv);
        p->Release();
        return hr;
    }
    IFACEMETHODIMP LockServer(BOOL lock) override
    {
        if (lock) DllAddRef(); else DllRelease();
        return S_OK;
    }

private:
    ~ClassFactory() { DllRelease(); }
    long m_cRef;
};

// ---- 注册表小工具 ----
inline LONG RegWriteString(HKEY root, PCWSTR subkey, PCWSTR name, PCWSTR value)
{
    HKEY hKey;
    LONG r = RegCreateKeyExW(root, subkey, 0, nullptr, 0, KEY_WRITE, nullptr, &hKey, nullptr);
    if (r != ERROR_SUCCESS) return r;
    const DWORD cb = static_cast<DWORD>((wcslen(value) + 1) * sizeof(wchar_t));
    r = RegSetValueExW(hKey, name, 0, REG_SZ, reinterpret_cast<const BYTE*>(value), cb);
    RegCloseKey(hKey);
    return r;
}

// HKCR\CLSID\{clsid}\InProcServer32 = dllPath, ThreadingModel=Apartment，并给 CLSID 一个友好名。
inline void RegisterInprocServer(REFCLSID clsid, PCWSTR dllPath, PCWSTR friendlyName)
{
    wchar_t clsidStr[64];
    StringFromGUID2(clsid, clsidStr, ARRAYSIZE(clsidStr));
    wchar_t key[160];
    StringCchPrintfW(key, ARRAYSIZE(key), L"CLSID\\%s", clsidStr);
    RegWriteString(HKEY_CLASSES_ROOT, key, nullptr, friendlyName);
    StringCchPrintfW(key, ARRAYSIZE(key), L"CLSID\\%s\\InProcServer32", clsidStr);
    RegWriteString(HKEY_CLASSES_ROOT, key, nullptr, dllPath);
    RegWriteString(HKEY_CLASSES_ROOT, key, L"ThreadingModel", L"Apartment");
}

inline void UnregisterInprocServer(REFCLSID clsid)
{
    wchar_t clsidStr[64];
    StringFromGUID2(clsid, clsidStr, ARRAYSIZE(clsidStr));
    wchar_t key[160];
    StringCchPrintfW(key, ARRAYSIZE(key), L"CLSID\\%s", clsidStr);
    RegDeleteTreeW(HKEY_CLASSES_ROOT, key);
}
