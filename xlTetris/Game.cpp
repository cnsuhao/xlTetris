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
#include <tchar.h>
#include <xl/String/xlString.h>
#include <xl/Meta/xlBind.h>
#include "Language.h"

Game::Game() :
    m_hWnd(nullptr),
    m_bStarted(false),
    m_bPaused(false),
    m_bGameover(false),
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
    m_bGameover = false;

    return true;
}

bool Game::Start()
{
    m_pointPosition.x = 5;
    m_pointPosition.y = 0;
    dwCount = 0;

    memset(m_byGameMap, 0, sizeof(m_byGameMap));

    GenerateNewTetris();

    NextTetris();

    m_bPaused = false;
    m_bStarted = true;
    m_bGameover = false;

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

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
    if (CanLeft())
    {
        --m_pointPosition.x;
    }

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

    return true;
}

bool Game::Right()
{
    if (CanRight())
    {
        ++m_pointPosition.x;
    }

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

    return true;
}

bool Game::Down()
{
    if (CanDown())
    {
        ++m_pointPosition.y;
    }

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

    return true;
}

bool Game::RotateLeft()
{
    if (!CanRotateLeft())
    {
        return false;
    }

    Shape::RotateLeft(m_shapeGaming);
    Shape::GetSize(m_shapeGaming, &m_size);

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

    return true;
}

bool Game::RotateRight()
{
    if (!CanRotateRight())
    {
        return false;
    }

    Shape::RotateRight(m_shapeGaming);
    Shape::GetSize(m_shapeGaming, &m_size);

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

    return true;
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

    if (CanDown())
    {
        ++m_pointPosition.y;
    }
    else
    {
        Paste();
        ClearFullRow();
        NextTetris();
    }

    InvalidateRect(m_hWnd, &m_rectGame, TRUE);

    return true;
}

bool Game::Render(RenderContext *pRC)
{
    DrawBackground(pRC);
    DrawPreviewShape(pRC);
    DrawShape(pRC);

    if (m_bGameover)
    {
        DrawGameOver(pRC);
    }

    DrawRenderEngine(pRC);

    return true;
}

void Game::DrawBackground(RenderContext *pRC)
{
    for (int i = 0; i < TETRIS_MAX_X; ++i)
    {
        for (int j = 0; j < TETRIS_MAX_Y; ++j)
        {
            if (m_byGameMap[j][i] != 0)
            {
                DrawTetris(pRC, i, j);
            }
            else
            {
                ClearTetris(pRC, i, j);
            }
        }
    }
}

void Game::Clear(RenderContext *pRC, LPCRECT rect)
{
    RGBQUAD color = { 0x00, 0x00, 0x00, 0xff };
    pRC->FillSolidRect(rect, color);
}

void Game::ClearTetris(RenderContext *pRC, int x, int y)
{
    RECT rect = { m_rectGame.left + x * TETRIS_SIZE,
                  m_rectGame.top  + y * TETRIS_SIZE,
                  m_rectGame.left + (x + 1) * TETRIS_SIZE,
                  m_rectGame.top  + (y + 1) * TETRIS_SIZE };
    RGBQUAD color = { 0x00, 0x00, 0x00, 0xff };
    pRC->FillSolidRect(&rect, color);
}

void Game::DrawTetris(RenderContext *pRC, int x, int y)
{
    RECT rect = { m_rectGame.left + x * TETRIS_SIZE + 1,
                  m_rectGame.top  + y * TETRIS_SIZE + 1,
                  m_rectGame.left + (x + 1) * TETRIS_SIZE,
                  m_rectGame.top  + (y + 1) * TETRIS_SIZE };
    RGBQUAD color = { 0x80, 0x80, 0x80, 0xff };
    pRC->FillSolidRect(&rect, color);
}

void Game::DrawPreviewTetris(RenderContext *pRC, int x, int y)
{
    RECT rect = { m_rectPreview.left + x * TETRIS_SIZE + 1,
                  m_rectPreview.top  + y * TETRIS_SIZE + 1,
                  m_rectPreview.left + (x + 1) * TETRIS_SIZE,
                  m_rectPreview.top  + (y + 1) * TETRIS_SIZE };
    RGBQUAD color = { 0x80, 0x80, 0x80, 0xff };
    pRC->FillSolidRect(&rect, color);
}

void Game::ClearShape(RenderContext *pRC)
{
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0)
            {
                ClearTetris(pRC, m_pointPosition.x + i, m_pointPosition.y + j);
            }
        }
    }
}

void Game::DrawShape(RenderContext *pRC)
{
    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapeGaming[j][i] != 0)
            {
                DrawTetris(pRC, m_pointPosition.x + i, m_pointPosition.y + j);
            }
        }
    }
}

void Game::DrawPreviewShape(RenderContext *pRC)
{
    Clear(pRC, &m_rectPreview);

    for (int i = 0; i < Shape::MAX_SHAPE_SIZE; ++i)
    {
        for (int j = 0; j < Shape::MAX_SHAPE_SIZE; ++j)
        {
            if (m_shapePreview[j][i] != 0)
            {
                DrawPreviewTetris(pRC, 1 + i, 1 + j);
            }
        }
    }
}

void Game::DrawGameOver(RenderContext *pRC)
{
    xl::String strGameover = _Language.GetString(_T("ID_GameOver"));
    RGBQUAD color = { 0x00, 0xff, 0xff, 0xff };
    pRC->DrawText(strGameover, strGameover.Length(), &m_rectGame, DT_SINGLELINE | DT_CENTER | DT_VCENTER, color);
}

void Game::DrawRenderEngine(RenderContext *pRC)
{
    xl::String strRenderer = _Renderer->GetName();
    RGBQUAD color = { 0x00, 0x80, 0x80, 0x80 };
    pRC->DrawText(strRenderer, strRenderer.Length(), &m_rectGame, DT_SINGLELINE, color);
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

bool Game::Paste()
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

bool Game::ClearFullRow()
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
        }
        else if (nDistance > 0)
        {
            for (int i = 0; i < TETRIS_MAX_X; ++i)
            {
                m_byGameMap[j + nDistance][i] = m_byGameMap[j][i];
            }
        }
    }

    for (int j = nDistance - 1; j >= 0; --j)
    {
        for (int i = 0; i < TETRIS_MAX_X; ++i)
        {
            m_byGameMap[j][i] =0;
        }
    }

    if (nDistance > 0)
    {
        int nScore = 5 * nDistance * (nDistance + 1);
        m_fnAddScore(nScore);
    }

    return false;
}

bool Game::NextTetris()
{
    memcpy(m_shapeGaming, m_shapePreview, sizeof(Shape::ShapeMatrix));
    Shape::GetSize(m_shapeGaming, &m_size);
    m_pointPosition.x = (TETRIS_MAX_X - m_size.cx) / 2;
    m_pointPosition.y = 0;

    if (IsGameOver())
    {
        m_bGameover = true;
        Stop();
        m_fnGameOver();
    }
    else
    {
        GenerateNewTetris();
    }

    return true;
}

bool Game::GenerateNewTetris()
{
    Shape::TetrisShapes ts = (Shape::TetrisShapes)(rand() % Shape::TS_MAX);
    Shape::GetPredefinedShape(ts, m_shapePreview);

    InvalidateRect(m_hWnd, &m_rectPreview, TRUE);

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

