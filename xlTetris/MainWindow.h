//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   MainWindow.h
//    Author:      Streamlet
//    Create Time: 2011-03-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __MAINWINDOW_H_7F2C24AC_2578_4C0F_9566_3D4705B7DD9A_INCLUDED__
#define __MAINWINDOW_H_7F2C24AC_2578_4C0F_9566_3D4705B7DD9A_INCLUDED__


#include <xl/Win32/GUI/xlWindow.h>
#include <xl/Win32/GUI/xlStdButton.h>
#include <xl/Win32/GUI/xlStdStatic.h>
#include <xl/Win32/GUI/xlStdLink.h>
#include <xl/String/xlString.h>

const int  MW_WIDTH         = 400;
const int  MW_HEIGHT        = 480;
const int  MW_GAME_WIDTH    = 240;
const int  MW_PREVIEW_WIDTH = 120;
const RECT MW_INST_RECT     = { MW_GAME_WIDTH, 0, MW_WIDTH, MW_HEIGHT };
const RECT MW_GAME_RECT     = { 0, 0, MW_GAME_WIDTH, MW_HEIGHT };
const int  MW_MARGIN        = (MW_WIDTH - MW_GAME_WIDTH - MW_PREVIEW_WIDTH) / 2;
const RECT MW_PREVIEW_RECT  = { MW_GAME_WIDTH + MW_MARGIN, MW_MARGIN, MW_WIDTH - MW_MARGIN,MW_MARGIN + MW_PREVIEW_WIDTH };

class MainWindow : public xl::Window
{
public:
    MainWindow();
    ~MainWindow();

private:
    // Controls

    // Labels
    xl::StdStatic m_labelScoreInst;
    xl::StdStatic m_labelScore;
    
    // Buttons
    xl::StdButton m_buttonPause;
    xl::StdButton m_buttonStart;

    // Links
    xl::StdLink m_linkWebSite;

private:
    HFONT m_hScoreFont;

private:
    void CreateControls();
    void SetTexts();

private:
    // Message Map Handlers
    // LRESULT OnMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnCreate         (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnDestroy        (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnEraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnPaint          (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnActivate       (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnSize           (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnKeyDown        (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT OnLButtonDown    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
    // Command Message Handlers
    // LRESULT OnCommandMessage(HWND hWnd, WORD wID, WORD wCode, HWND hControl);
    LRESULT OnButtonStart(HWND hWnd, WORD wID, WORD wCode, HWND hControl);
    LRESULT OnButtonPause(HWND hWnd, WORD wID, WORD wCode, HWND hControl);

private:
    // Notify Message Handlers
    // LRESULT OnNotifyMessage(HWND hWnd, UINT uID, UINT uCode, HWND hControl);
    LRESULT OnLinkWebsiteClick(HWND hWnd, UINT uID, UINT uCode, HWND hControl);

private:
    void Start();
    void Stop();
    void Pause();
    void Continue();

private:
    void UpdateScore();

public:
    // The callback to _Game, called when new score comes.
    void OnAddScore(int nScore);
    // The callback to _Game, called when game over.
    void OnGameOver();

private:
    xl::String m_strStart;
    xl::String m_strStop;
    xl::String m_strPause;
    xl::String m_strContinue;

private:
    bool m_bStarted;
    bool m_bPaused;

private:
    int m_nScore;
};

__declspec(selectany) MainWindow _MainWindow;

#endif // #ifndef __MAINWINDOW_H_7F2C24AC_2578_4C0F_9566_3D4705B7DD9A_INCLUDED__
