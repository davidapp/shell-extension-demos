#pragma once
#include <windows.h>
#include <shlobj.h>   // IShellExtInit, IContextMenu

// 右键菜单处理器：
//   IShellExtInit  —— Shell 通过它把「被右键的文件」交给我们；
//   IContextMenu   —— 我们借它往菜单里加项、并在用户点击时执行。
class ContextMenuHandler : public IShellExtInit, public IContextMenu
{
public:
    ContextMenuHandler();

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IShellExtInit
    IFACEMETHODIMP Initialize(PCIDLIST_ABSOLUTE pidlFolder,
                              IDataObject* pdtobj, HKEY hkeyProgID) override;

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu,
                                    UINT idCmdFirst, UINT idCmdLast, UINT uFlags) override;
    IFACEMETHODIMP InvokeCommand(CMINVOKECOMMANDINFO* pici) override;
    IFACEMETHODIMP GetCommandString(UINT_PTR idCmd, UINT uType,
                                    UINT* pReserved, CHAR* pszName, UINT cchMax) override;

private:
    ~ContextMenuHandler();
    long m_cRef;
    wchar_t m_szFile[MAX_PATH]; // 被右键的第一个文件
    bool m_hasFile;
};
