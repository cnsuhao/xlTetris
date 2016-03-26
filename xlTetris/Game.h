//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Game.h
//    Author:      Streamlet
//    Create Time: 2011-03-15
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __GAME_H_3B2655D1_4D60_473A_A5DE_228943FD605B_INCLUDED__
#define __GAME_H_3B2655D1_4D60_473A_A5DE_228943FD605B_INCLUDED__


#include <xl/Common/Meta/xlFunction.h>
#include <xl/Windows/GUI/xlDPI.h>
#include <Windows.h>
#include "Shape.h"
#include "Renderer.h"

const int TETRIS_SIZE  = XL_DPI_Y(24);
const int TETRIS_MAX_X = 10;
const int TETRIS_MAX_Y = 20;

typedef xl::Function<void (int)> FN_AddScore;
typedef xl::Function<void ()> FN_GameOver;

class Game
{
public:
    Game();
    ~Game();

public:
    bool Initialize(HWND hWnd, LPCRECT rectGame, LPCRECT rectPreview, FN_AddScore fnAddScore, FN_GameOver fnGameOver);

public:
    bool Start();
    bool Stop();
    bool Pause();
    bool Continue();

public:
    bool Left();
    bool Right();
    bool Down();
    bool RotateLeft();
    bool RotateRight();

public:
    bool Gaming();

public:
    bool Render(RenderContext *pRC);

private:
    HWND m_hWnd;
    RECT m_rectGame;
    RECT m_rectPreview;
    FN_AddScore m_fnAddScore;
    FN_GameOver m_fnGameOver;

private:
    bool m_bStarted;
    bool m_bPaused;
    bool m_bGameover;
    DWORD dwCount;

private:
    BYTE m_byGameMap[TETRIS_MAX_Y][TETRIS_MAX_X];

private:
    Shape::ShapeMatrix m_shapePreview;
    Shape::ShapeMatrix m_shapeGaming;
    POINT m_pointPosition;
    SIZE m_size;

private:
    // Draw background Tetris
    void DrawBackground(RenderContext *pRC);
    // Clear a region
    void Clear(RenderContext *pRC, LPCRECT rect);
    // Clear a block of Tetris
    void ClearTetris(RenderContext *pRC, int x, int y);
    // Draw a block of Tetris
    void DrawTetris(RenderContext *pRC, int x, int y);
    // Draw Tetris in preview region
    void DrawPreviewTetris(RenderContext *pRC, int x, int y);
    // Clear current Tetris
    void ClearShape(RenderContext *pRC);
    // Draw current Tetris
    void DrawShape(RenderContext *pRC);
    // Draw preview Tetris
    void DrawPreviewShape(RenderContext *pRC);
    // Draw game-over prompt
    void DrawGameOver(RenderContext *pRC);
    // Draw render engine name
    void DrawRenderEngine(RenderContext *pRC);

private:
    // If can move down
    bool CanDown();
    // If can move left
    bool CanLeft();
    // If can move right
    bool CanRight();
    // If can rotate left
    bool CanRotateLeft();
    // If can rotate right
    bool CanRotateRight();
    // Past the current Tetris to background
    bool Paste();
    // Check all rows and clear those with blocks full filled.
    bool ClearFullRow();
    // Use the Tetris in preview region as the current one, jduge if game over, and generate new preview Tetris
    bool NextTetris();
    // Generate new preview Tetris
    bool GenerateNewTetris();
    // Judge if game over
    bool IsGameOver();
};

__declspec(selectany) Game _Game;

#endif // #ifndef __GAME_H_3B2655D1_4D60_473A_A5DE_228943FD605B_INCLUDED__
