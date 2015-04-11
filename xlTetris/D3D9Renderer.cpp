//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D3D9Renderer.cpp
//    Author:      Streamlet
//    Create Time: 2015-03-23
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------



#include "D3D9Renderer.h"
#include "APIDynamic.h"
#include "resource.h"
#define _USE_MATH_DEFINES
#include <math.h>
#pragma comment(lib, "d3dx9.lib")


#define SAFE_RELEASE_COM_PTR(p) \
    do                          \
    {                           \
        if (p != nullptr)       \
        {                       \
            p->Release();       \
            p = nullptr;        \
        }                       \
    } while (false)

struct Vertex
{
    float x, y, z, rhw;
    DWORD color;
    float u, v;

    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

    Vertex()
        : x(0), y(0), z(0), rhw(1), color(0), u(0), v(0) { }
    Vertex(float x, float y)
        : x(x), y(y), z(0), rhw(1), color(0), u(0), v(0) { }
    Vertex(float x, float y, D3DCOLOR color)
        : x(x), y(y), z(0), rhw(1), color(color), u(0), v(0) { }
    Vertex(float x, float y, D3DCOLOR color, float u, float v)
        : x(x), y(y), z(0), rhw(1), color(color), u(u), v(v) { }
};

D3D9RenderContext::D3D9RenderContext(HWND hWnd, D3D9Renderer *pRenderer) :
    m_hWnd(hWnd), m_pRenderer(pRenderer), m_pD3DDevice(nullptr), m_pVertex(nullptr), m_pIndex(nullptr), m_pPSGaussianBlur(nullptr), m_pPSGaussianBlurConst(nullptr), m_pFont(nullptr)
{
    ZeroMemory(&m_Params, sizeof(m_Params));
}

D3D9RenderContext::~D3D9RenderContext()
{
    Uninitialize();
}

