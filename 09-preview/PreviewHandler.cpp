#include "PreviewHandler.h"
#include "common/ShellExtBase.h"
#include <vector>

extern HINSTANCE g_hInst;

PreviewHandler::PreviewHandler()
    : m_cRef(1), m_hwndParent(nullptr), m_rc{}, m_hwndPreview(nullptr),
      m_pStream(nullptr), m_pSite(nullptr) { DllAddRef(); }

PreviewHandler::~PreviewHandler()
{
    if (m_hwndPreview) DestroyWindow(m_hwndPreview);
    if (m_pStream) m_pStream->Release();
    if (m_pSite) m_pSite->Release();
    DllRelease();
}

IFACEMETHODIMP PreviewHandler::QueryInterface(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    if (riid == IID_IUnknown || riid == IID_IInitializeWithStream)
        *ppv = static_cast<IInitializeWithStream*>(this);
    else if (riid == IID_IPreviewHandler)
        *ppv = static_cast<IPreviewHandler*>(this);
    else if (riid == IID_IObjectWithSite)
        *ppv = static_cast<IObjectWithSite*>(this);
    else { *ppv = nullptr; return E_NOINTERFACE; }
    AddRef();
    return S_OK;
}
IFACEMETHODIMP_(ULONG) PreviewHandler::AddRef() { return InterlockedIncrement(&m_cRef); }
IFACEMETHODIMP_(ULONG) PreviewHandler::Release()
{
    long c = InterlockedDecrement(&m_cRef);
    if (c == 0) delete this;
    return c;
}

IFACEMETHODIMP PreviewHandler::Initialize(IStream* pStream, DWORD)
{
    if (m_pStream) m_pStream->Release();
    return pStream->QueryInterface(IID_PPV_ARGS(&m_pStream));
}

IFACEMETHODIMP PreviewHandler::SetWindow(HWND hwnd, const RECT* prc)
{
    m_hwndParent = hwnd;
    if (prc) m_rc = *prc;
    if (m_hwndPreview && prc)
        MoveWindow(m_hwndPreview, m_rc.left, m_rc.top,
                   m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, TRUE);
    return S_OK;
}
IFACEMETHODIMP PreviewHandler::SetRect(const RECT* prc)
{
    if (prc) m_rc = *prc;
    if (m_hwndPreview && prc)
        MoveWindow(m_hwndPreview, m_rc.left, m_rc.top,
                   m_rc.right - m_rc.left, m_rc.bottom - m_rc.top, TRUE);
    return S_OK;
}

// 真正渲染：在父窗口里建一个只读多行 EDIT，把文件内容当文本显示。
IFACEMETHODIMP PreviewHandler::DoPreview()
{
    if (m_hwndPreview || !m_hwndParent) return S_OK;

    m_hwndPreview = CreateWindowExW(0, L"EDIT", nullptr,
        WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
        m_rc.left, m_rc.top, m_rc.right - m_rc.left, m_rc.bottom - m_rc.top,
        m_hwndParent, nullptr, g_hInst, nullptr);
    if (!m_hwndPreview) return E_FAIL;

    // 读最多 64KB 内容，按 UTF-8 转宽字符显示。
    if (m_pStream)
    {
        LARGE_INTEGER zero = {};
        m_pStream->Seek(zero, STREAM_SEEK_SET, nullptr);
        std::vector<char> buf(64 * 1024);
        ULONG read = 0;
        if (SUCCEEDED(m_pStream->Read(buf.data(), (ULONG)buf.size() - 1, &read)) && read > 0)
        {
            buf[read] = '\0';
            int wlen = MultiByteToWideChar(CP_UTF8, 0, buf.data(), (int)read, nullptr, 0);
            std::vector<wchar_t> wbuf(wlen + 1);
            MultiByteToWideChar(CP_UTF8, 0, buf.data(), (int)read, wbuf.data(), wlen);
            wbuf[wlen] = L'\0';
            SetWindowTextW(m_hwndPreview, wbuf.data());
        }
    }
    return S_OK;
}

IFACEMETHODIMP PreviewHandler::Unload()
{
    if (m_hwndPreview) { DestroyWindow(m_hwndPreview); m_hwndPreview = nullptr; }
    if (m_pStream) { m_pStream->Release(); m_pStream = nullptr; }
    return S_OK;
}
IFACEMETHODIMP PreviewHandler::SetFocus()
{
    if (m_hwndPreview) ::SetFocus(m_hwndPreview);
    return S_OK;
}
IFACEMETHODIMP PreviewHandler::QueryFocus(HWND* phwnd)
{
    if (!phwnd) return E_POINTER;
    *phwnd = ::GetFocus();
    return *phwnd ? S_OK : HRESULT_FROM_WIN32(GetLastError());
}
IFACEMETHODIMP PreviewHandler::TranslateAccelerator(MSG*) { return S_FALSE; }

IFACEMETHODIMP PreviewHandler::SetSite(IUnknown* pUnkSite)
{
    if (m_pSite) { m_pSite->Release(); m_pSite = nullptr; }
    m_pSite = pUnkSite;
    if (m_pSite) m_pSite->AddRef();
    return S_OK;
}
IFACEMETHODIMP PreviewHandler::GetSite(REFIID riid, void** ppv)
{
    if (!ppv) return E_POINTER;
    *ppv = nullptr;
    return m_pSite ? m_pSite->QueryInterface(riid, ppv) : E_FAIL;
}
