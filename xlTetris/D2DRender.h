//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2DRender.h
//    Author:      Streamlet
//    Create Time: 2015-03-14
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __D2DRENDER_H_57B0DFA8_91CD_4B34_B4DF_39C4437292B1_INCLUDED__
#define __D2DRENDER_H_57B0DFA8_91CD_4B34_B4DF_39C4437292B1_INCLUDED__


#include "Renderer.h"
#include <d2d1.h>
#include <dwrite.h>


class D2DRenderer;
class D2DRenderContext : public RenderContext
{
public:
    D2DRenderContext(HWND hWnd, D2DRenderer *pRenderer);
    ~D2DRenderContext();

public:
    // RenderMethods methods
    void FillSolidRect(LPCRECT lpRect, const RGBQUAD &color) override;
    void DrawText(LPCTSTR lpszext, int cchText, LPCRECT lplpRectrc, UINT uFormat, const RGBQUAD &color) override;

public:
    // RenderContext Methods
    void BeginDraw() override;
    void EndDraw() override;

private:
    HWND m_hWnd;
    D2DRenderer *m_pRenderer;
    ID2D1HwndRenderTarget *m_pRenderTarget;
    ID2D1SolidColorBrush *m_pSolidBrush;
    IDWriteTextFormat *m_pTextFormat;

};

class D2DRenderer : public Renderer
{
public:
    D2DRenderer();
    ~D2DRenderer();

public:
    // Render methods
    LPCTSTR GetName() override;
    bool Initialize() override;
    void Uninitialize() override;
    RenderContext *CreateContext(HWND hWnd) override;
    void ReleaseContext(RenderContext *pContext) override;

private:
    friend D2DRenderContext;
    ID2D1Factory *m_pD2DFactory;
    IDWriteFactory *m_pDWriteFactory;
};

#endif // #ifndef __D2DRENDER_H_57B0DFA8_91CD_4B34_B4DF_39C4437292B1_INCLUDED__
