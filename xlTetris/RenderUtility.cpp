//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   RenderUtility.cpp
//    Author:      Streamlet
//    Create Time: 2015-04-12
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------


#include "RenderUtility.h"
#define _USE_MATH_DEFINES
#include <math.h>


HBITMAP RenderUtility::CreateBitmap(int cx, int cy, LPVOID *ppvBits)
{
    BITMAPINFO bmp = {};
    bmp.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmp.bmiHeader.biWidth = cx;
    bmp.bmiHeader.biHeight = -cy;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biBitCount = 32;
    bmp.bmiHeader.biCompression = BI_RGB;

    HWND hWnd = GetDesktopWindow();
    HDC hDC = GetDC(hWnd);
    HBITMAP hBitmap = CreateDIBSection(hDC, &bmp, DIB_RGB_COLORS, ppvBits, nullptr, 0);
    ReleaseDC(hWnd, hDC);

    return hBitmap;
}

HBITMAP RenderUtility::GaussianBlur(HBITMAP hBitmap, BYTE byRadius)
{
    double *pTemptlate = new double[byRadius + 1];
    // G(x) = 1/(sqrt(2*pi)*sigma) * e ^ (-x^2/(2*sigma^2))
    double dSigma = (double)byRadius / 3.0;
    double a = 1 / sqrt(2 * M_PI) / dSigma;
    double b = -0.5 / dSigma / dSigma;
    double sum = 0;
    for (int i = 0; i < byRadius + 1; ++i)
    {
        pTemptlate[i] = (float)(a * pow(M_E, i * i * b));
        sum += i == 0 ? pTemptlate[i] : pTemptlate[i] * 2;
    }
    for (int i = 0; i < byRadius + 1; ++i)
    {
        pTemptlate[i] /= sum;
        if (pTemptlate[i] <= 0)
        {
            byRadius = (BYTE)i;
            break;
        }
    }

    BITMAP bm = {};
    GetObject(hBitmap, sizeof(bm), &bm);

    RGBQUAD *p1 = (RGBQUAD *)bm.bmBits;
    RGBQUAD *p2 = new RGBQUAD[bm.bmWidth * bm.bmHeight];
    RGBQUAD *p3 = nullptr;
    HBITMAP hNew = CreateBitmap(bm.bmWidth, bm.bmHeight, (LPVOID *)&p3);

    for (int j = 0; j < bm.bmHeight; ++j)
    {
        for (int i = 0; i < bm.bmWidth; ++i)
        {
            WORD a = 0, r = 0, g = 0, b = 0;
            a = (WORD)((double)p1[j * bm.bmWidth + i].rgbReserved * pTemptlate[0]);
            r = (WORD)((double)p1[j * bm.bmWidth + i].rgbRed * pTemptlate[0]);
            g = (WORD)((double)p1[j * bm.bmWidth + i].rgbGreen * pTemptlate[0]);
            b = (WORD)((double)p1[j * bm.bmWidth + i].rgbBlue * pTemptlate[0]);

            for (int k = 1; k < byRadius + 1; ++k)
            {
                int i1 = i - k >= 0 ? i - k : 0;
                int i2 = i + k < bm.bmWidth ? i + k : bm.bmWidth - 1;
                a += (WORD)(((double)p1[j * bm.bmWidth + i1].rgbReserved + (double)p1[j * bm.bmWidth + i2].rgbReserved)* pTemptlate[k]);
                r += (WORD)(((double)p1[j * bm.bmWidth + i1].rgbRed + (double)p1[j * bm.bmWidth + i2].rgbRed)* pTemptlate[k]);
                g += (WORD)(((double)p1[j * bm.bmWidth + i1].rgbGreen + (double)p1[j * bm.bmWidth + i2].rgbGreen)* pTemptlate[k]);
                b += (WORD)(((double)p1[j * bm.bmWidth + i1].rgbBlue + (double)p1[j * bm.bmWidth + i2].rgbBlue)* pTemptlate[k]);
            }

            p2[j * bm.bmWidth + i].rgbReserved = (BYTE)min(a, 255);
            p2[j * bm.bmWidth + i].rgbRed = (BYTE)min(r, 255);
            p2[j * bm.bmWidth + i].rgbGreen = (BYTE)min(g, 255);
            p2[j * bm.bmWidth + i].rgbBlue = (BYTE)min(b, 255);
        }
    }


    for (int j = 0; j < bm.bmHeight; ++j)
    {
        for (int i = 0; i < bm.bmWidth; ++i)
        {
            WORD a = 0, r = 0, g = 0, b = 0;
            a = (WORD)((double)p2[j * bm.bmWidth + i].rgbReserved * pTemptlate[0]);
            r = (WORD)((double)p2[j * bm.bmWidth + i].rgbRed * pTemptlate[0]);
            g = (WORD)((double)p2[j * bm.bmWidth + i].rgbGreen * pTemptlate[0]);
            b = (WORD)((double)p2[j * bm.bmWidth + i].rgbBlue * pTemptlate[0]);

            for (int k = 1; k < byRadius + 1; ++k)
            {
                int j1 = j - k >= 0 ? j - k : 0;
                int j2 = j + k < bm.bmHeight ? j + k : bm.bmHeight - 1;
                a += (WORD)(((double)p2[j1 * bm.bmWidth + i].rgbReserved + (double)p2[j2 * bm.bmWidth + i].rgbReserved)* pTemptlate[k]);
                r += (WORD)(((double)p2[j1 * bm.bmWidth + i].rgbRed + (double)p2[j2 * bm.bmWidth + i].rgbRed)* pTemptlate[k]);
                g += (WORD)(((double)p2[j1 * bm.bmWidth + i].rgbGreen + (double)p2[j2 * bm.bmWidth + i].rgbGreen)* pTemptlate[k]);
                b += (WORD)(((double)p2[j1 * bm.bmWidth + i].rgbBlue + (double)p2[j2 * bm.bmWidth + i].rgbBlue)* pTemptlate[k]);
            }

            p3[j * bm.bmWidth + i].rgbReserved = (BYTE)min(a, 255);
            p3[j * bm.bmWidth + i].rgbRed = (BYTE)min(r, 255);
            p3[j * bm.bmWidth + i].rgbGreen = (BYTE)min(g, 255);
            p3[j * bm.bmWidth + i].rgbBlue = (BYTE)min(b, 255);
        }
    }

    delete[] p2;
    delete[] pTemptlate;

    return hNew;
}
