//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2DRender.cpp
//    Author:      Streamlet
//    Create Time: 2015-03-14
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------


#include "D2DRender.h"
#pragma comment(lib, "D2d1.lib")
// #pragma comment(lib, "DWrite.lib")


D2DRenderContext::D2DRenderContext(HWND hWnd, D2DRenderer *pRenderer) :
    m_hWnd(hWnd), m_pRenderer(pRenderer), m_pRenderTarget(nullptr), m_pSolidBrush(nullptr), m_pTextFormat(nullptr)
{
    m_pRenderer->m_pDWriteFactory->CreateTextFormat(DEFAULT_FONT_FACE, nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, (float)DEFAULT_FONT_SIZE, _T(""), &m_pTextFormat);
}

D2DRenderContext::~D2DRenderContext()
{
    if (m_pRenderTarget != nullptr)
    {
        m_pRenderTarget->Release();
        m_pRenderTarget = nullptr;
    }
}

void D2DRenderContext::FillSolidRect(LPCRECT lpRect, const RGBQUAD &color)
{
    D2D1_COLOR_F clr = D2D1::ColorF(color.rgbRed / 255.0f, color.rgbGreen / 255.0f, color.rgbBlue / 255.0f, color.rgbReserved / 255.0f);

    if (m_pSolidBrush == nullptr)
    {
        m_pRenderTarget->CreateSolidColorBrush(clr, &m_pSolidBrush);
    }
    else
    {
        m_pSolidBrush->SetColor(clr);
    }

    D2D1_RECT_F rect = D2D1::RectF((float)lpRect->left, (float)lpRect->top, (float)lpRect->right, (float)lpRect->bottom);
    m_pRenderTarget->FillRectangle(rect, m_pSolidBrush);
}

void D2DRenderContext::DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
{
    D2D1_COLOR_F clr = D2D1::ColorF(color.rgbRed / 255.0f, color.rgbGreen / 255.0f, color.rgbBlue / 255.0f, color.rgbReserved / 255.0f);

    if (m_pSolidBrush == nullptr)
    {
        m_pRenderTarget->CreateSolidColorBrush(clr, &m_pSolidBrush);
    }
    else
    {
        m_pSolidBrush->SetColor(clr);
    }

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

void D2DRenderContext::BeginDraw()
{
    RECT rc = {};
    GetClientRect(m_hWnd, &rc);
    D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

    if (m_pRenderTarget == nullptr)
    {
        m_pRenderer->m_pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(m_hWnd, size), &m_pRenderTarget);
    }
    else
    {
        m_pRenderTarget->Resize(size);
    }

    m_pRenderTarget->BeginDraw();
}

void D2DRenderContext::EndDraw()
{
    m_pRenderTarget->EndDraw();
    ValidateRect(m_hWnd, NULL);
}


D2DRenderer::D2DRenderer() : m_pD2DFactory(nullptr), m_pDWriteFactory(nullptr)
{

}

D2DRenderer::~D2DRenderer()
{

}

LPCTSTR D2DRenderer::GetName()
{
    return _T("D2D");
}

bool D2DRenderer::Initialize()
{
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);

    if (FAILED(hr) || m_pD2DFactory == nullptr)
    {
        return false;
    }

    IUnknown *pUnknown = nullptr;
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), &pUnknown);

    if (FAILED(hr) || pUnknown == nullptr)
    {
        return false;
    }

    hr = pUnknown->QueryInterface(&m_pDWriteFactory);
    pUnknown->Release();

    if (FAILED(hr) || m_pDWriteFactory == nullptr)
    {
        return false;
    }

    return true;
}

void D2DRenderer::Uninitialize()
{
    if (m_pDWriteFactory != nullptr)
    {
        m_pDWriteFactory->Release();
        m_pDWriteFactory = nullptr;
    }

    if (m_pD2DFactory != nullptr)
    {
        m_pD2DFactory->Release();
        m_pD2DFactory = nullptr;
    }
}

RenderContext *D2DRenderer::CreateContext(HWND hWnd)
{
    return new D2DRenderContext(hWnd, this);
}

void D2DRenderer::ReleaseContext(RenderContext *pContext)
{
    delete pContext;
}
