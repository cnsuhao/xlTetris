//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   NullRenderer.cpp
//    Author:      Streamlet
//    Create Time: 2015-04-03
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------



#include "NullRenderer.h"


void NullRenderContext::FillSolidRect(LPCRECT lpRect, const RGBQUAD &color)
{

}

void NullRenderContext::DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
{

}

void NullRenderContext::DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha)
{

}

void NullRenderContext::DrawImageGaussianBlur(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, BYTE byRadius)
{

}

bool NullRenderContext::Initialize()
{
    return true;
}

void NullRenderContext::Uninitialize()
{

}

void NullRenderContext::BeginDraw()
{

}

void NullRenderContext::EndDraw()
{

}


LPCTSTR NullRenderer::GetName()
{
    return _T("");
}

bool NullRenderer::Initialize()
{
    return true;
}

void NullRenderer::Uninitialize()
{

}

RenderContext *NullRenderer::CreateContext(HWND hWnd)
{
    return nullptr;
}

void NullRenderer::ReleaseContext(RenderContext *pContext)
{

}
