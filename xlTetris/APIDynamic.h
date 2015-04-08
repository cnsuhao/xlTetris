//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   APIDynamic.h
//    Author:      Streamlet
//    Create Time: 2015-04-08
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __APIDYNAMIC_H_68E05CB0_2BBC_4B98_BB5D_AEF7B51C873C_INCLUDED__
#define __APIDYNAMIC_H_68E05CB0_2BBC_4B98_BB5D_AEF7B51C873C_INCLUDED__


#include <xl/Win32/xlDllHelper.h>

#define _Direct3DCreate9(...)       xl::DllHelper<IDirect3D9 *(WINAPI *)(UINT)>::CallFunction(_T("D3D9.dll"), "Direct3DCreate9", __VA_ARGS__);

#define _D2D1CreateFactory(...)     xl::DllHelper<HRESULT (WINAPI *)(D2D1_FACTORY_TYPE, REFIID, CONST D2D1_FACTORY_OPTIONS *, void **)>::CallFunction(_T("D2D1.dll"), "D2D1CreateFactory", __VA_ARGS__)
#define _DWriteCreateFactory(...)   xl::DllHelper<HRESULT (WINAPI *)(DWRITE_FACTORY_TYPE, REFIID, IUnknown **)>::CallFunction(_T("DWrite.dll"), "DWriteCreateFactory", __VA_ARGS__)

#define _D3D11CreateDevice(...)     xl::DllHelper<HRESULT (WINAPI *)(IDXGIAdapter *, D3D_DRIVER_TYPE, HMODULE, UINT, D3D_FEATURE_LEVEL *, UINT, UINT, ID3D11Device **, D3D_FEATURE_LEVEL *,ID3D11DeviceContext **)>::CallFunction(_T("D3D11.dll"), "D3D11CreateDevice", __VA_ARGS__)

#define _AlphaBlend(...)            xl::DllHelper<BOOL (WINAPI *)(HDC, int, int, int, int, HDC, int, int, int, int, BLENDFUNCTION)>::CallFunction(_T("MSImg32.dll"), "AlphaBlend", __VA_ARGS__);;


#endif // #ifndef __APIDYNAMIC_H_68E05CB0_2BBC_4B98_BB5D_AEF7B51C873C_INCLUDED__
