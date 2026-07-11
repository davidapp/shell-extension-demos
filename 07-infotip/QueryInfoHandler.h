#pragma once
#include <windows.h>
#include <shlobj.h>   // IQueryInfo

// 信息提示（infotip）处理器：鼠标悬停文件时弹出的那段说明文字。
class QueryInfoHandler : public IPersistFile, public IQueryInfo
{
public:
    QueryInfoHandler();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IPersist / IPersistFile
    IFACEMETHODIMP GetClassID(CLSID* pClassID) override;
    IFACEMETHODIMP IsDirty() override;
    IFACEMETHODIMP Load(LPCOLESTR pszFileName, DWORD dwMode) override;
    IFACEMETHODIMP Save(LPCOLESTR, BOOL) override;
    IFACEMETHODIMP SaveCompleted(LPCOLESTR) override;
    IFACEMETHODIMP GetCurFile(LPOLESTR*) override;

    // IQueryInfo
    IFACEMETHODIMP GetInfoTip(DWORD dwFlags, PWSTR* ppwszTip) override;
    IFACEMETHODIMP GetInfoFlags(DWORD* pdwFlags) override;

private:
    ~QueryInfoHandler();
    long m_cRef;
    wchar_t m_szFile[MAX_PATH];
};
