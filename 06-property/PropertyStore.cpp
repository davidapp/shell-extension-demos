#include "PropertyStore.h"
#include "common/ShellExtBase.h"
#include <propkey.h>       // PKEY_Comment
#include <propvarutil.h>   // InitPropVariantFromString
#include <strsafe.h>

PropertyStore::PropertyStore() : m_cRef(1), m_size(0), m_initialized(false) { DllAddRef(); }
PropertyStore::~PropertyStore() { DllRelease(); }

IFACEMETHODIMP PropertyStore::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IPropertyStore)
        *ppv = static_cast<IPropertyStore*>(this);
    else if (riid == IID_IInitializeWithStream)
        *ppv = static_cast<IInitializeWithStream*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) PropertyStore::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) PropertyStore::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

IFACEMETHODIMP PropertyStore::Initialize(IStream* pStream, DWORD)
{
    if (m_initialized) return E_UNEXPECTED;
    STATSTG st;
    if (pStream && SUCCEEDED(pStream->Stat(&st, STATFLAG_NONAME)))
        m_size = st.cbSize.QuadPart;
    m_initialized = true;
    return S_OK;
}

// 本例只暴露 1 个属性：System.Comment（PKEY_Comment），值由文件大小算出。
IFACEMETHODIMP PropertyStore::GetCount(DWORD* cProps)
{
    if (!cProps) return E_POINTER;
    *cProps = 1;
    return S_OK;
}
IFACEMETHODIMP PropertyStore::GetAt(DWORD iProp, PROPERTYKEY* pkey)
{
    if (!pkey) return E_POINTER;
    if (iProp != 0) return E_INVALIDARG;
    *pkey = PKEY_Comment;
    return S_OK;
}
IFACEMETHODIMP PropertyStore::GetValue(REFPROPERTYKEY key, PROPVARIANT* pv)
{
    if (!pv) return E_POINTER;
    PropVariantInit(pv);   // 未知属性默认返回 VT_EMPTY
    if (IsEqualPropertyKey(key, PKEY_Comment))
    {
        wchar_t buf[96];
        StringCchPrintfW(buf, ARRAYSIZE(buf), L"演示属性：该文件 %llu 字节", m_size);
        return InitPropVariantFromString(buf, pv);
    }
    return S_OK;
}
// 本例只读。
IFACEMETHODIMP PropertyStore::SetValue(REFPROPERTYKEY, REFPROPVARIANT) { return STG_E_ACCESSDENIED; }
IFACEMETHODIMP PropertyStore::Commit() { return STG_E_ACCESSDENIED; }
