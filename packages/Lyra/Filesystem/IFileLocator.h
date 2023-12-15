
#pragma once

// Created on 2023-11-15 by sisco
#include <String/String.h>
#include <Core/Types.h>

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
        
        if(mNextFileLocator)
            return mNextFileLocator->Locate(path);

        return {};
    }
    
    IFileLocator& SetNextFileLocator(UniquePointer<IFileLocator> locator)
    {
        mNextFileLocator = std::move(locator);
        return *mNextFileLocator;
    }
    
protected:
    virtual Optional<SharedPointer<IFile>> LocateInternal(StringView path) = 0;
    UniquePointer<IFileLocator> mNextFileLocator;
};

}

