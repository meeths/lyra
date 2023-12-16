
#pragma once
#include <chrono>
#include <iostream>
#include <thread>
#include <Time/TimeTypes.h>
#include <Profiler/Profiler.h>

// Created on 2020-08-18 by Sisco

namespace lyra
{

class FrameLimiter
{
public:
    FrameLimiter(int fps)
    {
       mInvFPSLimit = std::chrono::duration_cast<std::chrono::system_clock::duration>(dsec{1./static_cast<double>(fps)});

       mBeginFrame = std::chrono::system_clock::now();
       mEndFrame = mBeginFrame + mInvFPSLimit;

    }
    void EndFrame();
    void StartFrame();
private:
    using dsec = std::chrono::duration<double>;
    std::chrono::system_clock::duration mInvFPSLimit;
    std::chrono::time_point<std::chrono::system_clock> mBeginFrame;
    std::chrono::time_point<std::chrono::system_clock> mEndFrame;
    
};

inline void FrameLimiter::EndFrame()
{
    ProfileScoped;

    // This part keeps the frame rate.
    std::this_thread::sleep_until(mEndFrame);
    mBeginFrame = mEndFrame;
    mEndFrame = mBeginFrame + mInvFPSLimit;
}

inline void FrameLimiter::StartFrame()
{
}
}

