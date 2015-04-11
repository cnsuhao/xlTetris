//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2D11Render.cpp
//    Author:      Streamlet
//    Create Time: 2015-03-17
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------



#include "D2D11Renderer.h"
#include "APIDynamic.h"
#pragma comment(lib, "dxguid.lib")

#define SAFE_RELEASE_COM_PTR(p) \
    do                          \
    {                           \
        if (p != nullptr)       \
        {                       \
            p->Release();       \
            p = nullptr;        \
        }                       \
    } while (false)

D2D11RenderContext::D2D11RenderContext(HWND hWnd, D2D11Renderer *pRenderer) :
    m_hWnd(hWnd), m_pRenderer(pRenderer), m_pSwapChain(nullptr), m_pDeviceContext(nullptr), m_pSolidBrush(nullptr), m_pTextFormat(nullptr)
{
    ZeroMemory(&m_szLast, sizeof(m_szLast));
    m_pRenderer->m_pDWriteFactory->CreateTextFormat(DEFAULT_FONT_FACE, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (float)DEFAULT_FONT_SIZE, _T(""), &m_pTextFormat);
}

D2D11RenderContext::~D2D11RenderContext()
{
    Uninitialize();
}

void D2D11RenderContext::FillSolidRect(LPCRECT lpRect, const RGBQUAD &color)
{
    D2D1_COLOR_F clr = D2D1::ColorF(color.rgbRed / 255.0f, color.rgbGreen / 255.0f, color.rgbBlue / 255.0f, color.rgbReserved / 255.0f);
    m_pSolidBrush->SetColor(clr);

    D2D1_RECT_F rect = D2D1::RectF((float)lpRect->left, (float)lpRect->top, (float)lpRect->right, (float)lpRect->bottom);
    m_pDeviceContext->FillRectangle(rect, m_pSolidBrush);
}

void D2D11RenderContext::DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
{
    D2D1_COLOR_F clr = D2D1::ColorF(color.rgbRed / 255.0f, color.rgbGreen / 255.0f, color.rgbBlue / 255.0f, color.rgbReserved / 255.0f);
    m_pSolidBrush->SetColor(clr);

    if ((uFormat & DT_CENTER) != 0)
    {
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
    }
    else if ((uFormat & DT_RIGHT) != 0)
    {
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
    }
    else
    {
        m_pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    }

    if ((uFormat & DT_VCENTER) != 0)
    {
        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
    }
    else if ((uFormat & DT_BOTTOM) != 0)
    {
        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
    }
    else
    {
        m_pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    }

    D2D1_RECT_F rect = D2D1::RectF((float)lpRect->left, (float)lpRect->top, (float)lpRect->right, (float)lpRect->bottom);
    m_pDeviceContext->DrawText(lpszext, cchText, m_pTextFormat, rect, m_pSolidBrush);
}

void D2D11RenderContext::DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha)
{
    BITMAP bm = {};
    GetObject(hBitmap, sizeof(bm), &bm);

    BITMAPINFO bmp = {};
    bmp.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmp.bmiHeader.biWidth = bm.bmWidth;
    bmp.bmiHeader.biHeight = -bm.bmHeight;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biBitCount = 32;
    bmp.bmiHeader.biCompression = BI_RGB;

    DWORD *lpBuffer = new DWORD[bm.bmWidth * bm.bmHeight];
    HDC hDC = GetDC(m_hWnd);
    int iLines = GetDIBits(hDC, hBitmap, 0, bm.bmHeight, lpBuffer, &bmp, DIB_RGB_COLORS);
    ReleaseDC(m_hWnd, hDC);

    ID2D1Bitmap *pBitmap = nullptr;
    HRESULT hr = m_pDeviceContext->CreateBitmap(D2D1::SizeU(bm.bmWidth, bm.bmHeight),
        lpBuffer, bm.bmWidthBytes, D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), &pBitmap);

    if (FAILED(hr) || pBitmap == nullptr)
    {
        delete[] lpBuffer;
        return;
    }

    delete[] lpBuffer;

    m_pDeviceContext->DrawBitmap(pBitmap, D2D1::RectF((float)lprcDest->left, (float)lprcDest->top, (float)lprcDest->right, (float)lprcDest->bottom),
        byAlpha / 255.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF((float)lprcSource->left, (float)lprcSource->top, (float)lprcSource->right, (float)lprcSource->bottom));
    pBitmap->Release();
}

