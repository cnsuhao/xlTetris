//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Shape.h
//    Author:      Streamlet
//    Create Time: 2011-03-19
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __SHAPE_H_0F2D00E4_0FE9_4003_93C7_601A2831EBF0_INCLUDED__
#define __SHAPE_H_0F2D00E4_0FE9_4003_93C7_601A2831EBF0_INCLUDED__

#include <Windows.h>

namespace Shape
{
    const int MAX_SHAPE_SIZE = 4;
    typedef unsigned char ShapeMatrix[MAX_SHAPE_SIZE][MAX_SHAPE_SIZE];

    enum TetrisShapes
    {
        TS_BLOCK = 0,
        TS_LONG,
        TS_Z,
        TS_Z_R,
        TS_7,
        TS_7_R,
        TS_T,
        TS_MAX
    };

    void GetPredefinedShape(TetrisShapes index, ShapeMatrix &matrix);
    void RotateLeft(ShapeMatrix &shape);
    void RotateRight(ShapeMatrix &shape);
    void GetSize(const ShapeMatrix shape, LPSIZE pSize);
    bool IsEmpty(const ShapeMatrix shape, int x, int y);
};

#endif // #ifndef __SHAPE_H_0F2D00E4_0FE9_4003_93C7_601A2831EBF0_INCLUDED__
