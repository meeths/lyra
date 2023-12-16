
#pragma once

// Created on 2020-08-05 by Sisco
#include <Engine/Task.h>
#include <Containers/Vector.h>

namespace lyra
{
class TaskLoop
{
public:
    void AddTask(Task&& _task);
    void Run(float _delta);
private:
    Vector<Task> m_Tasks;
};

}

