#ifndef RENDER_CONTEXT_H
#define RENDER_CONTEXT_H

#include <d2d1.h>
#include <dwrite.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

class RenderContext
{
private:
    ComPtr<ID2D1Factory>          m_pD2DFactory;
    ComPtr<IDWriteFactory>        m_pDWriteFactory;
    ComPtr<ID2D1HwndRenderTarget> m_pRenderTarget;
    ComPtr<ID2D1SolidColorBrush>  m_pSolidBrush;

public:
    RenderContext()
    {
        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, m_pD2DFactory.GetAddressOf());
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(m_pDWriteFactory.GetAddressOf()));
    }

    ~RenderContext()
    {
        DiscardDeviceResources();
        m_pDWriteFactory.Reset();
        m_pD2DFactory.Reset();
    }

    ID2D1Factory* GetFactory() const { return m_pD2DFactory.Get(); }
    IDWriteFactory* GetDWriteFactory() const { return m_pDWriteFactory.Get(); }
    ID2D1HwndRenderTarget* GetRenderTarget() const { return m_pRenderTarget.Get(); }
    ID2D1SolidColorBrush* GetSolidBrush() const { return m_pSolidBrush.Get(); }

    HRESULT CreateDeviceResources(HWND hWnd)
    {
        HRESULT hr = S_OK;
        if (!m_pRenderTarget && hWnd)
        {
            RECT rc;
            GetClientRect(hWnd, &rc);
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

            hr = m_pD2DFactory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(hWnd, size),
                &m_pRenderTarget
            );

            if (SUCCEEDED(hr))
            {
                hr = m_pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(D2D1::ColorF::Black),
                    &m_pSolidBrush
                );
            }
        }
        return hr;
    }

    void DiscardDeviceResources()
    {
        m_pSolidBrush.Reset();
        m_pRenderTarget.Reset();
    }

    void Resize(UINT width, UINT height)
    {
        if (m_pRenderTarget)
        {
            m_pRenderTarget->Resize(D2D1::SizeU(width, height));
        }
    }
};

#endif // RENDER_CONTEXT_H
