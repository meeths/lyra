#include <Application/Windows/WindowMessageLoop.h>

namespace lyra
{
WindowMessageLoop* WindowMessageLoop::mMessageLoop = nullptr;

WindowMessageLoop::WindowMessageLoop(InitInfo _creationInfo) :
    mCreationInfo(std::move(_creationInfo))
{
    mMessageLoop = this;
}
//----------------------------------------------------------------------------------------------------------------------
LRESULT WindowMessageLoop::Win32MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

    switch (msg)
    {
    case WM_DESTROY:
    {
        mMessageLoop->mCreationInfo.mOnWindowClose();
        PostQuitMessage(0);
        return 0;
    }
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
    {
        if (wParam)
        {
            mMessageLoop->mCreationInfo.mOnFocusGained();
        }
        else
        {
            mMessageLoop->mCreationInfo.mOnFocusLost();
        }
        break;
    }
    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
        {
            mMessageLoop->mCreationInfo.mOnFocusLost();
        }
        else if (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED)
        {
            mMessageLoop->mCreationInfo.mOnSizeChanged({ LOWORD(lParam) , HIWORD(lParam) });
        }
        break;
    }
    case WM_MOUSEWHEEL:
        mMessageLoop->mCreationInfo.mOnMouseWheelV(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA));
        return 0;
    case WM_MOUSEHWHEEL:
        mMessageLoop->mCreationInfo.mOnMouseWheelH(static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / static_cast<float>(WHEEL_DELTA));
        return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        mMessageLoop->mCreationInfo.mOnKeyDown(static_cast<Word>(wParam));
        return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        mMessageLoop->mCreationInfo.mOnKeyUp(static_cast<Word>(wParam));
        return 0;
    case WM_CHAR:
        if (wParam > 0 && wParam < 0x10000)
        {
            mMessageLoop->mCreationInfo.mOnKeyInput(static_cast<Word>(wParam));
        }
        return 0;
    case WM_SETCURSOR:
        if(LOWORD(lParam) == HTCLIENT)
            SetCursor(nullptr);
        return 0;
        
    default: break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
    
}
}
