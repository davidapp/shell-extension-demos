#pragma once
#include <windows.h>
#include <shobjidl.h>   // IPreviewHandler
#include <ocidl.h>      // IObjectWithSite
#include <propsys.h>    // IInitializeWithStream

// 预览处理器：在资源管理器“预览窗格”里渲染文件内容。
// 它跑在独立宿主进程 prevhost.exe 里（通过 AppID 指定），崩了不连累资源管理器。
class PreviewHandler : public IInitializeWithStream, public IPreviewHandler, public IObjectWithSite
{
public:
    PreviewHandler();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    // IInitializeWithStream
    IFACEMETHODIMP Initialize(IStream* pStream, DWORD grfMode) override;

    // IPreviewHandler
    IFACEMETHODIMP SetWindow(HWND hwnd, const RECT* prc) override;
    IFACEMETHODIMP SetRect(const RECT* prc) override;
    IFACEMETHODIMP DoPreview() override;
    IFACEMETHODIMP Unload() override;
    IFACEMETHODIMP SetFocus() override;
    IFACEMETHODIMP QueryFocus(HWND* phwnd) override;
    IFACEMETHODIMP TranslateAccelerator(MSG* pmsg) override;

    // IObjectWithSite
    IFACEMETHODIMP SetSite(IUnknown* pUnkSite) override;
    IFACEMETHODIMP GetSite(REFIID riid, void** ppv) override;

private:
    ~PreviewHandler();
    long m_cRef;
    HWND m_hwndParent;
    RECT m_rc;
    HWND m_hwndPreview;   // 我们创建的子窗口（只读多行 EDIT）
    IStream* m_pStream;
    IUnknown* m_pSite;
};
