#pragma once
#include <windows.h>
#include <shlobj.h>   // IPersistFile, IDropTarget

// 拖放处理器：让某类文件成为“拖放目标”——把文件拖到它上面松手时，你的代码被调用。
class DropHandler : public IPersistFile, public IDropTarget
{
public:
    DropHandler();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IPersist / IPersistFile —— 拿到“被拖放到的目标文件”路径
    IFACEMETHODIMP GetClassID(CLSID* pClassID) override;
    IFACEMETHODIMP IsDirty() override;
    IFACEMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode) override;
    IFACEMETHODIMP Save(LPCOLESTR, BOOL) override;
    IFACEMETHODIMP SaveCompleted(LPCOLESTR) override;
    IFACEMETHODIMP GetCurFile(LPOLESTR*) override;

    // IDropTarget
    IFACEMETHODIMP DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    IFACEMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;
    IFACEMETHODIMP DragLeave() override;
    IFACEMETHODIMP Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect) override;

private:
    ~DropHandler();
    long m_cRef;
    wchar_t m_szTarget[MAX_PATH];
};
