#pragma once
#include <windows.h>
#include <unknwn.h>

// 类工厂：COM 通过 DllGetClassObject 拿到它，再用 CreateInstance 造出真正的处理器对象。
class ClassFactory : public IClassFactory
{
public:
    ClassFactory();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown* pUnkOuter, REFIID riid, void** ppv) override;
    IFACEMETHODIMP LockServer(BOOL fLock) override;

private:
    ~ClassFactory();
    long m_cRef;
};
