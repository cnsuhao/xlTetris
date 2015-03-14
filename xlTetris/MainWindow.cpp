//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   MainWindow.cpp
//    Author:      Streamlet
//    Create Time: 2011-03-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "MainWindow.h"
#include "Language.h"
#include <xl/String/xlString.h>
#include <ShellAPI.h>
#include "Game.h"
#include "resource.h"

enum
{
    ID_STATIC   = -1,
    ID_NULL     = 0,

    ID_BUTTEN_PAUSE,
    ID_BUTTEN_START,

    ID_LINK_WEBSITE
};

MainWindow::MainWindow() :
    m_bStarted(false),
    m_bPaused(false),
    m_nScore(0)
{
    AppendMsgHandler(WM_CREATE,      MsgHandler(this, &MainWindow::OnCreate));
    AppendMsgHandler(WM_DESTROY,     MsgHandler(this, &MainWindow::OnDestroy));
    AppendMsgHandler(WM_ERASEBKGND,  MsgHandler(this, &MainWindow::OnEraseBackground));
    AppendMsgHandler(WM_PAINT,       MsgHandler(this, &MainWindow::OnPaint));
    AppendMsgHandler(WM_ACTIVATE,    MsgHandler(this, &MainWindow::OnActivate));
    AppendMsgHandler(WM_SIZE,        MsgHandler(this, &MainWindow::OnSize));
    AppendMsgHandler(WM_KEYDOWN,     MsgHandler(this, &MainWindow::OnKeyDown));
    AppendMsgHandler(WM_LBUTTONDOWN, MsgHandler(this, &MainWindow::OnLButtonDown));

    AppendCommandMsgHandler(ID_BUTTEN_START, CommandMsgHandler(this, &MainWindow::OnButtonStart));
    AppendCommandMsgHandler(ID_BUTTEN_PAUSE, CommandMsgHandler(this, &MainWindow::OnButtonPause));

    AppendNotifyMsgHandler(ID_LINK_WEBSITE, NM_CLICK, NotifyMsgHandler(this, &MainWindow::OnLinkWebsiteClick));
}

MainWindow::~MainWindow()
{
    if (m_hScoreFont != nullptr)
    {
        DeleteObject(m_hScoreFont);
    }
}

void MainWindow::CreateControls()
{
    m_labelScoreInst.Create(ID_STATIC, this, MW_GAME_WIDTH + MW_MARGIN, MW_MARGIN * 2 + MW_PREVIEW_WIDTH, 120, 16);
    m_labelScore    .Create(ID_STATIC, this, MW_GAME_WIDTH + MW_MARGIN, MW_MARGIN * 2 + MW_PREVIEW_WIDTH + 20, 120, 36, WS_CHILD | WS_VISIBLE | SS_CENTER);

    HFONT hFont = m_labelScore.GetFont();
    LOGFONT lf = {};
    GetObject(hFont, sizeof(lf), &lf);
    lf.lfHeight  = 32;
    lf.lfWeight  = FW_BOLD;
    m_hScoreFont = CreateFontIndirect(&lf);

    m_labelScore.SetFont(m_hScoreFont);

    m_buttonPause.Create(ID_BUTTEN_PAUSE, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_MARGIN * 2 + MW_PREVIEW_WIDTH + 64, 80, 24);
    m_buttonStart.Create(ID_BUTTEN_START, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_HEIGHT - 68, 80, 24);

    m_buttonPause.EnableWindow(FALSE);

    m_linkWebSite.Create(ID_LINK_WEBSITE, this, MW_GAME_WIDTH + MW_MARGIN, MW_HEIGHT - 24, 120, 24);
}

void MainWindow::SetTexts()
{
    xl::String strText;

    strText = _Language.GetString(_T("ID_MainWindow_Caption"));
    SetWindowText(strText);

    strText = _Language.GetString(_T("ID_MainWindow_Lable_Score"));
    m_labelScoreInst.SetWindowText(strText);

    m_labelScore.SetWindowText(_T("0"));

    m_strPause = _Language.GetString(_T("ID_MainWindow_Button_Pause"));
    m_buttonPause.SetWindowText(m_strPause);
    m_strContinue = _Language.GetString(_T("ID_MainWindow_Button_Continue"));

    m_strStart = _Language.GetString(_T("ID_MainWindow_Button_Start"));
    m_buttonStart.SetWindowText(m_strStart);
    m_strStop = _Language.GetString(_T("ID_MainWindow_Button_Stop"));

    strText = _Language.GetString(_T("ID_MainWnd_Link_WebSite"));
    m_linkWebSite.SetWindowText(strText);
}

