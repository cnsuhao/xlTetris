//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   RenderUtility.h
//    Author:      Streamlet
//    Create Time: 2015-04-12
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __RENDERUTILITY_H_6DBF0A66_78CE_4ECB_BCF1_07D1CC2C246A_INCLUDED__
#define __RENDERUTILITY_H_6DBF0A66_78CE_4ECB_BCF1_07D1CC2C246A_INCLUDED__


#include <Windows.h>

namespace RenderUtility
{
    HBITMAP CreateBitmap(int cx, int cy, LPVOID *ppvBits);
    void FixAlpha(HBITMAP hBitmap);
    HBITMAP GaussianBlur(HBITMAP hBitmap, BYTE byRadius);
}

#endif // #ifndef __RENDERUTILITY_H_6DBF0A66_78CE_4ECB_BCF1_07D1CC2C246A_INCLUDED__
