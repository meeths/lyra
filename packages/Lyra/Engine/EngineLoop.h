
#pragma once
#include <Engine/Task.h>
#include <Engine/TaskLoop.h>
#include <Containers/Array.h>

// Created on 2020-08-05 by Sisco

namespace lyra
{

class EngineLoop
{
public:
    enum class Phase : int
    {
        StartFrame,
        PreUpdate,
        Update,
        PostUpdate,
        PreRender,
        Render,
        PostRender,
        EndFrame,
        //-------
        MaxPhases
    };

    void Step(float _delta);
    void AddExecutionUnit(Phase _phase, Task&& _executionUnit);
protected:
    Array<TaskLoop, static_cast<int>(Phase::MaxPhases)> mFramePhases;
};

}

