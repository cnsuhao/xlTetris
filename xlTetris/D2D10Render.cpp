//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2D10Render.cpp
//    Author:      Streamlet
//    Create Time: 2015-03-17
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------



#include "D2D10Render.h"
#include "D2DDynamic.h"

#define SAFE_RELEASE_COM_PTR(p) \
    do                          \
    {                           \
        if (p != nullptr)       \
        {                       \
            p->Release();       \
            p = nullptr;        \
        }                       \
    } while (false)

D2D10RenderContext::D2D10RenderContext(HWND hWnd, D2D10Renderer *pRenderer) :
    m_hWnd(hWnd), m_pRenderer(pRenderer), m_pRenderTarget(nullptr), m_pSolidBrush(nullptr), m_pTextFormat(nullptr)
{

}

D2D10RenderContext::~D2D10RenderContext()
{
    Uninitialize();
}

void D2D10RenderContext::FillSolidRect(LPCRECT lpRect, const RGBQUAD &color)
{
    D2D1_COLOR_F clr = D2D1::ColorF(color.rgbRed / 255.0f, color.rgbGreen / 255.0f, color.rgbBlue / 255.0f, color.rgbReserved / 255.0f);
    m_pSolidBrush->SetColor(clr);

    D2D1_RECT_F rect = D2D1::RectF((float)lpRect->left, (float)lpRect->top, (float)lpRect->right, (float)lpRect->bottom);
    m_pRenderTarget->FillRectangle(rect, m_pSolidBrush);
}

void D2D10RenderContext::DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
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
    m_pRenderTarget->DrawText(lpszext, cchText, m_pTextFormat, rect, m_pSolidBrush);
}

bool D2D10RenderContext::Initialize()
{
    HRESULT hr = m_pRenderer->m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, D2D1::SizeU()), &m_pRenderTarget);

    if (FAILED(hr) || m_pRenderTarget == nullptr)
    {
        return false;
    }

    hr = m_pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0, 0.0f), &m_pSolidBrush);

    if (FAILED(hr) || m_pSolidBrush == nullptr)
    {
        return false;
    }

    hr = m_pRenderer->m_pDWriteFactory->CreateTextFormat(DEFAULT_FONT_FACE, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (float)DEFAULT_FONT_SIZE, _T(""), &m_pTextFormat);

    if (FAILED(hr) || m_pTextFormat == nullptr)
    {
        return false;
    }

    return true;
}

void D2D10RenderContext::Uninitialize()
{
    SAFE_RELEASE_COM_PTR(m_pTextFormat);
    SAFE_RELEASE_COM_PTR(m_pSolidBrush);
    SAFE_RELEASE_COM_PTR(m_pRenderTarget);
}

void D2D10RenderContext::BeginDraw()
{
    static int nLastWidth = 0;
    static int nLastHeight = 0;
    RECT rc = {};
    GetClientRect(m_hWnd, &rc);

    if (nLastWidth != rc.right - rc.left || nLastHeight != rc.bottom - rc.top)
    {
        nLastWidth = rc.right - rc.left;
        nLastHeight = rc.bottom - rc.top;

        m_pRenderTarget->Resize(D2D1::SizeU(nLastWidth, nLastHeight));
    }

    m_pRenderTarget->BeginDraw();
}

void D2D10RenderContext::EndDraw()
{
    m_pRenderTarget->EndDraw();
    ValidateRect(m_hWnd, NULL);
}


D2D10Renderer::D2D10Renderer() : m_pD2DFactory(nullptr), m_pDWriteFactory(nullptr)
{

}

D2D10Renderer::~D2D10Renderer()
{
    Uninitialize();
}

LPCTSTR D2D10Renderer::GetName()
{
    return _T("D2D1.0");
}

bool D2D10Renderer::Initialize()
{
    D2D1_FACTORY_OPTIONS dfo = { D2D1_DEBUG_LEVEL_NONE };
    HRESULT hr = _D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), &dfo, (void **)&m_pD2DFactory);

    if (FAILED(hr) || m_pD2DFactory == nullptr)
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

void D2D10Renderer::Uninitialize()
{
    SAFE_RELEASE_COM_PTR(m_pDWriteFactory);
    SAFE_RELEASE_COM_PTR(m_pD2DFactory);
}

RenderContext *D2D10Renderer::CreateContext(HWND hWnd)
{
    D2D10RenderContext *pRC = new D2D10RenderContext(hWnd, this);

    if (!pRC->Initialize())
    {
        pRC->Uninitialize();
        delete pRC;
        return nullptr;
    }

    return pRC;
}

void D2D10Renderer::ReleaseContext(RenderContext *pContext)
{
    delete pContext;
}
