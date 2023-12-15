
#pragma once
#include <Filesystem/IFileLocator.h>

// Created on 2023-11-15 by sisco

namespace lyra
{

class DiscFileLocator : public IFileLocator
{
protected:
    Optional<SharedPointer<IFile>> LocateInternal(StringView path) override;
};

}

