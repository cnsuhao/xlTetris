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


#include <xl/Windows/GUI/xlWindow.h>
#include <xl/Windows/GUI/xlStdButton.h>
#include <xl/Windows/GUI/xlStdStatic.h>
#include <xl/Windows/GUI/xlStdComboBox.h>
#include <xl/Windows/GUI/xlStdLink.h>
#include <xl/Common/String/xlString.h>
#include "Renderer.h"

const int  MW_WIDTH         = 400;
const int  MW_HEIGHT        = 480;
const int  MW_GAME_WIDTH    = 240;
const int  MW_PREVIEW_WIDTH = 120;
const RECT MW_INST_RECT     = { MW_GAME_WIDTH, 0, MW_WIDTH, MW_HEIGHT };
const RECT MW_GAME_RECT     = { 0, 0, MW_GAME_WIDTH, MW_HEIGHT };
const int  MW_MARGIN        = (MW_WIDTH - MW_GAME_WIDTH - MW_PREVIEW_WIDTH) / 2;
const RECT MW_PREVIEW_RECT  = { MW_GAME_WIDTH + MW_MARGIN, MW_MARGIN, MW_WIDTH - MW_MARGIN,MW_MARGIN + MW_PREVIEW_WIDTH };

class MainWindow : public xl::Windows::Window
{
public:
    MainWindow();
    ~MainWindow();

private:
    // Controls

    // Labels
    xl::Windows::StdStatic m_labelScoreInst;
    xl::Windows::StdStatic m_labelScore;
    
    // Buttons
    xl::Windows::StdButton m_buttonPause;
    xl::Windows::StdButton m_buttonStart;
    xl::Windows::StdButton m_buttonChangeImage;

    // ComboBox
    xl::Windows::StdComboBox m_comboRenderer;

    // Links
    xl::Windows::StdLink m_linkWebSite;

private:
    HFONT m_hScoreFont;
    HBITMAP m_hBackground;
    SIZE m_szBackground;

private:
    void CreateControls();
    void SetTexts();
    void AutoSelectRenderer();
    void ReleaseRenderer();

private:
    // Message Map Handlers
    // LRESULT OnMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnCreate         (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnDestroy        (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnEraseBackground(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnPaint          (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnActivate       (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnSize           (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnKeyDown        (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnLButtonDown    (HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

private:
    // Command Message Handlers
    // LRESULT OnCommandMessage(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled);
    LRESULT OnButtonStart(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled);
    LRESULT OnButtonPause(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled);
    LRESULT OnButtonChangeImage(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled);
    LRESULT OnComboBoxRendererChange(HWND hWnd, WORD wID, WORD wCode, HWND hControl, BOOL &bHandled);

private:
    // Notify Message Handlers
    // LRESULT OnNotifyMessage(HWND hWnd, LPNMHDR lpNMHDR, BOOL &bHandled);
    LRESULT OnLinkWebsiteClick(HWND hWnd, LPNMHDR lpNMHDR, BOOL &bHandled);

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
    int m_iRenderer;
    RenderContext *m_pRC;

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
