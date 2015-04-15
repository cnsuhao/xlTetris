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
#include "RenderUtility.h"


Renderer *g_pRenderers[] =
{
    new D3D9Renderer,
    new GDIRenderer,
    new D2D11Renderer,
    new D2D10Renderer,
};

enum
{
    ID_STATIC   = -1,
    ID_NULL     = 0,

    ID_BUTTON_PAUSE,
    ID_BUTTON_START,
    ID_BUTTON_CHANGEIMAGE,

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
    AppendCommandMsgHandler(ID_BUTTON_CHANGEIMAGE, CommandMsgHandler(this, &MainWindow::OnButtonChangeImage));
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
    m_buttonStart.Create(ID_BUTTON_START, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_MARGIN * 2 + MW_PREVIEW_WIDTH + 100, 80, 24);

    m_buttonPause.EnableWindow(FALSE);

    m_buttonChangeImage.Create(ID_BUTTON_CHANGEIMAGE, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_HEIGHT - 88, 80, 24);
    m_comboRenderer.Create(ID_COMBOBOX_RENDERER, this, MW_GAME_WIDTH + MW_MARGIN + 20, MW_HEIGHT - 60, 80, 24);

    for (int i = 0; i < _countof(g_pRenderers); ++i)
    {
        m_comboRenderer.AddString(g_pRenderers[i]->GetName());
    }

    m_comboRenderer.SetCurSel(-1);

    m_linkWebSite.Create(ID_LINK_WEBSITE, this, MW_GAME_WIDTH + MW_MARGIN, MW_HEIGHT - 24, 120, 24);

    m_hBackground = (HBITMAP)LoadImage(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDB_BACKGROUND), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

    BITMAP bm = {};
    GetObject(m_hBackground, sizeof(bm), &bm);
    m_szBackground.cx = bm.bmWidth;
    m_szBackground.cy = bm.bmHeight;
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

    strText = _Language.GetString(_T("ID_MainWindow_Button_ChangeImage"));
    m_buttonChangeImage.SetWindowText(strText);
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
    ModifyStyle(0, WS_CLIPCHILDREN | WS_MAXIMIZEBOX | WS_THICKFRAME);

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

    RECT rcClient = {};
    GetClientRect(&rcClient);
    RGBQUAD colorBack = { 0, 0, 0, 0 };
    m_pRC->FillSolidRect(&rcClient, colorBack);

    COLORREF clrWindow = GetSysColor(COLOR_3DFACE);
    RGBQUAD colorWindow = { GetBValue(clrWindow), GetGValue(clrWindow), GetRValue(clrWindow), 0xff };
    m_pRC->FillSolidRect(&MW_INST_RECT, colorWindow);

    _Game.Render(m_pRC);

    RECT rcImage = { 0, 0, min(rcClient.right - rcClient.left, m_szBackground.cx), min(rcClient.bottom - rcClient.top, m_szBackground.cy) };
    m_pRC->DrawImageGaussianBlur(m_hBackground, &rcImage, &rcImage, 0x80, 48);

    if (m_iRenderer >= 0)
    {
        xl::String strRenderer = g_pRenderers[m_iRenderer]->GetName();
        RGBQUAD colorRender = { 0x00, 0x80, 0x80, 0x80 };
        m_pRC->DrawText(strRenderer, strRenderer.Length(), &MW_GAME_RECT, DT_SINGLELINE, colorRender);
    }

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

    InvalidateRect(NULL);

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

LRESULT MainWindow::OnButtonChangeImage(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled)
{
    TCHAR szPath[MAX_PATH] = {};

    OPENFILENAME ofn = { sizeof(OPENFILENAME) };
    ofn.hwndOwner = m_hWnd;
    ofn.hInstance = GetModuleHandle(nullptr);
    ofn.lpstrFilter = _T("*.bmp\0*.bmp\0");
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = szPath;
    ofn.nMaxFile = _countof(szPath);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (!GetOpenFileName(&ofn))
    {
        return 0;
    }

    HBITMAP hBitmap = (HBITMAP)LoadImage(GetModuleHandle(nullptr), szPath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    DWORD dw = GetLastError();

    BITMAP bm = {};
    GetObject(hBitmap, sizeof(bm), &bm);

    if (bm.bmBitsPixel != 32)
    {
        DeleteObject(hBitmap);
        return 0;
    }

    DeleteObject(m_hBackground);
    m_hBackground = hBitmap;
    m_szBackground.cx = bm.bmWidth;
    m_szBackground.cy = bm.bmHeight;

    InvalidateRect(NULL);

    return 0;
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

    SetFocus();

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
