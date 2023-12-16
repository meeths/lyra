
#include <Engine/EngineLoop.h>
#include <Core/Types.h>
#include <Profiler/Profiler.h>
#include <String/String.h>

// Created on 2020-08-05 by Sisco
namespace
{
    constexpr lyra::Array<const char*, static_cast<size_t>(lyra::EngineLoop::Phase::MaxPhases)> phaseNames =
    {
        "StartFrame",
        "PreUpdate",
        "Update",
        "PostUpdate",
        "PreRender",
        "Render",
        "PostRender",
        "EndFrame"
    };
}

namespace lyra
{
    void EngineLoop::Step(float _delta)
    {
        uint32 phaseNameIndex = 0; 
        for (auto& phase : mFramePhases)
        {
            ProfileScoped;
            ProfileScopeName(phaseNames[phaseNameIndex], strlen(phaseNames[phaseNameIndex]));
            
            ++phaseNameIndex;
            phase.Run(_delta);
        }
    }

    void EngineLoop::AddExecutionUnit(Phase _phase, Task&& _executionUnit)
    {
        mFramePhases[static_cast<int>(_phase)].AddTask(std::move(_executionUnit));
    }
}