void D2D11RenderContext::DrawImageGaussianBlur(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, BYTE byRadius)
{
    BITMAP bm = {};
    GetObject(hBitmap, sizeof(bm), &bm);

    BITMAPINFO bmp = {};
    bmp.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmp.bmiHeader.biWidth = bm.bmWidth;
    bmp.bmiHeader.biHeight = -bm.bmHeight;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biBitCount = 32;
    bmp.bmiHeader.biCompression = BI_RGB;

    DWORD *lpBuffer = new DWORD[bm.bmWidth * bm.bmHeight];
    HDC hDC = GetDC(m_hWnd);
    int iLines = GetDIBits(hDC, hBitmap, 0, bm.bmHeight, lpBuffer, &bmp, DIB_RGB_COLORS);
    ReleaseDC(m_hWnd, hDC);

    ID2D1Bitmap *pBitmap = nullptr;
    HRESULT hr = m_pDeviceContext->CreateBitmap(D2D1::SizeU(bm.bmWidth, bm.bmHeight), lpBuffer, bm.bmWidthBytes,
        D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), &pBitmap);

    if (FAILED(hr) || pBitmap == nullptr)
    {
        delete[] lpBuffer;
        return;
    }

    delete[] lpBuffer;

    ID2D1Effect *pEffect = nullptr;
    hr = m_pDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &pEffect);

    if (FAILED(hr) || pEffect == nullptr)
    {
        pBitmap->Release();
        return;
    }

    pEffect->SetInput(0, pBitmap);
    pEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, (float)byRadius);
    pBitmap->Release();

    ID2D1Bitmap1 *pBitmap1 = nullptr;
    hr = m_pDeviceContext->CreateBitmap(D2D1::SizeU(bm.bmWidth, bm.bmHeight), nullptr, 0,
        D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)), &pBitmap1);

    if (FAILED(hr) || pBitmap1 == nullptr)
    {
        return;
    }

    ID2D1Image *pBackBuffer = nullptr;
    m_pDeviceContext->GetTarget(&pBackBuffer);
    m_pDeviceContext->SetTarget(pBitmap1);

    m_pDeviceContext->DrawImage(pEffect, D2D1::Point2F((float)lprcDest->left, (float)lprcDest->top),
        D2D1::RectF((float)lprcSource->left, (float)lprcSource->top, (float)lprcSource->right, (float)lprcSource->bottom));
    pEffect->Release();

    m_pDeviceContext->SetTarget(pBackBuffer);
    pBackBuffer->Release();

    m_pDeviceContext->DrawBitmap(pBitmap1, D2D1::RectF((float)lprcDest->left, (float)lprcDest->top, (float)lprcDest->right, (float)lprcDest->bottom),
        byAlpha / 255.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR, D2D1::RectF((float)lprcSource->left, (float)lprcSource->top, (float)lprcSource->right, (float)lprcSource->bottom));
    pBitmap1->Release();
}

bool D2D11RenderContext::Initialize()
{
    DXGI_SWAP_CHAIN_DESC1 scd = {};
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.SampleDesc.Count = 1;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    HRESULT hr = m_pRenderer->m_pDxgiFactory->CreateSwapChainForHwnd(m_pRenderer->m_pD3DDevice, m_hWnd, &scd, NULL, NULL, &m_pSwapChain);

    if (FAILED(hr) || m_pSwapChain == nullptr)
    {
        return false;
    }

    hr = m_pRenderer->m_pD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &m_pDeviceContext);

    if (FAILED(hr) || m_pDeviceContext == nullptr)
    {
        return false;
    }

    hr = m_pDeviceContext->CreateSolidColorBrush(D2D1::ColorF(0, 0.0f), &m_pSolidBrush);

    if (FAILED(hr) || m_pSolidBrush == nullptr)
    {
        return false;
    }

    hr = m_pRenderer->m_pDWriteFactory->CreateTextFormat(DEFAULT_FONT_FACE, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (float)DEFAULT_FONT_SIZE, _T(""), &m_pTextFormat);

    if (FAILED(hr) || m_pTextFormat == nullptr)
    {
        return false;
    }

    m_szLast.cx = 0;
    m_szLast.cy = 0;

    return true;
}

void D2D11RenderContext::Uninitialize()
{
    SAFE_RELEASE_COM_PTR(m_pTextFormat);
    SAFE_RELEASE_COM_PTR(m_pSolidBrush);
    SAFE_RELEASE_COM_PTR(m_pDeviceContext);
    SAFE_RELEASE_COM_PTR(m_pSwapChain);
}

