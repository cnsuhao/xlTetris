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


#include "GDIRender.h"


GDIRenderContext::GDIRenderContext(HWND hWnd) :
    m_hWnd(hWnd)
{
    ZeroMemory(&m_ps, sizeof(m_ps));
    m_hFont = CreateFont(-DEFAULT_FONT_SIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DEFAULT_FONT_FACE);
}

GDIRenderContext::~GDIRenderContext()
{
    DeleteObject(m_hFont);
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
    return new GDIRenderContext(hWnd);
}

void GDIRenderer::ReleaseContext(RenderContext *pContext)
{
    delete pContext;
}
