#pragma once
#include <windows.h>
#include <shlobj.h>   // IShellExtInit, IShellPropSheetExt

// 属性表处理器：往文件「属性」对话框里加一个自定义页。
class PropSheetHandler : public IShellExtInit, public IShellPropSheetExt
{
public:
    PropSheetHandler();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder, IDataObject* pdtobj, HKEY hkeyProgID) override;

    // IShellPropSheetExt
    IFACEMETHODIMP AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam) override;
    IFACEMETHODIMP ReplacePage(EXPPS uPageID, LPFNADDPROPSHEETPAGE lpfnReplace, LPARAM lParam) override;

private:
    ~PropSheetHandler();
    long m_cRef;
    wchar_t m_szFile[MAX_PATH];
    bool m_hasFile;
};
