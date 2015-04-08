//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   GDIRender.cpp
//    Author:      Streamlet
//    Create Time: 2015-03-14
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------


#include "GDIRenderer.h"
#include "APIDynamic.h"


GDIRenderContext::GDIRenderContext(HWND hWnd) :
    m_hWnd(hWnd)
{
    ZeroMemory(&m_ps, sizeof(m_ps));
}

GDIRenderContext::~GDIRenderContext()
{
    Uninitialize();
}

void GDIRenderContext::FillSolidRect(LPCRECT lpRect, const RGBQUAD &color)
{
    COLORREF clr = RGB(color.rgbRed, color.rgbGreen, color.rgbBlue);
    SetBkColor(m_ps.hdc, clr);
    ExtTextOut(m_ps.hdc, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}

void GDIRenderContext::DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
{
    RECT rc = *lpRect;
    COLORREF clr = RGB(color.rgbRed, color.rgbGreen, color.rgbBlue);
    SetTextColor(m_ps.hdc, clr);
    SelectObject(m_ps.hdc, m_hFont);
    SetBkMode(m_ps.hdc, TRANSPARENT);
    ::DrawText(m_ps.hdc, lpszext, cchText, &rc, uFormat);
}

void GDIRenderContext::DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha)
{
    HDC hDC = CreateCompatibleDC(m_ps.hdc);
    HBITMAP hOld = (HBITMAP)SelectObject(hDC, hBitmap);
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, byAlpha, AC_SRC_ALPHA };
    _AlphaBlend(m_ps.hdc, lprcDest->left, lprcDest->top, lprcDest->right - lprcDest->left, lprcDest->bottom - lprcDest->top,
        hDC, lprcSource->left, lprcSource->top, lprcSource->right - lprcSource->left, lprcSource->bottom - lprcSource->top, bf);
    SelectObject(hDC, hOld);
    DeleteDC(hDC);
}

bool GDIRenderContext::Initialize()
{
    m_hFont = CreateFont(-DEFAULT_FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DEFAULT_FONT_FACE);

    if (m_hFont == nullptr)
    {
        return false;
    }

    return true;
}

void GDIRenderContext::Uninitialize()
{
    DeleteObject(m_hFont);
}

void GDIRenderContext::BeginDraw()
{
    BeginPaint(m_hWnd, &m_ps);
}

void GDIRenderContext::EndDraw()
{
    EndPaint(m_hWnd, &m_ps);
}


GDIRenderer::GDIRenderer()
{

}

GDIRenderer::~GDIRenderer()
{
    Uninitialize();
}

LPCTSTR GDIRenderer::GetName()
{
    return _T("GDI");
}

bool GDIRenderer::Initialize()
{
    return true;
}

void GDIRenderer::Uninitialize()
{

}

RenderContext *GDIRenderer::CreateContext(HWND hWnd)
{
    RenderContext *pRC = new GDIRenderContext(hWnd);

    if (!pRC->Initialize())
    {
        pRC->Uninitialize();
        delete pRC;
        return nullptr;
    }

    return pRC;
}

void GDIRenderer::ReleaseContext(RenderContext *pContext)
{
    delete pContext;
}
