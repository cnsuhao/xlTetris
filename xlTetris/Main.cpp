//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Main.cpp
//    Author:      Streamlet
//    Create Time: 2011-03-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include <Windows.h>
#include <tchar.h>
#include "Language.h"
#include "Tetris.h"

int WINAPI _tWinMain(__in HINSTANCE hInstance,
                     __in_opt HINSTANCE hPrevInstance,
                     __in LPTSTR lpCmdLine,
                     __in int nShowCmd)
{
    ::CoInitialize(NULL);

    if (!_Tetris.Initialize())
    {
        xl::String strMessage = _Language.GetString(_T("ID_App_Init_Fail"));

        if (strMessage.Empty())
        {
            strMessage = _T("Failed to initialize the application.");
        }

        MessageBox(NULL, strMessage, NULL, MB_OK | MB_ICONEXCLAMATION);

        return 0;
    }

    _Tetris.Run();

    return 0;
}