void D2D11RenderContext::BeginDraw()
{
    RECT rc = {};
    GetClientRect(m_hWnd, &rc);

    if (m_szLast.cx != rc.right - rc.left || m_szLast.cy != rc.bottom - rc.top)
    {
        m_szLast.cx = rc.right - rc.left;
        m_szLast.cy = rc.bottom - rc.top;

        m_pDeviceContext->SetTarget(nullptr);
        HRESULT hr = m_pSwapChain->ResizeBuffers(0, m_szLast.cx, m_szLast.cy, DXGI_FORMAT_UNKNOWN, 0);

        if (FAILED(hr))
        {
            return;
        }

        IDXGISurface *pSurface;
        hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pSurface));

        if (FAILED(hr) || pSurface == nullptr)
        {
            return;
        }

        float fDpiX = 0.0f, fDpiY = 0.0f;
        m_pRenderer->m_pD2DFactory->GetDesktopDpi(&fDpiX, &fDpiY);

        ID2D1Bitmap1 *pBitmap = nullptr;
        D2D1_BITMAP_PROPERTIES1 dbp = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED), fDpiX, fDpiY);
        hr = m_pDeviceContext->CreateBitmapFromDxgiSurface(pSurface, dbp, &pBitmap);
        SAFE_RELEASE_COM_PTR(pSurface);

        if (FAILED(hr))
        {
            return;
        }

        m_pDeviceContext->SetTarget(pBitmap);
        SAFE_RELEASE_COM_PTR(pBitmap);
    }

    m_pDeviceContext->BeginDraw();
}

void D2D11RenderContext::EndDraw()
{
    m_pDeviceContext->EndDraw();
    m_pSwapChain->Present(1, 0);
    ValidateRect(m_hWnd, NULL);
}


D2D11Renderer::D2D11Renderer() :
    m_pD3DDevice(nullptr), m_pDxgiFactory(nullptr), m_pD2DFactory(nullptr), m_pD2DDevice(nullptr), m_pDWriteFactory(nullptr)
{

}

D2D11Renderer::~D2D11Renderer()
{
    Uninitialize();
}

LPCTSTR D2D11Renderer::GetName()
{
    return _T("D2D1.1");
}

bool D2D11Renderer::Initialize()
{
    D3D_FEATURE_LEVEL eFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    D3D_DRIVER_TYPE eDriverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
        D3D_DRIVER_TYPE_SOFTWARE,
    };

    HRESULT hr = S_OK;
    D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_1;
    ID3D11DeviceContext *pD3DDeviceContext = nullptr;

    for (int i = 0; i < _countof(eDriverTypes); ++i)
    {
        hr = _D3D11CreateDevice(NULL, eDriverTypes[i], NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, eFeatureLevels, _countof(eFeatureLevels), D3D11_SDK_VERSION, &m_pD3DDevice, &eFeatureLevel, &pD3DDeviceContext);

        if (SUCCEEDED(hr))
        {
            break;
        }
    }

    if (FAILED(hr) || m_pD3DDevice == nullptr)
    {
        return false;
    }

    SAFE_RELEASE_COM_PTR(pD3DDeviceContext);

    IDXGIDevice1 *pDxgiDevice = nullptr;
    hr = m_pD3DDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice));

    if (FAILED(hr) || pDxgiDevice == nullptr)
    {
        return false;
    }

    hr = pDxgiDevice->SetMaximumFrameLatency(1);

    if (FAILED(hr))
    {
        return false;
    }

    IDXGIAdapter *pDxgiAdapter = nullptr;
    hr = pDxgiDevice->GetAdapter(&pDxgiAdapter);

    if (FAILED(hr) || pDxgiAdapter == nullptr)
    {
        return false;
    }

    hr = pDxgiAdapter->GetParent(IID_PPV_ARGS(&m_pDxgiFactory));

    if (FAILED(hr) || m_pDxgiFactory == nullptr)
    {
        return false;
    }


    D2D1_FACTORY_OPTIONS dfo = { D2D1_DEBUG_LEVEL_NONE };
    hr = _D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &dfo, (void **)&m_pD2DFactory);

    if (FAILED(hr) || m_pD2DFactory == nullptr)
    {
        return false;
    }

    hr = m_pD2DFactory->CreateDevice(pDxgiDevice, &m_pD2DDevice);

    if (FAILED(hr) || m_pD2DDevice == nullptr)
    {
        return false;
    }

    hr = _DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), (IUnknown **)&m_pDWriteFactory);

    if (FAILED(hr) || m_pDWriteFactory == nullptr)
    {
        return false;
    }

    return true;
}

void D2D11Renderer::Uninitialize()
{
    SAFE_RELEASE_COM_PTR(m_pDWriteFactory);
    SAFE_RELEASE_COM_PTR(m_pD2DFactory);
    SAFE_RELEASE_COM_PTR(m_pDxgiFactory);
    SAFE_RELEASE_COM_PTR(m_pD3DDevice);
}

RenderContext *D2D11Renderer::CreateContext(HWND hWnd)
{
    D2D11RenderContext *pRC = new D2D11RenderContext(hWnd, this);

    if (!pRC->Initialize())
    {
        pRC->Uninitialize();
        delete pRC;
        return nullptr;
    }

    return pRC;
}

void D2D11Renderer::ReleaseContext(RenderContext *pContext)
{
    delete pContext;
}
