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

    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

    Vertex()
        : x(0), y(0), z(0), rhw(1), color(0) { }
    Vertex(float x, float y)
        : x(x), y(y), z(0), rhw(1), color(0) { }
    Vertex(float x, float y, D3DCOLOR color)
        : x(x), y(y), z(0), rhw(1), color(color) { }
};

D3D9RenderContext::D3D9RenderContext(HWND hWnd, D3D9Renderer *pRenderer) :
    m_hWnd(hWnd), m_pRenderer(pRenderer), m_pD3DDevice(nullptr), m_pVertex(nullptr), m_pIndex(nullptr), m_pFont(nullptr)
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
