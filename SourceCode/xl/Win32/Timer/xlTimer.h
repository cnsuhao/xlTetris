//--------------------------------------------------------------------
//
//    Copyright (C) Streamlet. All rights reserved.
//
//    File Name:   xlHandle.h
//    Author:      Streamlet
//    Create Time: 2013-05-15
//    Description: 
//
//    Version history:
//
//
//--------------------------------------------------------------------

#ifndef __XLHANDLE_H_39C0A466_0EB3_4D32_879B_1984722D3AED_INCLUDED__
#define __XLHANDLE_H_39C0A466_0EB3_4D32_879B_1984722D3AED_INCLUDED__


#include <xl/Meta/xlFunction.h>
#include <xl/Win32/GUI/xlThunk.h>
#include <Windows.h>

namespace xl
{
    typedef Function<void (DWORD dwTime)> TimerCallback;

    class Timer
    {
    public:
        Timer() : m_uTimerId(0)
        {
        
        }

        ~Timer()
        {
            Kill();
        }

    public:
        bool Set(UINT uElapse, TimerCallback fnCallback)
        {
            if (m_uTimerId != 0)
            {
                return false;
            }

            m_fnCallback = fnCallback;
            m_thunk.SetObject(this);
            m_thunk.SetRealProc(StaticTimerProc);

            m_uTimerId = SetTimer(nullptr, 0, uElapse, m_thunk.GetThunkProc());

            if (m_uTimerId == 0)
            {
                return false;
            }

            return true;
        }

        void Kill()
        {
            if (m_uTimerId != 0)
            {
                KillTimer(nullptr, m_uTimerId);
                m_uTimerId = 0;
            }
        }

    protected:
        static VOID CALLBACK StaticTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
        {
            return ((Timer *)hWnd)->m_fnCallback(dwTime);
        }

    protected:
        UINT_PTR m_uTimerId;
        Thunk<TIMERPROC> m_thunk;
        TimerCallback m_fnCallback;
    };

} // namespace xl

#endif // #ifndef __XLHANDLE_H_39C0A466_0EB3_4D32_879B_1984722D3AED_INCLUDED__
