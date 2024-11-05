#pragma once
#include <glm/vec2.hpp>
#include <functional>
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
#include <Core/Types.h>
#include <Core/Function.h>

namespace lyra
{
class WindowMessageLoop
{
public:
	typedef struct
	{
		Function<void(const glm::ivec2&)> mOnSizeChanged;
		Function<void(void)> mOnFocusLost;
		Function<void(void)> mOnFocusGained;
		Function<void(void)> mOnWindowClose;
		Function<void(Word)> mOnKeyDown;
		Function<void(Word)> mOnKeyUp;
		Function<void(Word)> mOnKeyInput;
		Function<void(float)> mOnMouseWheelV;
		Function<void(float)> mOnMouseWheelH;
	}InitInfo;
	
	WindowMessageLoop(InitInfo _creationInfo);

	static LRESULT Win32MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
private:
	static WindowMessageLoop* mMessageLoop;

	InitInfo mCreationInfo;
};

}
