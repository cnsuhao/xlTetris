//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Shape.cpp
//    Author:      Streamlet
//    Create Time: 2011-03-19
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------



#include "Shape.h"

namespace Shape
{
    enum PredifinedTetrisShapes
    {
        PTS_NULL  = 0,
        PTS_TEST  = 1,
        PTS_BLOCK,
        PTS_LONG,
        PTS_Z,
        PTS_Z_R,
        PTS_7,
        PTS_7_R,
        PTS_T,
        PTS_MAX
    };

    const ShapeMatrix PREDEFINED_TETRIS_SHAPES[PTS_MAX] =
    {
        {   // PTS_NULL
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        {   // PTS_TEST
            { 0, 1, 2, 3 },
            { 4, 5, 6, 7 },
            { 8, 9,10,11 },
            {12,13,14,15 }
        },
        {   // PTS_BLOCK
            { 1, 1, 0, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        {   // PTS_LONG
            { 1, 0, 0, 0 },
            { 1, 0, 0, 0 },
            { 1, 0, 0, 0 },
            { 1, 0, 0, 0 }
        },
        {   // PTS_Z
            { 1, 1, 0, 0 },
            { 0, 1, 1, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        {   // PTS_Z_R
            { 0, 1, 1, 0 },
            { 1, 1, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        {   // PTS_7
            { 1, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 1, 0, 0 },
            { 0, 0, 0, 0 }
        },
        {   // PTS_7_R
            { 1, 1, 0, 0 },
            { 1, 0, 0, 0 },
            { 1, 0, 0, 0 },
            { 0, 0, 0, 0 }
        },
        {   // PTS_T
            { 1, 0, 0, 0 },
            { 1, 1, 0, 0 },
            { 1, 0, 0, 0 },
            { 0, 0, 0, 0 }
        }
    };

    void GetPredefinedShape(TetrisShapes index, ShapeMatrix &matrix)
    {
        PredifinedTetrisShapes pts = PTS_NULL;

        switch (index)
        {
        case TS_BLOCK:
            pts = PTS_BLOCK;
            break;
        case TS_LONG:
            pts = PTS_LONG;
            break;
        case TS_Z:
            pts = PTS_Z;
            break;
        case TS_Z_R:
            pts = PTS_Z_R;
            break;
        case TS_7:
            pts = PTS_7;
            break;
        case TS_7_R:
            pts = PTS_7_R;
            break;
        case TS_T:
            pts = PTS_T;
            break;
        default:
            break;
        }

        memcpy(matrix, PREDEFINED_TETRIS_SHAPES[pts], sizeof(ShapeMatrix));
    }

    void ClearTopLeftZeros(ShapeMatrix &shape)
    {
        int nDistanceI = 0;

        for (int i = 0; i < MAX_SHAPE_SIZE; ++i)
        {
            bool bEmpty = true;

            for (int j = 0; j < MAX_SHAPE_SIZE; ++j)
            {
                if (shape[i][j] != 0)
                {
                    bEmpty = false;
                    break;
                }
            }

            if (bEmpty)
            {
                ++nDistanceI;
            }
            else
            {
                break;
            }
        }

        if (nDistanceI > 0)
        {
            for (int i = 0; i < MAX_SHAPE_SIZE; ++i)
            {
                if (i + nDistanceI < MAX_SHAPE_SIZE)
                {
                    for (int j = 0; j < MAX_SHAPE_SIZE; ++j)
                    {
                        shape[i][j] = shape[i + nDistanceI][j];
                    }
                }
                else
                {
                    for (int j = 0; j < MAX_SHAPE_SIZE; ++j)
                    {
                        shape[i][j] = 0;
                    }
                }
            }
        }

        int nDistanceJ = 0;

        for (int j = 0; j < MAX_SHAPE_SIZE; ++j)
        {
            bool bEmpty = true;

            for (int i = 0; i < MAX_SHAPE_SIZE - nDistanceI; ++i)
            {
                if (shape[i][j] != 0)
                {
                    bEmpty = false;
                    break;
                }
            }

            if (bEmpty)
            {
                ++nDistanceJ;
            }
            else
            {
                break;
            }
        }

        if (nDistanceJ > 0)
        {
            for (int j = 0; j < MAX_SHAPE_SIZE; ++j)
            {
                if (j + nDistanceJ < MAX_SHAPE_SIZE)
                {
                    for (int i = 0; i < MAX_SHAPE_SIZE - nDistanceI; ++i)
                    {
                        shape[i][j] = shape[i][j + nDistanceJ];
                    }
                }
                else
                {
                    for (int i = 0; i < MAX_SHAPE_SIZE - nDistanceI; ++i)
                    {
                        shape[i][j] = 0;
                    }
                }
            }
        }
    }

    void RotateLeft(ShapeMatrix &shape)
    {
        // 0 1 2 3      3 7 b f
        // 4 5 6 7  =>  2 6 a e
        // 8 9 a b      1 5 9 d
        // c d e f      0 4 8 c

        for (int i = 0; i < MAX_SHAPE_SIZE / 2; ++i)
        {
            int ri = MAX_SHAPE_SIZE - 1 - i;

            for (int j = i; j < MAX_SHAPE_SIZE - 1 - i; ++j)
            {
                int rj = MAX_SHAPE_SIZE - 1 - j;

                auto tmp = shape[i][j];
                shape[i][j] = shape[j][ri];
                shape[j][ri] = shape[ri][rj];
                shape[ri][rj] = shape[rj][i];
                shape[rj][i] = tmp;
            }
        }

        ClearTopLeftZeros(shape);
    }

    void RotateRight(ShapeMatrix &shape)
    {
        // 0 1 2 3      c 8 4 0
        // 4 5 6 7  =>  d 9 5 1
        // 8 9 a b      e a 6 2
        // c d e f      f b 7 3

        for (int i = 0; i < MAX_SHAPE_SIZE / 2; ++i)
        {
            int ri = MAX_SHAPE_SIZE - 1 - i;

            for (int j = i; j < MAX_SHAPE_SIZE - 1 - i; ++j)
            {
                int rj = MAX_SHAPE_SIZE - 1 - j;

                auto tmp = shape[rj][i];
                shape[rj][i] = shape[ri][rj];
                shape[ri][rj] = shape[j][ri];
                shape[j][ri] = shape[i][j];
                shape[i][j] = tmp;
            }
        }

        ClearTopLeftZeros(shape);
    }

    void GetSize(const ShapeMatrix shape, LPSIZE pSize)
    {
        pSize->cx = -1;

        for (int i = MAX_SHAPE_SIZE; i > 0 && pSize->cx == -1; --i)
        {
            for (int j = MAX_SHAPE_SIZE; j > 0 && pSize->cx == -1; --j)
            {
                if (shape[j - 1][i - 1] != 0)
                {
                    pSize->cx = i;
                    break;
                }
            }
        }

        pSize->cy = -1;

        for (int i = MAX_SHAPE_SIZE; i > 0 && pSize->cy == -1; --i)
        {
            for (int j = MAX_SHAPE_SIZE; j > 0 && pSize->cy == -1; --j)
            {
                if (shape[i - 1][j - 1] != 0)
                {
                    pSize->cy = i;
                    break;
                }
            }
        }
    }

    bool IsEmpty(const ShapeMatrix shape, int x, int y)
    {
        if (x < 0 || x >= MAX_SHAPE_SIZE || y < 0 || y >= MAX_SHAPE_SIZE)
        {
            return true;
        }

        return shape[x][y] == 0;
    }
};

