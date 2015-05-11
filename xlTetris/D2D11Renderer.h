//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2D11Render.h
//    Author:      Streamlet
//    Create Time: 2015-03-17
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __D2D11RENDER_H_DC0866E4_E9D7_4A01_9DD3_5637D65C08ED_INCLUDED__
#define __D2D11RENDER_H_DC0866E4_E9D7_4A01_9DD3_5637D65C08ED_INCLUDED__


#include "Renderer.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>


class D2D11Renderer;
class D2D11RenderContext : public RenderContext
{
public:
    D2D11RenderContext(HWND hWnd, D2D11Renderer *pRenderer);
    ~D2D11RenderContext();

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
    D2D11Renderer *m_pRenderer;
    IDXGISwapChain1 *m_pSwapChain;
    ID2D1DeviceContext *m_pDeviceContext;
    ID2D1SolidColorBrush *m_pSolidBrush;
    IDWriteTextFormat *m_pTextFormat;

};

class D2D11Renderer : public Renderer
{
public:
    D2D11Renderer();
    ~D2D11Renderer();

public:
    // Render methods
    LPCTSTR GetName() override;
    bool Initialize() override;
    void Uninitialize() override;
    RenderContext *CreateContext(HWND hWnd) override;
    void ReleaseContext(RenderContext *pContext) override;

private:
    friend D2D11RenderContext;
    ID3D11Device *m_pD3DDevice;
    IDXGIFactory2 *m_pDxgiFactory;
    ID2D1Factory1 *m_pD2DFactory;
    ID2D1Device *m_pD2DDevice;
    IDWriteFactory *m_pDWriteFactory;
};

#endif // #ifndef __D2D11RENDER_H_DC0866E4_E9D7_4A01_9DD3_5637D65C08ED_INCLUDED__
