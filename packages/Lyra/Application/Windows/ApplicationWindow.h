#pragma once
#include <Application/Windows/WindowMessageLoop.h>
#include <Core/Types.h>
#include <glm/vec2.hpp>
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <Containers/Vector.h>
#include <Memory/UniquePointer.h>
#include <String/String.h>

#include "Application/WindowHandle.h"

namespace lyra
{
	struct ChangeWindowTitleEvent;

	class ApplicationWindow
{
public:
	ApplicationWindow(const glm::ivec2& size, bool _fullscreen, const String& _windowName);
	
	void RequestClose() { mCloseRequested = true; }
	bool CloseRequested() const { return mCloseRequested; }
	void Update();
	void Shutdown();

	WindowHandle GetWindowHandle() const { return WindowHandle::FromHWND(mHwnd); }
	HINSTANCE GetInstanceHandle() const { return mApplicationHandle; }

    typedef Function<void(glm::vec2, bool, bool, bool, unsigned int)> MouseMoveCallback;
    void AddMouseMoveCallback(MouseMoveCallback _onMouseMove) { mMouseMoveCallbacks.push_back(_onMouseMove); }

    typedef Function<void(float)> MouseWheelCallback;
    void AddMouseMoveWheelVCallback(MouseWheelCallback _onMouseWheelV) { mMouseWheelVCallbacks.push_back(_onMouseWheelV); }
    void AddMouseMoveWheelHCallback(MouseWheelCallback _onMouseWheelH) { mMouseWheelHCallbacks.push_back(_onMouseWheelH); }
    
    
	typedef Function<void(glm::vec2)> ResizeCallback;
	void AddResizeCallback(ResizeCallback _onResize) { mResizeCallbacks.push_back(_onResize); };

    typedef Function<void(Word)> KeyCallback;
    void AddKeyUpCallback(KeyCallback _onKeyUp) { mKeyUpCallbacks.push_back(_onKeyUp); };
    void AddKeyDownCallback(KeyCallback _onKeyDown) { mKeyDownCallbacks.push_back(_onKeyDown); };
    void AddKeyInputCallback(KeyCallback _onKeyInput) { mKeyInputCallbacks.push_back(_onKeyInput); };

    void SetWindowTitle(StringView _title) const;
	void ToggleFullscreen();

    glm::ivec2 GetDimensions() const;
private:
	void OnSetWindowTitleEvent(ChangeWindowTitleEvent _event) const;

	void OnWindowResize(const glm::ivec2& newSize);
	void OnLoseFocus();
	void OnGainFocus();
	void OnCloseWindow();
    void OnKeyDown(Word _key);
    void OnKeyUp(Word _key);
    void OnKeyInput(Word _key);
    void OnMouseWheelV(float _delta);
    void OnMouseWheelH(float _delta);
    
	HWND mHwnd = nullptr;
	HINSTANCE mApplicationHandle = nullptr;
	
	UniquePointer<WindowMessageLoop> mMessageLoop;
	bool mFullScreen = false;
	bool mCloseRequested = false;

    Vector<MouseMoveCallback> mMouseMoveCallbacks;
    Vector<MouseWheelCallback> mMouseWheelVCallbacks;
    Vector<MouseWheelCallback> mMouseWheelHCallbacks;
    Vector<ResizeCallback> mResizeCallbacks;
    Vector<KeyCallback> mKeyUpCallbacks;
    Vector<KeyCallback> mKeyDownCallbacks;
    Vector<KeyCallback> mKeyInputCallbacks;
    
};

}
