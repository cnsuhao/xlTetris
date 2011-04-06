//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Game.cpp
//    Author:      Streamlet
//    Create Time: 2011-03-15
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------


#include "Game.h"
#include <time.h>


Game::Game() :
    m_hWnd(nullptr),
    m_bStarted(false),
    m_bPaused(false),
    dwCount(0)
{
    memset(m_byGameMap, 0, sizeof(m_byGameMap));
    memset(m_shapeGaming, 0, sizeof(m_shapeGaming));
    memset(m_shapePreview, 0, sizeof(m_shapePreview));

    srand((unsigned int)time(NULL));
}

Game::~Game()
{
    
}

bool Game::Initialize(HWND hWnd, LPCRECT rectGame, LPCRECT rectPreview, FN_AddScore fnAddScore, FN_GameOver fnGameover)
{
    m_hWnd = hWnd;

    memcpy(&m_rectGame, rectGame, sizeof(RECT));
    memcpy(&m_rectPreview, rectPreview, sizeof(RECT));

    m_fnAddScore = fnAddScore;
    m_fnGameOver = fnGameover;

    return true;
}

bool Game::Start()
{
    m_pointPosition.x = 5;
    m_pointPosition.y = 0;
    dwCount = 0;

    memset(m_byGameMap, 0, sizeof(m_byGameMap));

    GenerateNewTetris();

    HDC hDC = GetDC(m_hWnd);

    Clear(hDC, &m_rectGame);
    NextTetris(hDC);

    ReleaseDC(m_hWnd, hDC);

    m_bPaused = false;
    m_bStarted = true;

    return true;
}

bool Game::Stop()
{
    m_bStarted = false;
    return true;
}

bool Game::Pause()
{
    m_bPaused = true;
    return true;
}

bool Game::Continue()
{
    m_bPaused = false;
    return true;
}

bool Game::Left()
{
    HDC hDC = GetDC(m_hWnd);

    ClearShape(hDC);

    if (CanLeft())
    {
        --m_pointPosition.x;
    }

    DrawShape(hDC);

    ReleaseDC(m_hWnd, hDC);

    return true;
}

bool Game::Right()
{
    HDC hDC = GetDC(m_hWnd);

    ClearShape(hDC);

    if (CanRight())
    {
        ++m_pointPosition.x;
    }

    DrawShape(hDC);

    ReleaseDC(m_hWnd, hDC);

    return true;
}

bool Game::Down()
{
    HDC hDC = GetDC(m_hWnd);

    ClearShape(hDC);

    if (CanDown())
    {
        ++m_pointPosition.y;
    }

    DrawShape(hDC);

    ReleaseDC(m_hWnd, hDC);

    return true;
}

bool Game::RotateLeft()
{
    if (!CanRotateLeft())
    {
        return false;
    }

    HDC hDC = GetDC(m_hWnd);

    ClearShape(hDC);

    Shape::RotateLeft(m_shapeGaming);
    Shape::GetSize(m_shapeGaming, &m_size);

    DrawShape(hDC);

    ReleaseDC(m_hWnd, hDC);

    return true;
}

bool Game::RotateRight()
{
    if (!CanRotateRight())
    {
        return false;
    }

    HDC hDC = GetDC(m_hWnd);

    ClearShape(hDC);

    Shape::RotateRight(m_shapeGaming);
    Shape::GetSize(m_shapeGaming, &m_size);

    DrawShape(hDC);

    ReleaseDC(m_hWnd, hDC);    return true;
}

bool Game::Gaming()
{
    if (!m_bStarted || m_bPaused)
    {
        return false;
    }

    if (++dwCount < 50)
    {
        return false;
    }

    dwCount = 0;

    HDC hDC = GetDC(m_hWnd);

    if (CanDown())
    {
        ClearShape(hDC);
        ++m_pointPosition.y;
        DrawShape(hDC);
    }
    else
    {
        Paste(hDC);
        ClearFullRow(hDC);
        NextTetris(hDC);
    }
    
    ReleaseDC(m_hWnd, hDC);

    return true;
}

bool Game::Render(HDC hDC)
{
    DrawBackground(hDC);
    DrawShape(hDC);

    return true;
}

void Game::DrawBackground(HDC hDC)
{
    for (int i = 0; i < TETRIS_MAX_X; ++i)
    {
        for (int j = 0; j < TETRIS_MAX_Y; ++j)
        {
            if (m_byGameMap[j][i] != 0)
            {
                DrawTetris(hDC, i, j);
            }
            else
            {
                ClearTetris(hDC, i, j);
            }
        }
    }
}

void Game::Clear(HDC hDC, LPCRECT rect)
{
    HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hDC, rect, hBrush);
}

void Game::ClearTetris(HDC hDC, int x, int y)
{
    RECT rect = { m_rectGame.left + x * TETRIS_SIZE,
                  m_rectGame.top  + y * TETRIS_SIZE,
                  m_rectGame.left + (x + 1) * TETRIS_SIZE,
                  m_rectGame.top  + (y + 1) * TETRIS_SIZE };
    HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    FillRect(hDC, &rect, hBrush);
}

void Game::DrawTetris(HDC hDC, int x, int y)
{
    RECT rect = { m_rectGame.left + x * TETRIS_SIZE + 1,
                  m_rectGame.top  + y * TETRIS_SIZE + 1,
                  m_rectGame.left + (x + 1) * TETRIS_SIZE,
                  m_rectGame.top  + (y + 1) * TETRIS_SIZE };
    HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
    FillRect(hDC, &rect, hBrush);
}

