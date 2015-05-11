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
#include "RenderUtility.h"


GDIRenderContext::GDIRenderContext(HWND hWnd) :
    m_hWnd(hWnd), m_hFont(nullptr), m_hDC(nullptr), m_hOldBitmap(nullptr)
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
    SetBkColor(m_hDC, clr);
    ExtTextOut(m_hDC, 0, 0, ETO_OPAQUE, lpRect, NULL, 0, NULL);
}

void GDIRenderContext::DrawText(LPCTSTR lpszText, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
{
    RECT rc = *lpRect;
    COLORREF clr = RGB(color.rgbRed, color.rgbGreen, color.rgbBlue);
    SetTextColor(m_hDC, clr);
    SelectObject(m_hDC, m_hFont);
    SetBkMode(m_hDC, TRANSPARENT);
    ::DrawText(m_hDC, lpszText, cchText, &rc, uFormat);
}

void GDIRenderContext::DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha)
{
    HDC hDC = CreateCompatibleDC(m_ps.hdc);
    HBITMAP hOld = (HBITMAP)SelectObject(hDC, hBitmap);
    BLENDFUNCTION bf = { AC_SRC_OVER, 0, byAlpha, 0 };
    _AlphaBlend(m_hDC, lprcDest->left, lprcDest->top, lprcDest->right - lprcDest->left, lprcDest->bottom - lprcDest->top,
        hDC, lprcSource->left, lprcSource->top, lprcSource->right - lprcSource->left, lprcSource->bottom - lprcSource->top, bf);
    SelectObject(hDC, hOld);
    DeleteDC(hDC);
}

void GDIRenderContext::DrawImageGaussianBlur(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, BYTE byRadius)
{
    HBITMAP hNew = RenderUtility::GaussianBlur(hBitmap, byRadius);
    DrawImage(hNew, lprcDest, lprcSource, byAlpha);
    DeleteObject(hNew);
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
    SIZE sz = { m_ps.rcPaint.right - m_ps.rcPaint.left, m_ps.rcPaint.bottom - m_ps.rcPaint.top };
    m_hDC = CreateCompatibleDC(m_ps.hdc);
    HBITMAP hBitmap = RenderUtility::CreateBitmap(sz.cx, sz.cy, nullptr);
    m_hOldBitmap = (HBITMAP)SelectObject(m_hDC, hBitmap);
    BitBlt(m_hDC, 0, 0, sz.cx, sz.cy, m_ps.hdc, m_ps.rcPaint.left, m_ps.rcPaint.top, SRCCOPY);
    SetViewportOrgEx(m_hDC, -m_ps.rcPaint.left, m_ps.rcPaint.top, nullptr);
}

void GDIRenderContext::EndDraw()
{
    SetViewportOrgEx(m_hDC, 0, 0, nullptr);
    SIZE sz = { m_ps.rcPaint.right - m_ps.rcPaint.left, m_ps.rcPaint.bottom - m_ps.rcPaint.top };
    BitBlt(m_ps.hdc, m_ps.rcPaint.left, m_ps.rcPaint.top, sz.cx, sz.cy, m_hDC, 0, 0, SRCCOPY);
    HBITMAP hBitmap = (HBITMAP)SelectObject(m_hDC, m_hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(m_hDC);
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