LRESULT MainWindow::OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TETRIS));
    SetIcon(hIcon);
    SetIcon(hIcon, FALSE);

    CreateControls();
    SetTexts();

    return FALSE;
}

LRESULT MainWindow::OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    PostQuitMessage(0);
    return FALSE;
}

LRESULT MainWindow::OnEraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    HDC hDC = (HDC)wParam;

    RECT rect;
    GetClientRect(&rect);

    SetDCBrushColor(hDC, GetSysColor(COLOR_3DFACE));
    HBRUSH hBrush = (HBRUSH)GetStockObject(DC_BRUSH);
    FillRect(hDC, &MW_INST_RECT, hBrush);

    hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hDC, &MW_GAME_RECT, hBrush);
    FillRect(hDC, &MW_PREVIEW_RECT, hBrush);

    return TRUE;
}

LRESULT MainWindow::OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    RECT rect;
    GetClientRect(&rect);

    HPEN hPen = (HPEN)GetStockObject(WHITE_PEN);

    PAINTSTRUCT ps = {};
    BeginPaint(&ps);

    if (ps.fErase)
    {
        SendMessage(WM_ERASEBKGND, (WPARAM)ps.hdc, 0);
    }

    _Game.Render(ps.hdc);

    EndPaint(&ps);

    return FALSE;
}

LRESULT MainWindow::OnActivate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (wParam == WA_INACTIVE && m_bStarted && !m_bPaused)
    {
        Pause();
    }

    return FALSE;
}

LRESULT MainWindow::OnSize(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (m_bStarted && !m_bPaused)
    {
        Pause();
    }

    return FALSE;
}

LRESULT MainWindow::OnKeyDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    if (m_bStarted && wParam == 'P')
    {
        if (m_bPaused)
        {
            Continue();
        }
        else
        {
            Pause();
        }

        return FALSE;
    }

    if (!m_bStarted || m_bPaused)
    {
        return FALSE;
    }

    switch (wParam)
    {
    case VK_LEFT:
        _Game.Left();
        break;
    case VK_RIGHT:
        _Game.Right();
        break;
    case VK_DOWN:
        _Game.Down();
        break;
    case 'Z':
        _Game.RotateLeft();
        break;
    case 'X':
    case VK_UP:
        _Game.RotateRight();
        break;
    default:
        break;
    }

    return FALSE;
}

LRESULT MainWindow::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    SetFocus();
    return FALSE;
}

LRESULT MainWindow::OnButtonStart(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled)
{
    if (m_bStarted)
    {
        Stop();
    }
    else
    {
        Start();
    }

    return FALSE;
}

LRESULT MainWindow::OnButtonPause(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled)
{
    if (m_bPaused)
    {
        Continue();
    }
    else
    {
        Pause();
    }

    return FALSE;
}

LRESULT MainWindow::OnLinkWebsiteClick(HWND hWnd, UINT_PTR uID, UINT uCode, HWND hContro, BOOL &bHandledl)
{
    ShellExecute(m_hWnd, _T("open"), _T("http://www.streamlet.org/"), NULL, NULL, SW_SHOW);
    return FALSE;
}

void MainWindow::Start()
{
    m_buttonStart.SetWindowText(m_strStop);

    m_buttonPause.SetWindowText(m_strPause);
    m_buttonPause.EnableWindow();

    m_nScore = 0;
    UpdateScore();

    m_bPaused = false;
    m_bStarted = true;
    SetFocus();
    _Game.Start();
}

void MainWindow::Stop()
{
    _Game.Stop();
    m_bStarted = false;

    m_buttonPause.EnableWindow(FALSE);
    m_buttonPause.SetWindowText(m_strPause);

    m_buttonStart.SetWindowText(m_strStart);
}

void MainWindow::Pause()
{
    _Game.Pause();
    m_bPaused = true;

    m_buttonPause.SetWindowText(m_strContinue);
}

void MainWindow::Continue()
{
    m_buttonPause.SetWindowText(m_strPause);

    m_bPaused = false;
    SetFocus();
    _Game.Continue();
}

void MainWindow::UpdateScore()
{
    TCHAR szScore[MAX_PATH] = {};
    _stprintf_s(szScore, _T("%d"), m_nScore);

    m_labelScore.SetWindowText(szScore);
}

void MainWindow::OnAddScore(int nScore)
{
    m_nScore += nScore;
    UpdateScore();
}

void MainWindow::OnGameOver()
{
    Stop();
}
