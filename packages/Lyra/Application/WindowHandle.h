
#pragma once
#include "String/String.h"
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif
// Created on 2024-11-12 by sisco

namespace lyra
{

class WindowHandle
{
public:
    enum class Type
    {
        Unknown,
        Win32
    };

    static WindowHandle FromHWND(HWND hwnd)
    {
        WindowHandle handle = {};
        handle.m_type = Type::Win32;
        handle.m_platformHandle = reinterpret_cast<intptr_t>(hwnd);
        return handle;
    }

    static WindowHandle FromString(StringView windowHandleString)
    {
        WindowHandle handle = {};
#ifdef _WIN32
        handle.m_type = Type::Win32;
#else
#error Platform not implemented
#endif
        handle.m_platformHandle = std::stoll(windowHandleString.data());
        return handle;
    }

    template <typename T>
    T ConvertToPlatformWindowHandle()
    {
        return *reinterpret_cast<T*>(&m_platformHandle);
    }
    
private:
    Type m_type{};
    intptr_t m_platformHandle{};
};

}