void D3D9RenderContext::FillSolidRect(LPCRECT lpRect, const RGBQUAD &color)
{
    D3DCOLOR clr = D3DCOLOR_ARGB(color.rgbReserved, color.rgbRed, color.rgbGreen, color.rgbBlue);

    /*
      0 +----------+------> x
        |          | 1
        |          |
        |          |
      3 +----------+ 2
        |
        V y
    */

    Vertex *vertices = NULL;
    m_pVertex->Lock(0, 0, (LPVOID *)&vertices, D3DLOCK_DISCARD);

    vertices[0] = Vertex((float)lpRect->left,  (float)lpRect->top,    clr);
    vertices[1] = Vertex((float)lpRect->right, (float)lpRect->top,    clr);
    vertices[2] = Vertex((float)lpRect->right, (float)lpRect->bottom, clr);
    vertices[3] = Vertex((float)lpRect->left,  (float)lpRect->bottom, clr);

    m_pVertex->Unlock();

    m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

void D3D9RenderContext::DrawText(LPCTSTR lpszext, int cchText, LPCRECT lpRect, UINT uFormat, const RGBQUAD &color)
{
    D3DCOLOR clr = D3DCOLOR_ARGB(color.rgbReserved, color.rgbRed, color.rgbGreen, color.rgbBlue);
    m_pFont->DrawText(NULL, lpszext, cchText, (LPRECT)lpRect, uFormat, clr);
}

void D3D9RenderContext::DrawImage(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha)
{
    SIZE sz = {};
    IDirect3DTexture9 *pTexture = BitmapToTexture(hBitmap, &sz);
    DrawImage(pTexture, sz, lprcDest, lprcSource, byAlpha, nullptr, nullptr);
    pTexture->Release();
}

void D3D9RenderContext::DrawImageGaussianBlur(HBITMAP hBitmap, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, BYTE byRadius)
{
    SIZE sz = {};
    IDirect3DTexture9 *pTexture = BitmapToTexture(hBitmap, &sz);

    D3DXHANDLE hTexSize = m_pPSGaussianBlurConst->GetConstantByName(nullptr, "TexSize");
    float fTexSize[] = { (float)sz.cx, (float)sz.cy };
    m_pPSGaussianBlurConst->SetFloatArray(m_pD3DDevice, hTexSize, fTexSize, _countof(fTexSize));

    float *pTemptlate = new float[byRadius * 3 + 2];
    // G(x) = 1/(sqrt(2*pi)*sigma) * e ^ (-x^2/(2*sigma^2))
    double dSigma = (double)byRadius;
    double a = 1 / sqrt(2 * M_PI) / dSigma;
    double b = -0.5 / dSigma / dSigma;
    for (int i = 0; i < byRadius * 3 + 1; ++i)
    {
        pTemptlate[i] = (float)(a * pow(M_E, i * i * b));
    }
    pTemptlate[byRadius * 3 + 1] = 0;

    D3DXHANDLE hTemplate = m_pPSGaussianBlurConst->GetConstantByName(nullptr, "Template");
    m_pPSGaussianBlurConst->SetFloatArray(m_pD3DDevice, hTemplate, pTemptlate, byRadius * 3 + 2);

    D3DXHANDLE hPass = m_pPSGaussianBlurConst->GetConstantByName(nullptr, "ScanPass");
    m_pPSGaussianBlurConst->SetInt(m_pD3DDevice, hPass, 0);

    IDirect3DSurface9 *pBackSurface = nullptr;
    m_pD3DDevice->GetRenderTarget(0, &pBackSurface);

    IDirect3DTexture9 *pTexturePass0 = nullptr;
    m_pD3DDevice->CreateTexture(sz.cx, sz.cy, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexturePass0, nullptr);
    IDirect3DSurface9 *pSurfacePass0 = nullptr;
    pTexturePass0->GetSurfaceLevel(0, &pSurfacePass0);
    m_pD3DDevice->SetRenderTarget(0, pSurfacePass0);
    DrawImage(pTexture, sz, lprcDest, lprcSource, 255, nullptr, m_pPSGaussianBlur);

    pTexture->Release();

    IDirect3DTexture9 *pTexturePass1 = nullptr;
    m_pD3DDevice->CreateTexture(sz.cx, sz.cy, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexturePass1, nullptr);
    IDirect3DSurface9 *pSurfacePass1 = nullptr;
    pTexturePass1->GetSurfaceLevel(0, &pSurfacePass1);
    m_pD3DDevice->SetRenderTarget(0, pSurfacePass1);

    pSurfacePass0->Release();

    m_pPSGaussianBlurConst->SetInt(m_pD3DDevice, hPass, 1);

    DrawImage(pTexturePass0, sz, lprcDest, lprcSource, 255, nullptr, m_pPSGaussianBlur);

    pTexturePass0->Release();

    m_pD3DDevice->SetRenderTarget(0, pBackSurface);
    pSurfacePass1->Release();
    pBackSurface->Release();

    DrawImage(pTexturePass1, sz, lprcDest, lprcSource, byAlpha, nullptr, nullptr);

    pTexturePass1->Release();
}

bool D3D9RenderContext::Initialize()
{
    m_Params.BackBufferWidth = 0;  
    m_Params.BackBufferHeight = 0;
    m_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
    m_Params.BackBufferCount = 1;
    m_Params.MultiSampleType = D3DMULTISAMPLE_NONE;
    m_Params.MultiSampleQuality = 0;
    m_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
    m_Params.hDeviceWindow = m_hWnd;
    m_Params.Windowed = TRUE;
    m_Params.EnableAutoDepthStencil = TRUE;
    m_Params.AutoDepthStencilFormat = D3DFMT_D24S8;
    m_Params.Flags = 0;
    m_Params.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    m_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    for (int i = D3DMULTISAMPLE_16_SAMPLES; i > D3DMULTISAMPLE_NONE; --i)
    {
        DWORD dwQualityLevel = 0;
        HRESULT hr = m_pRenderer->m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_A8R8G8B8, TRUE, (D3DMULTISAMPLE_TYPE)i, &dwQualityLevel);

        if (FAILED(hr))
        {
            continue;
        }

        DWORD dwQualityLevelDepth = 0;
        hr = m_pRenderer->m_pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_D24S8, TRUE, (D3DMULTISAMPLE_TYPE)i, &dwQualityLevelDepth);

        if (FAILED(hr))
        {
            continue;
        }

        dwQualityLevel = min(dwQualityLevel, dwQualityLevelDepth);

        if (dwQualityLevel > 1)
        {
            m_Params.MultiSampleType = (D3DMULTISAMPLE_TYPE)i;
            m_Params.MultiSampleQuality = dwQualityLevel - 1;

            break;
        }
    }

    HRESULT hr = m_pRenderer->m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &m_Params, &m_pD3DDevice);

    if (FAILED(hr) || m_pD3DDevice == nullptr)
    {
        return false;
    }

    hr = m_pD3DDevice->CreateVertexBuffer(4 * sizeof(Vertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, Vertex::FVF, D3DPOOL_DEFAULT, &m_pVertex, NULL);

    if (FAILED(hr) || m_pVertex == nullptr)
    {
        return false;
    }

    hr = m_pD3DDevice->CreateIndexBuffer(6 * sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIndex, NULL);

    if (FAILED(hr) || m_pIndex == nullptr)
    {
        return false;
    }

    WORD *indices = NULL;
    m_pIndex->Lock(0, 0, (LPVOID *)&indices, 0);
    // 0, 1, 2
    indices[0] = 0, indices[1] = 1, indices[2] = 2;
    // 0, 2, 3
    indices[3] = 0, indices[4] = 2, indices[5] = 3;
    m_pIndex->Unlock();

    LPVOID pData = NULL;
    DWORD cbSize = GetResource(&pData, _T("HLSL"), IDR_HLSL_PS);

    hr = m_pD3DDevice->CreatePixelShader((DWORD *)pData, &m_pPSGaussianBlur);

    if (FAILED(hr) || m_pPSGaussianBlur == nullptr)
    {
        return false;
    }

    UINT uSize = 0;
    hr = m_pPSGaussianBlur->GetFunction(nullptr, &uSize);

    if (FAILED(hr) || uSize == 0)
    {
        return false;
    }

    BYTE *pFunction = new BYTE[uSize];
    hr = m_pPSGaussianBlur->GetFunction(pFunction, &uSize);

    if (FAILED(hr) || pFunction == nullptr)
    {
        return false;
    }

    hr = D3DXGetShaderConstantTable((DWORD *)pFunction, &m_pPSGaussianBlurConst);
    delete[] pFunction;

    if (FAILED(hr) || m_pPSGaussianBlurConst == nullptr)
    {
        return false;
    }

    hr = D3DXCreateFont(m_pD3DDevice, -DEFAULT_FONT_SIZE, 0, FW_NORMAL, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE, DEFAULT_FONT_FACE, &m_pFont);

    if (FAILED(hr) || m_pFont == nullptr)
    {
        return false;
    }

    return true;
}

