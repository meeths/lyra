#pragma once
#include <Containers/Vector.h>
#include <Filesystem/FileConstants.h>
#include <Memory/SharedPointer.h>
#include <String/String.h>
#include <Core/Expected.h>
#include <Core/Types.h> 
namespace lyra
{
class IFile;

namespace FileSystem
{
    Expected<SharedPointer<IFile>, String> OpenFile(StringView filename, FileConstants::OpenMode openMode);
    Expected<uint64, String> GetFileSize(StringView filename);
    bool FileExists(StringView filename);
    Expected<Vector<char>, String> ReadFile(StringView filename);
    void WriteFile(StringView filename, const Vector<char>& _data);
    void WriteFile(StringView filename, StringView _data);

    bool CreateDirectories(StringView _path);
    
    StringView GetFileBaseName(StringView filename);
    StringView GetFileName(StringView filename);
    StringView GetFileDirectory(StringView filename);
    StringView GetFileExtension(StringView filename);
    String GetRelativePath(StringView filename);
    String GetAbsolutePath(StringView filename);
    String GetExecutablePath();

    String GetFileCreationTime(StringView filename);
    String GetFileLastAccessTime(StringView filename);
    String GetFileLastWriteTime(StringView filename);
    bool GetFileTimes(StringView filename, String& _creation, String& _access, String& _write);

    void SetCurrentPath(StringView _directory);
    namespace Windows
    {
        String OpenFileDialog(const char* filter = "All Files (*.*)\0*.*\0", void* owner = NULL);
        String OpenDirectoryDialog(StringView title = "Browse director", void* owner = NULL);
    }
};

}