void Game::DrawPreviewTetris(HDC hDC, int x, int y)
{
    RECT rect = { m_rectPreview.left + x * TETRIS_SIZE + 1,
                  m_rectPreview.top  + y * TETRIS_SIZE + 1,
                  m_rectPreview.left + (x + 1) * TETRIS_SIZE,
                  m_rectPreview.top  + (y + 1) * TETRIS_SIZE };
    HBRUSH hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
    FillRect(hDC, &rect, hBrush);
}

void Game::ClearShape(HDC hDC)
{
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0)
            {
                ClearTetris(hDC, m_pointPosition.x + i, m_pointPosition.y + j);
            }
        }
    }
}

void Game::DrawShape(HDC hDC)
{
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0)
            {
                DrawTetris(hDC, m_pointPosition.x + i, m_pointPosition.y + j);
            }
        }
    }
}

void Game::DrawPreviewShape(HDC hDC)
{
    Clear(hDC, &m_rectPreview);

    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapePreview[j][i] != 0)
            {
                DrawPreviewTetris(hDC, 1 + i, 1 + j);
            }
        }
    }
}

bool Game::CanDown()
{
    if (m_pointPosition.y + m_size.cy >= TETRIS_MAX_Y)
    {
        return false;
    }

    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0 &&
                m_byGameMap[m_pointPosition.y + j + 1][m_pointPosition.x + i] != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::CanLeft()
{
    if (m_pointPosition.x == 0)
    {
        return false;
    }

    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0 &&
                m_byGameMap[m_pointPosition.y + j][m_pointPosition.x + i - 1] != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::CanRight()
{
    if (m_pointPosition.x + m_size.cx >= TETRIS_MAX_X)
    {
        return false;
    }

    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0 &&
                m_byGameMap[m_pointPosition.y + j][m_pointPosition.x + i + 1] != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::CanRotateLeft()
{
    Shape::ShapeMatrix shape = {};
    memcpy(&shape, &m_shapeGaming, sizeof(shape));
    Shape::RotateLeft(shape);
    SIZE size = {};
    Shape::GetSize(shape, &size);

    if (m_pointPosition.x + size.cx > TETRIS_MAX_X ||
        m_pointPosition.y + size.cy > TETRIS_MAX_Y)
    {
        return false;
    }

    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (shape[j][i] != 0 &&
                m_byGameMap[m_pointPosition.y + j][m_pointPosition.x + i] != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::CanRotateRight()
{
    Shape::ShapeMatrix shape = {};
    memcpy(&shape, &m_shapeGaming, sizeof(shape));
    Shape::RotateRight(shape);
    SIZE size = {};
    Shape::GetSize(shape, &size);

    if (m_pointPosition.x + size.cx > TETRIS_MAX_X ||
        m_pointPosition.y + size.cy > TETRIS_MAX_Y)
    {
        return false;
    }
    
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (shape[j][i] != 0 &&
                m_byGameMap[m_pointPosition.y + j][m_pointPosition.x + i] != 0)
            {
                return false;
            }
        }
    }

    return true;
}

bool Game::Paste(HDC hDC)
{
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            m_byGameMap[m_pointPosition.y + j][m_pointPosition.x + i] += m_shapeGaming[j][i];
        }
    }

    return true;
}

bool Game::ClearFullRow(HDC hDC)
{
    int nDistance = 0;

    for (int j = TETRIS_MAX_Y - 1; j >= 0; --j)
    {
        bool bFull = true;

        for (int i = 0; i < TETRIS_MAX_X; ++i)
        {
            if (m_byGameMap[j][i] == 0)
            {
                bFull = false;
                break;
            }
        }

        if (bFull)
        {
            ++nDistance;

            for (int i = 0; i < TETRIS_MAX_X; ++i)
            {
                ClearTetris(hDC, i, j);
            }
        }
        else if (nDistance > 0)
        {
            for (int i = 0; i < TETRIS_MAX_X; ++i)
            {
                m_byGameMap[j + nDistance][i] = m_byGameMap[j][i];

                if (m_byGameMap[j][i] != 0)
                {
                    DrawTetris(hDC, i, j + nDistance);
                }
                else
                {
                    ClearTetris(hDC, i, j + nDistance);
                }
            }
        }
    }

    for (int j = nDistance - 1; j >= 0; --j)
    {
        for (int i = 0; i < TETRIS_MAX_X; ++i)
        {
            m_byGameMap[j][i] =0;
            ClearTetris(hDC, i, j);
        }
    }

    if (nDistance > 0)
    {
        int nScore = 5 * nDistance * (nDistance + 1);
        m_fnAddScore(nScore);
    }

    return false;
}

bool Game::NextTetris(HDC hDC)
{
    memcpy(m_shapeGaming, m_shapePreview, sizeof(Shape::ShapeMatrix));
    Shape::GetSize(m_shapeGaming, &m_size);
    m_pointPosition.x = (TETRIS_MAX_X - m_size.cx) / 2;
    m_pointPosition.y = 0;

    if (IsGameOver())
    {
        Stop();
        m_fnGameOver();
    }
    else
    {
        DrawShape(hDC);

        GenerateNewTetris();
        DrawPreviewShape(hDC);
    }

    return true;
}

bool Game::GenerateNewTetris()
{
    Shape::TetrisShapes ts = (Shape::TetrisShapes)(rand() % Shape::TS_MAX);
    Shape::GetPredefinedShape(ts, m_shapePreview);

    return true;
}

bool Game::IsGameOver()
{
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0 &&
                m_byGameMap[m_pointPosition.y + j][m_pointPosition.x + i] != 0)
            {
                return true;
            }
        }
    }

    return false;
}