void D3D9RenderContext::Uninitialize()
{
    SAFE_RELEASE_COM_PTR(m_pFont);
    SAFE_RELEASE_COM_PTR(m_pPSGaussianBlurConst);
    SAFE_RELEASE_COM_PTR(m_pPSGaussianBlur);
    SAFE_RELEASE_COM_PTR(m_pD3DDevice);
}

void D3D9RenderContext::BeginDraw()
{
    RECT rc = {};
    GetClientRect(m_hWnd, &rc);

    if (m_Params.BackBufferWidth != rc.right - rc.left || m_Params.BackBufferHeight != rc.bottom - rc.top)
    {
        m_Params.BackBufferWidth = rc.right - rc.left;
        m_Params.BackBufferHeight = rc.bottom - rc.top;

        m_pD3DDevice->Reset(&m_Params);
    }

    m_pD3DDevice->BeginScene();
    m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

    m_pD3DDevice->SetFVF(Vertex::FVF);
    m_pD3DDevice->SetStreamSource(0, m_pVertex, 0, sizeof(Vertex));
    m_pD3DDevice->SetIndices(m_pIndex);
    m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}

void D3D9RenderContext::EndDraw()
{
    m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    m_pD3DDevice->EndScene();
    m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
    ValidateRect(m_hWnd, NULL);
}

IDirect3DTexture9 *D3D9RenderContext::BitmapToTexture(HBITMAP hBitmap, SIZE *pSize)
{
    BITMAP bm = {};
    GetObject(hBitmap, sizeof(bm), &bm);

    BITMAPINFO bmp = {};
    bmp.bmiHeader.biSize = sizeof(BITMAPINFO);
    bmp.bmiHeader.biWidth = bm.bmWidth;
    bmp.bmiHeader.biHeight = -bm.bmHeight;
    bmp.bmiHeader.biPlanes = 1;
    bmp.bmiHeader.biBitCount = 32;
    bmp.bmiHeader.biCompression = BI_RGB;

    DWORD *lpBuffer = new DWORD[bm.bmWidth * bm.bmHeight];
    HDC hDC = GetDC(m_hWnd);
    int iLines = GetDIBits(hDC, hBitmap, 0, bm.bmHeight, lpBuffer, &bmp, DIB_RGB_COLORS);
    ReleaseDC(m_hWnd, hDC);

    if (iLines == 0)
    {
        delete[] lpBuffer;
        return nullptr;
    }

    IDirect3DTexture9 *pTexture = nullptr;
    HRESULT hr = m_pD3DDevice->CreateTexture(bm.bmWidth, bm.bmHeight, 0, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &pTexture, nullptr);

    if (FAILED(hr) || pTexture == nullptr)
    {
        delete[] lpBuffer;
        return nullptr;
    }

    D3DLOCKED_RECT lr = {};
    RECT rc = { 0, 0, bm.bmWidth, bm.bmHeight };
    hr = pTexture->LockRect(0, &lr, &rc, D3DLOCK_DISCARD);

    if (FAILED(hr))
    {
        delete[] lpBuffer;
        pTexture->Release();
        return nullptr;
    }

    for (int j = 0; j < bm.bmHeight; ++j)
    {
        DWORD *pLine = (DWORD *)((BYTE *)lr.pBits + j * lr.Pitch);

        for (int i = 0; i < bm.bmWidth; ++i)
        {
            pLine[i] = lpBuffer[j * bm.bmWidth + i] | 0xff000000;
        }
    }

    pTexture->UnlockRect(0);

    delete[] lpBuffer;

    if (pSize != nullptr)
    {
        pSize->cx = bm.bmWidth;
        pSize->cy = bm.bmHeight;
    }

    return pTexture;
}

