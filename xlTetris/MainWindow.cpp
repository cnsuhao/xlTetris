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
#include "NullRenderer.h"
#include "GDIRenderer.h"
#include "D3D9Renderer.h"
#include "D2D10Renderer.h"
#include "D2D11Renderer.h"


Renderer *g_pRenderers[] =
{
    new GDIRenderer,
    new D2D11Renderer,
    new D2D10Renderer,
    new D3D9Renderer,
};

enum
{
    ID_STATIC   = -1,
    ID_NULL     = 0,

    ID_BUTTON_PAUSE,
    ID_BUTTON_START,

    ID_COMBOBOX_RENDERER,

    ID_LINK_WEBSITE
};

MainWindow::MainWindow() :
    m_iRenderer(-1),
    m_pRC(nullptr),
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

    AppendCommandMsgHandler(ID_BUTTON_START, CommandMsgHandler(this, &MainWindow::OnButtonStart));
    AppendCommandMsgHandler(ID_BUTTON_PAUSE, CommandMsgHandler(this, &MainWindow::OnButtonPause));
    AppendCommandMsgHandler(ID_COMBOBOX_RENDERER, CBN_SELCHANGE, CommandMsgHandler(this, &MainWindow::OnComboBoxRendererChange));

    AppendNotifyMsgHandler(ID_LINK_WEBSITE, NM_CLICK, NotifyMsgHandler(this, &MainWindow::OnLinkWebsiteClick));
}

MainWindow::~MainWindow()
{
    if (m_hBackground != nullptr)
    {
        DeleteObject(m_hBackground);
    }

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

    m_buttonPause.Create(ID_BUTTON_PAUSE, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_MARGIN * 2 + MW_PREVIEW_WIDTH + 64, 80, 24);
    m_buttonStart.Create(ID_BUTTON_START, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_HEIGHT - 88, 80, 24);

    m_buttonPause.EnableWindow(FALSE);

    m_comboRenderer.Create(ID_COMBOBOX_RENDERER, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_HEIGHT - 50, 80, 24);

    for (int i = 0; i < _countof(g_pRenderers); ++i)
    {
        m_comboRenderer.AddString(g_pRenderers[i]->GetName());
    }

    m_comboRenderer.SetCurSel(-1);

    m_linkWebSite.Create(ID_LINK_WEBSITE, this, MW_GAME_WIDTH + MW_MARGIN, MW_HEIGHT - 24, 120, 24);

    m_hBackground = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BACKGROUND));
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

void MainWindow::AutoSelectRenderer()
{
    for (int i = 0; i < _countof(g_pRenderers); ++i)
    {
        if (g_pRenderers[i]->Initialize())
        {
            m_iRenderer = i;
            break;
        }
    }

    if (m_iRenderer >= 0)
    {
        m_pRC = g_pRenderers[m_iRenderer]->CreateContext(m_hWnd);
    }

    if (m_pRC == nullptr)
    {
        m_iRenderer = -1;
        m_pRC = new NullRenderContext;
    }

    m_comboRenderer.SetCurSel(m_iRenderer);
    InvalidateRect(NULL);
}

void MainWindow::ReleaseRenderer()
{
    if (m_iRenderer >= 0)
    {
        g_pRenderers[m_iRenderer]->ReleaseContext(m_pRC);
        g_pRenderers[m_iRenderer]->Uninitialize();
    }
    else
    {
        delete m_pRC;
    }

    m_iRenderer = -1;
    m_pRC = nullptr;
}

LRESULT MainWindow::OnCreate(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ModifyStyle(0, WS_CLIPCHILDREN);

    HICON hIcon = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_TETRIS));
    SetIcon(hIcon);
    SetIcon(hIcon, FALSE);

    CreateControls();
    SetTexts();

    AutoSelectRenderer();

    return FALSE;
}

LRESULT MainWindow::OnDestroy(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    ReleaseRenderer();

    PostQuitMessage(0);
    return FALSE;
}

LRESULT MainWindow::OnEraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    return TRUE;
}

LRESULT MainWindow::OnPaint(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    m_pRC->BeginDraw();

    COLORREF clr = GetSysColor(COLOR_3DFACE);
    RGBQUAD color = { GetBValue(clr), GetGValue(clr), GetRValue(clr), 0xff };
    m_pRC->FillSolidRect(&MW_INST_RECT, color);


    _Game.Render(m_pRC);
    m_pRC->DrawImage(m_hBackground, &MW_WINDOW_RECT, &MW_WINDOW_RECT, 0x80);

    xl::String strRenderer = g_pRenderers[m_iRenderer]->GetName();
    RGBQUAD colorRender = { 0x00, 0x80, 0x80, 0x80 };
    m_pRC->DrawText(strRenderer, strRenderer.Length(), &MW_GAME_RECT, DT_SINGLELINE, colorRender);

    m_pRC->EndDraw();

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

LRESULT MainWindow::OnComboBoxRendererChange(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled)
{
    ReleaseRenderer();

    m_iRenderer = m_comboRenderer.GetCurSel();

    if (g_pRenderers[m_iRenderer]->Initialize())
    {
        m_pRC = g_pRenderers[m_iRenderer]->CreateContext(m_hWnd);
    }

    if (m_pRC == nullptr)
    {
        AutoSelectRenderer();
    }
    else
    {
        InvalidateRect(NULL);
    }

    return FALSE;
}

LRESULT MainWindow::OnLinkWebsiteClick(HWND hWnd, UINT_PTR uID, UINT uCode, HWND hContro, BOOL &bHandled)
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
