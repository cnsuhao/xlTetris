//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   D2DDynamic.h
//    Author:      Streamlet
//    Create Time: 2015-03-15
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __D2DDYNAMIC_H_302A1794_EB58_4C06_BB03_63392C226FD0_INCLUDED__
#define __D2DDYNAMIC_H_302A1794_EB58_4C06_BB03_63392C226FD0_INCLUDED__


#include <xl/Win32/xlDllHelper.h>


#define _D2D1CreateFactory(...)   xl::DllHelper<HRESULT (WINAPI *)(D2D1_FACTORY_TYPE, REFIID, CONST D2D1_FACTORY_OPTIONS *, void **)>::CallFunction(_T("D2d1.dll"), "D2D1CreateFactory", __VA_ARGS__)
#define _DWriteCreateFactory(...) xl::DllHelper<HRESULT (WINAPI *)(DWRITE_FACTORY_TYPE, REFIID, IUnknown **)>::CallFunction(_T("DWrite.dll"), "DWriteCreateFactory", __VA_ARGS__)

#endif // #ifndef __D2DDYNAMIC_H_302A1794_EB58_4C06_BB03_63392C226FD0_INCLUDED__
