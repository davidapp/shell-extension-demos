#include "ClassFactory.h"
#include "ContextMenuHandler.h"
#include "ModuleRef.h"
#include <new>

ClassFactory::ClassFactory() : m_cRef(1) { DllAddRef(); }
ClassFactory::~ClassFactory() { DllRelease(); }

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
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

IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
    long cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0) delete this;
    return cRef;
}

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv)
{
    if (pUnkOuter) return CLASS_E_NOAGGREGATION; // 不支持聚合

    ContextMenuHandler* pHandler = new (std::nothrow) ContextMenuHandler();
    if (!pHandler) return E_OUTOFMEMORY;

    HRESULT hr = pHandler->QueryInterface(riid, ppv);
    pHandler->Release(); // QueryInterface 成功会自增引用；这里释放构造时的那一次
    return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock) DllAddRef(); else DllRelease();
    return S_OK;
}
