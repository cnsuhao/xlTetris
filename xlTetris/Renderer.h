//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Renderer.h
//    Author:      Streamlet
//    Create Time: 2015-03-14
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __RENDERER_H_C4ECC913_B674_48BB_B902_C5B426C1041B_INCLUDED__
#define __RENDERER_H_C4ECC913_B674_48BB_B902_C5B426C1041B_INCLUDED__


#include <Windows.h>
#include <tchar.h>

struct RenderMethods
{
    virtual void FillSolidRect(LPCRECT lpRect, const RGBQUAD &color) = 0;
    virtual void DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color) = 0;
};

struct RenderContext : public RenderMethods
{
    virtual bool Initialize() = 0;
    virtual void Uninitialize() = 0;
    virtual void BeginDraw() = 0;
    virtual void EndDraw() = 0;
};

struct Renderer
{
    virtual LPCTSTR GetName() = 0;
    virtual bool Initialize() = 0;
    virtual void Uninitialize() = 0;
    virtual RenderContext *CreateContext(HWND hWnd) = 0;
    virtual void ReleaseContext(RenderContext *pContext) = 0;
};

#define DEFAULT_FONT_FACE    _T("Î¢ÈíÑÅºÚ")
#define DEFAULT_FONT_SIZE   20

#endif // #ifndef __RENDERER_H_C4ECC913_B674_48BB_B902_C5B426C1041B_INCLUDED__
