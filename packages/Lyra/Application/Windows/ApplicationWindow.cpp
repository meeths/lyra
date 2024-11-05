#include <Application/Windows/ApplicationWindow.h>
#include <glm/vec2.hpp>
#include <Core/CoreDefines.h>
#include <String/StringUtils.h>

#include <Profiler/Profiler.h>

namespace lyra
{
    ApplicationWindow::ApplicationWindow(const glm::ivec2& size, bool _fullscreen, const String& _windowName)
    {
        mFullScreen = _fullscreen;

        WindowMessageLoop::InitInfo messageLoopInitInfo{
            [this](const glm::ivec2& size) { OnWindowResize(size); },
            [this]() { OnLoseFocus(); },
            [this]() { OnGainFocus(); },
            [this]() { OnCloseWindow(); },
            [this](Word _key){ OnKeyDown(_key); },
            [this](Word _key){ OnKeyUp(_key); },
            [this](Word _key){ OnKeyInput(_key); },
            [this](float _delta){ OnMouseWheelV(_delta); },
            [this](float _delta){ OnMouseWheelH(_delta); }
        };

        mMessageLoop = MakeUniquePointer<WindowMessageLoop>(messageLoopInitInfo);

        WNDCLASSEX oWinClass;
        mApplicationHandle = static_cast<HINSTANCE>(GetModuleHandle(nullptr));

        oWinClass.cbSize = sizeof(WNDCLASSEX);
        oWinClass.lpfnWndProc = static_cast<WNDPROC>(WindowMessageLoop::Win32MsgProc);
        oWinClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        oWinClass.cbClsExtra = 0;
        oWinClass.cbWndExtra = 0;
        oWinClass.hInstance = mApplicationHandle;
        oWinClass.hIcon = LoadIcon(mApplicationHandle, IDI_APPLICATION);
        oWinClass.hCursor = NULL;
        oWinClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
        oWinClass.lpszMenuName = nullptr;
        oWinClass.hIconSm = LoadIcon(mApplicationHandle, IDI_APPLICATION);
        oWinClass.lpszClassName = L"lyraApplicationWindow"; //m_oParams.szWindowName.c_str();


        int registerResults = RegisterClassEx(&oWinClass);
        lyraAssert(registerResults);

        if (mFullScreen) // Attempt Fullscreen Mode?
        {
            DEVMODE dmScreenSettings; // Device Mode
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings)); // Makes Sure Memory's Cleared
            dmScreenSettings.dmSize = sizeof(dmScreenSettings); // Size Of The Devmode Structure
            dmScreenSettings.dmPelsWidth = size.x; // Selected Screen Width
            dmScreenSettings.dmPelsHeight = size.y; // Selected Screen Height
            dmScreenSettings.dmBitsPerPel = 32; // Selected Bits Per Pixel
            dmScreenSettings.dmDisplayFrequency = 60;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

            // Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
            if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
            {
                // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
                if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?", L"lyraEngine",MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
                {
                    mFullScreen = FALSE; // Windowed Mode Selected.  Fullscreen = FALSE
                }
                else
                {
                    // Pop Up A Message Box Letting User Know The Program Is Closing.
                    lyraAssert(0 && "Could not start in full screen mode.");
                }
            }
        }


        DWORD dwExStyle; // Window Extended Style
        DWORD dwStyle; // Window Style

        if (mFullScreen)
        {
            dwExStyle = WS_EX_APPWINDOW; // Window Extended Style
            dwStyle = WS_POPUP; // Windows Style
        }
        else
        {
            dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // Window Extended Style
            dwStyle = WS_OVERLAPPEDWINDOW; // Windows Style
        }

        RECT windowRect = {0, 0, size.x, size.y};
        AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle); // Adjust Window To True Requested Size

        windowRect.bottom -= windowRect.top;
        windowRect.top = 0;

        mHwnd = CreateWindow( // Extended style 
            oWinClass.lpszClassName,
            StringUtils::ToWstring(_windowName).c_str(), // Title
            dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // Window Style
            windowRect.left, // Initial X
            windowRect.top, // Initial Y
            windowRect.right - windowRect.left, // Width
            windowRect.bottom - windowRect.top, // Height
            NULL, // Handle to parent
            NULL, // Handle to menu
            mApplicationHandle, // Instance of app
            this);

        lyraAssert(mHwnd);

        ::ShowWindow(mHwnd, SW_SHOW);
        ::SetForegroundWindow(mHwnd);
        ::SetFocus(mHwnd);
    }

    void ApplicationWindow::Update()
    {
        ProfileScoped;
        
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                mCloseRequested = true;
            }

            if (msg.message == WM_MOUSEMOVE ||
                msg.message == WM_LBUTTONDOWN ||
                msg.message == WM_LBUTTONUP ||
                msg.message == WM_RBUTTONDOWN ||
                msg.message == WM_RBUTTONUP ||
                msg.message == WM_MBUTTONDOWN ||
                msg.message == WM_MBUTTONUP)
            {
                const bool lButton = msg.wParam & MK_LBUTTON;
                const bool rButton = msg.wParam & MK_RBUTTON;
                const bool mButton = msg.wParam & MK_MBUTTON;
                const glm::vec2 mousePos(static_cast<float>(LOWORD(msg.lParam)), static_cast<float>(HIWORD(msg.lParam)));

                const bool lAlt = GetAsyncKeyState(VK_LMENU) & 0x8000;
                const bool lCtrl = GetAsyncKeyState(VK_LCONTROL) & 0x8000;
                const bool lShift = GetAsyncKeyState(VK_LSHIFT) & 0x8000;

                const bool rAlt = GetAsyncKeyState(VK_RMENU) & 0x8000;
                const bool rCtrl = GetAsyncKeyState(VK_RCONTROL) & 0x8000;
                const bool rShift = GetAsyncKeyState(VK_RSHIFT) & 0x8000;

                const unsigned int modifiers =
                    (lAlt ? 1 << 0 : 0) |
                    (lCtrl ? 1 << 1 : 0) |
                    (lShift ? 1 << 2 : 0) |
                    (rAlt ? 1 << 3 : 0) |
                    (rCtrl ? 1 << 4 : 0) |
                    (rShift ? 1 << 5 : 0);


                for (auto& mouseMoveCallback : mMouseMoveCallbacks)
                {
                    mouseMoveCallback(mousePos, lButton, rButton, mButton, modifiers);
                }
            }


            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    void ApplicationWindow::Shutdown()
    {
        if (mHwnd)
        {
            DestroyWindow(mHwnd);
            mHwnd = nullptr;
        }
    }

    void ApplicationWindow::SetWindowTitle(StringView _title) const
    {
        ::SetWindowTextA(mHwnd, _title.data());
    }

    void ApplicationWindow::ToggleFullscreen()
    {
        static RECT lastWindowedRect;

        mFullScreen = !mFullScreen;
        if (mFullScreen)
        {
            GetWindowRect(mHwnd, &lastWindowedRect);

            RECT desktopRect;
            const auto hDesktop = GetDesktopWindow();
            GetWindowRect(hDesktop, &desktopRect);
            const int width = desktopRect.right;
            const int height = desktopRect.bottom;

            SetWindowLongPtr(mHwnd, GWL_STYLE, WS_POPUP);
            SetWindowPos(mHwnd, HWND_TOP, 0, 0, width, height, SWP_SHOWWINDOW);

            DEVMODE dmScreenSettings;
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);
            dmScreenSettings.dmPelsWidth = width;
            dmScreenSettings.dmPelsHeight = height;
            dmScreenSettings.dmBitsPerPel = 32;
            dmScreenSettings.dmDisplayFrequency = 60;
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

            if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
            {
                // If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
                if (MessageBox(NULL, L"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Reverting back to windowed mode", L"lyraEngine",MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
                {
                    mFullScreen = false; // Windowed Mode Selected.  Fullscreen = FALSE
                }
            }
        }

        if (!mFullScreen)
        {
            ChangeDisplaySettings(nullptr, 0);
            SetWindowLongPtr(mHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
            SetWindowPos(mHwnd, HWND_TOP,
                         lastWindowedRect.left,
                         lastWindowedRect.top,
                         lastWindowedRect.right - lastWindowedRect.left,
                         lastWindowedRect.bottom - lastWindowedRect.top, SWP_SHOWWINDOW);
        }
    }

    glm::ivec2 ApplicationWindow::GetDimensions() const
    {
        RECT clientRect;
        GetClientRect(mHwnd, &clientRect);
        return { clientRect.right - clientRect.left, clientRect.bottom - clientRect.top };
    }

    void ApplicationWindow::OnWindowResize(const glm::ivec2& newSize)
    {
        const glm::vec2 windowSize(static_cast<float>(newSize.x), static_cast<float>(newSize.y));
        for (auto& resizeCallback : mResizeCallbacks)
        {
            resizeCallback(windowSize);
        }
    }

    void ApplicationWindow::OnLoseFocus()
    {
    }

    void ApplicationWindow::OnGainFocus()
    {
    }

    void ApplicationWindow::OnCloseWindow()
    {
        mCloseRequested = true;
    }

    void ApplicationWindow::OnKeyDown(Word _key)
    {
        for (auto& keyDownCallback : mKeyDownCallbacks)
        {
            keyDownCallback(_key);
        }
    }
    
    void ApplicationWindow::OnKeyUp(Word _key)
    {
        for (auto& keyUpCallback : mKeyUpCallbacks)
        {
            keyUpCallback(_key);
        }
    }
    
    void ApplicationWindow::OnKeyInput(Word _key)
    {
        for (auto& keyInputCallback : mKeyInputCallbacks)
        {
            keyInputCallback(_key);
        }
    }

    void ApplicationWindow::OnMouseWheelV(float _delta)
    {
        for (auto& mouseWheelVCallback : mMouseWheelVCallbacks)
        {
            mouseWheelVCallback(_delta);
        }
    }

    
    void ApplicationWindow::OnMouseWheelH(float _delta)
    {
        for (auto& mouseWheelHCallback : mMouseWheelHCallbacks)
        {
            mouseWheelHCallback(_delta);
        }
    }

}
