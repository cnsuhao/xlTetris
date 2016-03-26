//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Tetris.cpp
//    Author:      Streamlet
//    Create Time: 2011-03-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "Tetris.h"
#include "Language.h"
#include <tchar.h>
#include "MainWindow.h"
#include "Game.h"
#include "resource.h"


Tetris::Tetris() :
    m_hWaitableTimer(nullptr)
{

}

Tetris::~Tetris()
{
    Release();
}

bool Tetris::Initialize()
{
    if (!_Language.Initialize(_T("languages.xml")))
    {
        if (!_Language.Initialize(_T("XML"), IDR_XML_LANGUAGE))
        {
            return false;
        }
    }

    _Language.SetCurrentLanguage(GetUserDefaultUILanguage());

    m_hWaitableTimer = CreateWaitableTimer(NULL, FALSE, NULL);

    if (m_hWaitableTimer == NULL)
    {
        return false;
    }

    const int nWidth = MW_WIDTH + GetSystemMetrics(SM_CXFRAME) * 2;
    const int nHeight = MW_HEIGHT + GetSystemMetrics(SM_CYFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION);
    const int nX = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2;
    const int nY = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;

    _MainWindow.Create(nullptr, nX, nY, nWidth, nHeight, WS_OVERLAPPEDWINDOW | WS_VISIBLE);

    if (!_Game.Initialize(_MainWindow,
                          &MW_GAME_RECT,
                          &MW_PREVIEW_RECT,
                          FN_AddScore(&_MainWindow, &MainWindow::OnAddScore),
                          FN_GameOver(&_MainWindow, &MainWindow::OnGameOver)))
    {
        _MainWindow.Destroy();
        return false;
    }

    return true;
}

void Tetris::Release()
{
    if (m_hWaitableTimer != nullptr)
    {
        CloseHandle(m_hWaitableTimer);
        m_hWaitableTimer = nullptr;
    }
}

void Tetris::Run()
{
    ShowWindow(_MainWindow, SW_SHOW);
    
    LARGE_INTEGER li = {};
    li.QuadPart = -1i64;
    SetWaitableTimer(m_hWaitableTimer, &li, 10, NULL, NULL, 0);

    MSG msg = {};
    bool bRun = true;

    while(bRun)
    {
        DWORD dwWaitResult = MsgWaitForMultipleObjects(1, &m_hWaitableTimer, FALSE, INFINITE, QS_ALLINPUT);

        switch(dwWaitResult)
        {
        case WAIT_OBJECT_0:
            _Game.Gaming();
            break;
        default:
            while (PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
            {
                if (!GetMessage(&msg, NULL, NULL, NULL))
                {
                    bRun = false;
                    break;
                }

                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            break;
        }
    }
}
