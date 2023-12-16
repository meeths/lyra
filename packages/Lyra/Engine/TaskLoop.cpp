
#include <Engine/TaskLoop.h>

// Created on 2020-08-05 by Sisco

namespace lyra
{
    void TaskLoop::AddTask(Task&& _task)
    {
        m_Tasks.emplace_back(_task);
    }

    void TaskLoop::Run(float _delta)
    {
        for (auto& task : m_Tasks)
        {
            task.Execute(_delta);
        }
    }
}

