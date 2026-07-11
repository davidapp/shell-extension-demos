#include "ThumbnailProvider.h"
#include "common/ShellExtBase.h"
#include <strsafe.h>

ThumbnailProvider::ThumbnailProvider() : m_cRef(1), m_pStream(nullptr) { DllAddRef(); }
ThumbnailProvider::~ThumbnailProvider()
{
    if (m_pStream) m_pStream->Release();
    DllRelease();
}

IFACEMETHODIMP ThumbnailProvider::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IInitializeWithStream)
        *ppv = static_cast<IInitializeWithStream*>(this);
    else if (riid == IID_IThumbnailProvider)
        *ppv = static_cast<IThumbnailProvider*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) ThumbnailProvider::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) ThumbnailProvider::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

// 只保存流；真正读取放到 GetThumbnail 里。
IFACEMETHODIMP ThumbnailProvider::Initialize(IStream* pStream, DWORD)
{
    if (m_pStream) return E_UNEXPECTED; // Shell 保证只调一次
    return pStream->QueryInterface(IID_PPV_ARGS(&m_pStream));
}

IFACEMETHODIMP ThumbnailProvider::GetThumbnail(UINT cx, HBITMAP* phbmp, WTS_ALPHATYPE* pdwAlpha)
{
    if (!phbmp || !pdwAlpha) return E_POINTER;
    *phbmp = nullptr;
    *pdwAlpha = WTSAT_RGB;

    // 用文件大小作为“内容”的代理，演示如何根据内容生成不同缩略图。
    ULONGLONG size = 0;
    if (m_pStream)
    {
        STATSTG st;
        if (SUCCEEDED(m_pStream->Stat(&st, STATFLAG_NONAME)))
            size = st.cbSize.QuadPart;
    }

    // 创建 32bpp、自顶向下的 DIB 段（Shell 缩略图要求 32 位位图）。
    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = static_cast<LONG>(cx);
    bmi.bmiHeader.biHeight = -static_cast<LONG>(cx); // 负 = 自顶向下
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    void* bits = nullptr;
    HBITMAP hbmp = CreateDIBSection(nullptr, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
    if (!hbmp) return E_OUTOFMEMORY;

    HDC hdc = CreateCompatibleDC(nullptr);
    HGDIOBJ oldBmp = SelectObject(hdc, hbmp);

    // 背景色由大小推导（同大小的文件颜色一致）。
    BYTE r = static_cast<BYTE>(50 + (size * 37) % 180);
    BYTE g = static_cast<BYTE>(50 + (size * 71) % 180);
    BYTE b = static_cast<BYTE>(50 + (size * 113) % 180);
    RECT rc = { 0, 0, static_cast<LONG>(cx), static_cast<LONG>(cx) };
    HBRUSH bg = CreateSolidBrush(RGB(r, g, b));
    FillRect(hdc, &rc, bg);
    DeleteObject(bg);

    // 居中画出文件大小。
    wchar_t text[64];
    StringCchPrintfW(text, ARRAYSIZE(text), L"%llu B", size);
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255));
    HFONT font = CreateFontW(-static_cast<LONG>(cx / 8), 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    SelectObject(hdc, oldFont);
    DeleteObject(font);

    GdiFlush();
    SelectObject(hdc, oldBmp);
    DeleteDC(hdc);

    *phbmp = hbmp;
    *pdwAlpha = WTSAT_RGB; // 我们不用 alpha 通道，声明 RGB
    return S_OK;
}
