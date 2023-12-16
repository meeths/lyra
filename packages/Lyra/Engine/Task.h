
#pragma once
#include <Core/Function.h>

// Created on 2020-08-05 by Sisco

namespace lyra
{

class Task
{
public:
    Task(Function<void(float)> _function)
    : m_Function(std::move(_function))
    {}

    void Execute(float _f) const { m_Function(_f); }
private:
    Function<void(float)> m_Function;
};

}

