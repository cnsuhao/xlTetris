//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2D10Render.h
//    Author:      Streamlet
//    Create Time: 2015-03-17
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __D2D10RENDER_H_4A811EE0_5DA9_4BCC_A4D1_B04406312E6C_INCLUDED__
#define __D2D10RENDER_H_4A811EE0_5DA9_4BCC_A4D1_B04406312E6C_INCLUDED__


#include <xl/Windows/Memory/xlSafeSmartPtr.h>
#include "Renderer.h"
#include <d2d1.h>
#include <dwrite.h>


class D2D10Renderer;
class D2D10RenderContext : public RenderContext
{
public:
    D2D10RenderContext(HWND hWnd, D2D10Renderer *pRenderer);
    ~D2D10RenderContext();

public:
    // RenderMethods methods
    void FillSolidRect(LPCRECT lpRect, const RGBQUAD &color) override;
    void DrawText(LPCTSTR lpszText, int cchText, LPCRECT lplpRectrc, UINT uFormat, const RGBQUAD &color) override;
    void DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha) override;
    void DrawImageGaussianBlur(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, BYTE byRadius) override;

public:
    // RenderContext Methods
    bool Initialize() override;
    void Uninitialize() override;
    void BeginDraw() override;
    void EndDraw() override;

private:
    HWND m_hWnd;
    SIZE m_szLast;
    D2D10Renderer *m_pRenderer;
    ID2D1HwndRenderTarget *m_pRenderTarget;
    ID2D1SolidColorBrush *m_pSolidBrush;
    IDWriteTextFormat *m_pTextFormat;

};

class D2D10Renderer : public Renderer
{
public:
    D2D10Renderer();
    ~D2D10Renderer();

public:
    // Render methods
    LPCTSTR GetName() override;
    bool Initialize() override;
    void Uninitialize() override;
    RenderContext *CreateContext(HWND hWnd) override;
    void ReleaseContext(RenderContext *pContext) override;

private:
    friend D2D10RenderContext;
    ID2D1Factory *m_pD2DFactory;
    IDWriteFactory *m_pDWriteFactory;
};

#endif // #ifndef __D2D10RENDER_H_4A811EE0_5DA9_4BCC_A4D1_B04406312E6C_INCLUDED__
