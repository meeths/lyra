
#pragma once

// Created on 2023-12-16 by sisco
#include <taskflow.hpp>
#include <Core/Function.h>
#include <String/String.h>

namespace lyra
{

class TaskGraphExecutorThreadObserver : public tf::ObserverInterface
{
public:
    TaskGraphExecutorThreadObserver(
        const String& name,
        Function<void(int)> _onSetup,
        Function<void(int, const String&)> _onStart,
        Function<void(int, const String&)> _onEnd) :
        m_onSetup(std::move(_onSetup)), m_onStart(std::move(_onStart)), m_onEnd(std::move(_onEnd))
    {
    }

    void set_up(size_t num_workers) override final
    {
        m_onSetup(static_cast<int>(num_workers));
    }

    void on_entry(tf::WorkerView w, tf::TaskView tv) override final
    {
        m_onStart(static_cast<int>(w.id()), tv.name());
    }

    void on_exit(tf::WorkerView w, tf::TaskView tv) override final
    {
        m_onEnd(static_cast<int>(w.id()), tv.name());
    }
    
private:
    Function<void(int)> m_onSetup;
    Function<void(int, const String&)> m_onStart;
    Function<void(int, const String&)> m_onEnd;
};

}

