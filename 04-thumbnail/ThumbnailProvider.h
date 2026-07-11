#pragma once
#include <windows.h>
#include <thumbcache.h>   // IThumbnailProvider, WTS_ALPHATYPE
#include <propsys.h>      // IInitializeWithStream

// 缩略图处理器（取代老的 IExtractImage）：
//   IInitializeWithStream —— Shell 以“流”的形式把文件内容交给我们（在隔离宿主里运行，最安全）；
//   IThumbnailProvider    —— 给定边长 cx，返回一张 HBITMAP 缩略图。
class ThumbnailProvider : public IInitializeWithStream, public IThumbnailProvider
{
public:
    ThumbnailProvider();

    IFACEMETHODIMP QueryInterface(REFIID riid, void** ppv) override;
    IFACEMETHODIMP_(ULONG) AddRef() override;
    IFACEMETHODIMP_(ULONG) Release() override;

    IFACEMETHODIMP Initialize(IStream* pStream, DWORD grfMode) override;
    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha) override;

private:
    ~ThumbnailProvider();
    long m_cRef;
    IStream* m_pStream;
};