void D3D9RenderContext::DrawImage(IDirect3DTexture9 *pTexture, SIZE sz, LPCRECT lprcDest, LPCRECT lprcSource, BYTE byAlpha, IDirect3DVertexShader9 *pVS, IDirect3DPixelShader9 *pPS)
{
    /*
      0 +----------+------> x
        |          | 1
        |          |
        |          |
      3 +----------+ 2
        |
        V y

        +---> u
        |
        V v

    */

    float left = (float)lprcSource->left / sz.cx;
    float top = (float)lprcSource->top / sz.cy;
    float right = (float)lprcSource->right / sz.cx;
    float bottom = (float)lprcSource->bottom / sz.cy;

    D3DCOLOR clr = D3DCOLOR_ARGB(byAlpha, 0, 0, 0);

    Vertex *vertices = NULL;
    m_pVertex->Lock(0, 0, (LPVOID *)&vertices, D3DLOCK_DISCARD);

    vertices[0] = Vertex((float)lprcDest->left,      (float)lprcDest->top,        clr, left,  top);
    vertices[1] = Vertex((float)lprcDest->right - 1, (float)lprcDest->top,        clr, right, top);
    vertices[2] = Vertex((float)lprcDest->right - 1, (float)lprcDest->bottom - 1, clr, right, bottom);
    vertices[3] = Vertex((float)lprcDest->left,      (float)lprcDest->bottom - 1, clr, left,  bottom);

    m_pVertex->Unlock();

    m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
    m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pD3DDevice->SetTexture(0, pTexture);
    m_pD3DDevice->SetVertexShader(pVS);
    m_pD3DDevice->SetPixelShader(pPS);
    m_pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
    m_pD3DDevice->SetPixelShader(nullptr);
    m_pD3DDevice->SetVertexShader(nullptr);
    m_pD3DDevice->SetTexture(0, nullptr);
}

DWORD D3D9RenderContext::GetResource(LPVOID *pBuffer, LPCTSTR lpszResType, UINT nResID)
{
    HRSRC hRes = FindResource(GetModuleHandle(NULL), MAKEINTRESOURCE(nResID), lpszResType);

    if (hRes == nullptr)
    {
        return 0;
    }

    DWORD cbSize = SizeofResource(GetModuleHandle(nullptr), hRes);
    HGLOBAL hResData = LoadResource(GetModuleHandle(nullptr), hRes);

    if (hResData == nullptr)
    {
        return 0;
    }

    *pBuffer = LockResource(hResData);

    return cbSize;
}


D3D9Renderer::D3D9Renderer() : m_pD3D(nullptr)
{

}

D3D9Renderer::~D3D9Renderer()
{
    Uninitialize();
}

LPCTSTR D3D9Renderer::GetName()
{
    return _T("D3D9");
}

bool D3D9Renderer::Initialize()
{
    m_pD3D = _Direct3DCreate9(D3D9b_SDK_VERSION);

    if (m_pD3D == nullptr)
    {
        return false;
    }

    D3DCAPS9 caps = {};
    HRESULT hr = m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

void D3D9Renderer::Uninitialize()
{
    SAFE_RELEASE_COM_PTR(m_pD3D);
}

RenderContext *D3D9Renderer::CreateContext(HWND hWnd)
{
    D3D9RenderContext *pRC = new D3D9RenderContext(hWnd, this);

    if (!pRC->Initialize())
    {
        pRC->Uninitialize();
        delete pRC;
        return nullptr;
    }

    return pRC;
}

void D3D9Renderer::ReleaseContext(RenderContext *pContext)
{
    delete pContext;
}
