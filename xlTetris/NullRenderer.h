//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   NullRenderer.h
//    Author:      Streamlet
//    Create Time: 2015-04-03
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __NULLRENDERER_H_F649909F_9650_4919_972A_FA57CC1F92EA_INCLUDED__
#define __NULLRENDERER_H_F649909F_9650_4919_972A_FA57CC1F92EA_INCLUDED__


#include "Renderer.h"

class NullRenderContext : public RenderContext
{
public:
    // RenderMethods methods
    void FillSolidRect(LPCRECT lpRect, const RGBQUAD &color) override;
    void DrawText(LPCTSTR lpszext, int cchText, LPCRECT lplpRectrc, UINT uFormat, const RGBQUAD &color) override;

    // RenderContext Methods
    bool Initialize() override;
    void Uninitialize() override;
    void BeginDraw() override;
    void EndDraw() override;
};

class NullRenderer : public Renderer
{
public:
    // Render methods
    LPCTSTR GetName() override;
    bool Initialize() override;
    void Uninitialize() override;
    RenderContext *CreateContext(HWND hWnd) override;
    void ReleaseContext(RenderContext *pContext) override;
};

#endif // #ifndef __NULLRENDERER_H_F649909F_9650_4919_972A_FA57CC1F92EA_INCLUDED__
