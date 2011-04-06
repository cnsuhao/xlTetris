//------------------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   Tetris.h
//    Author:      Streamlet
//    Create Time: 2011-03-14
//    Description: 
//
//    Version history:
//
//
//
//------------------------------------------------------------------------------

#ifndef __TETRIS_H_12CABA9F_6F89_41F0_AB5E_6109B3DAE613_INCLUDED__
#define __TETRIS_H_12CABA9F_6F89_41F0_AB5E_6109B3DAE613_INCLUDED__


#include <Windows.h>

class Tetris
{
public:
    Tetris();
    ~Tetris();

public:
    bool Initialize();

private:
    void Release();

public:
    void Run();

private:
    HANDLE m_hWaitableTimer;
};

__declspec(selectany) Tetris _Tetris;


#endif // #ifndef __TETRIS_H_12CABA9F_6F89_41F0_AB5E_6109B3DAE613_INCLUDED__
