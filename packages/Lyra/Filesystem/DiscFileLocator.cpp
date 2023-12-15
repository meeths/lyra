
#include <Filesystem/DiscFileLocator.h>
#include <Filesystem/FileSystem.h>
#include <Filesystem/File.h>

// Created on 2023-11-15 by sisco

namespace lyra
{
    Optional<SharedPointer<IFile>> DiscFileLocator::LocateInternal(StringView path)
    {
        if(FileSystem::FileExists(path))
        {
            return MakeSharedPointer<File>(path);
        }
        return {};
    }
}

