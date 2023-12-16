
#pragma once

// Created on 2023-11-15 by sisco
#include <Core/Optional.h>
#include <Memory/SharedPointer.h>
#include <Memory/UniquePointer.h>
#include <String/String.h>

namespace lyra
{
class IFile;
class IFileLocator
{
public:
    virtual ~IFileLocator() = default;
    virtual Optional<SharedPointer<IFile>> Locate(StringView path)
    {
        auto located = LocateInternal(path);

        if(located)
        {
            return located;
        }
        
        if(m_NextFileLocator)
            return m_NextFileLocator->Locate(path);

        return {};
    }
    
    IFileLocator& SetNextFileLocator(UniquePointer<IFileLocator> locator)
    {
        m_NextFileLocator = std::move(locator);
        return *m_NextFileLocator;
    }
    
protected:
    virtual Optional<SharedPointer<IFile>> LocateInternal(StringView path) = 0;
    UniquePointer<IFileLocator> m_NextFileLocator;
};

}

