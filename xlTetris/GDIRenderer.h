//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   GDIRender.h
//    Author:      Streamlet
//    Create Time: 2015-03-14
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __GDIRENDER_H_92456F59_9926_4F6E_BC0A_5CA884065BE1_INCLUDED__
#define __GDIRENDER_H_92456F59_9926_4F6E_BC0A_5CA884065BE1_INCLUDED__


#include "Renderer.h"

class GDIRenderContext : public RenderContext
{
public:
    GDIRenderContext(HWND hWnd);
    ~GDIRenderContext();

    // RenderMethods methods
    void FillSolidRect(LPCRECT lpRect, const RGBQUAD &color) override;
    void DrawText(LPCTSTR lpszText, int cchText, LPCRECT lplpRectrc, UINT uFormat, const RGBQUAD &color) override;
    void DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha) override;
    void DrawImageGaussianBlur(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, BYTE byRadius) override;

    // RenderContext Methods
    bool Initialize() override;
    void Uninitialize() override;
    void BeginDraw() override;
    void EndDraw() override;

private:
    HWND m_hWnd;
    HFONT m_hFont;
    PAINTSTRUCT m_ps;
    HDC m_hDC;
    HBITMAP m_hOldBitmap;
};

class GDIRenderer : public Renderer
{
public:
    GDIRenderer();
    ~GDIRenderer();

public:
    // Render methods
    LPCTSTR GetName() override;
    bool Initialize() override;
    void Uninitialize() override;
    RenderContext *CreateContext(HWND hWnd) override;
    void ReleaseContext(RenderContext *pContext) override;
};

#endif // #ifndef __GDIRENDER_H_92456F59_9926_4F6E_BC0A_5CA884065BE1_INCLUDED__
