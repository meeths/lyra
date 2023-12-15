#pragma once
#include <Core/Types.h>
#include <Filesystem/FileConstants.h>
#include <String/String.h>

namespace lyra
{
class IFile;

namespace FileSystem
{
    SharedPointer<IFile> OpenFile(StringView filename, FileConstants::OpenMode openMode);
    unsigned int GetFileSize(StringView filename);
    bool FileExists(StringView filename);
    Vector<char> ReadFile(StringView filename);
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
