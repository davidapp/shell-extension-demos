#pragma once
#include <windows.h>
#include <propsys.h>   // IPropertyStore, IInitializeWithStream

// 属性处理器：把文件的元数据以“属性”形式暴露给整个属性系统。
// 一个处理器就能同时喂给：详细信息视图的列、信息提示、分组、搜索、属性页——
// 这也是老“列处理器（IColumnProvider，Vista 已移除）”的现代替代。
class PropertyStore : public IPropertyStore, public IInitializeWithStream
{
public:
    PropertyStore();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IInitializeWithStream
    IFACEMETHODIMP Initialize(IStream* pStream, DWORD grfMode) override;

    // IPropertyStore
    IFACEMETHODIMP GetCount(DWORD* cProps) override;
    IFACEMETHODIMP GetAt(DWORD iProp, PROPERTYKEY* pkey) override;
    IFACEMETHODIMP GetValue(REFPROPERTYKEY key, PROPVARIANT* pv) override;
    IFACEMETHODIMP SetValue(REFPROPERTYKEY key, REFPROPVARIANT propvar) override;
    IFACEMETHODIMP Commit() override;

private:
    ~PropertyStore();
    long m_cRef;
    ULONGLONG m_size;
    bool m_initialized;
};
