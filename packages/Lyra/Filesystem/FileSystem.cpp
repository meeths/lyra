#include <Filesystem/FileSystem.h>
#include <Filesystem/IFile.h>
#include <Filesystem/File.h>
#include <filesystem>

#pragma warning( disable: 4996 ) 

#define WINDOWS_LEAN_AND_MEAN
#include <codecvt>
#include <windows.h>
#include <shlobj_core.h>
#include <String/StringUtils.h>
#include <Filesystem/DiscFileLocator.h>

namespace lyra
{
Expected<SharedPointer<IFile>, String> FileSystem::OpenFile(StringView filename, FileConstants::OpenMode openMode)
{
    auto file = MakeSharedPointer<File>(filename);
    file->Open(openMode);
    if(!file->IsOpen())
    {
        return Unexpected{"Could not open file " + String(filename)};
    }
    return file;
}

Expected<uint64, String> FileSystem::GetFileSize(StringView filename)
{
    if(!FileExists(filename))
    {
        lyraAssert(0 && "Error retrieving size of non existent file.");
        return Unexpected{"Could not open file " + String(filename)};
        
    }
    struct stat buffer{};
    stat(filename.data(), &buffer);
    return buffer.st_size;
}

bool FileSystem::FileExists(StringView filename)
{
    struct stat buffer{};   
    return stat (filename.data(), &buffer) == 0;
}

Expected<Vector<char>, String> FileSystem::ReadFile(StringView filename)
{

    DiscFileLocator locator;
    const auto locatedFile = locator.Locate(filename);
    if(!locatedFile)
    {
        lyraAssert(0 && "File does not exist");
        return Unexpected{"Could not open file " + String(filename)};
    }

    auto expectedFileSize = GetFileSize(filename);
    if(!expectedFileSize)
    {
        return Unexpected(expectedFileSize.error());
    }
    const auto fileSize = *expectedFileSize;
    
    Vector<char> buffer(fileSize);
    const auto& file = locatedFile.value();
    file->Open(static_cast<FileConstants::OpenMode>(FileConstants::OpenMode::Read | FileConstants::OpenMode::Binary));
    file->Read(buffer.data(), fileSize, 1);
    return buffer;
}
void FileSystem::WriteFile(StringView filename, const Vector<char>& _data)
{
    auto file = OpenFile(filename, static_cast<FileConstants::OpenMode>(FileConstants::OpenMode::Write | FileConstants::OpenMode::Binary));
    if(!file)
    {
        lyraAssert(0);
    }
    (*file)->Write((void*)_data.data(), static_cast<unsigned int>(_data.size()), 1);
}
void FileSystem::WriteFile(StringView filename, StringView _data)
{
    auto file = OpenFile(filename, static_cast<FileConstants::OpenMode>(FileConstants::OpenMode::Write | FileConstants::OpenMode::Binary));
    if(!file)
    {
        lyraAssert(0);
    }
    (*file)->Write((void*)_data.data(), static_cast<unsigned int>(_data.size()), 1);
}

bool FileSystem::CreateDirectories(StringView _path)
{
    return std::filesystem::create_directories(_path);
}

StringView FileSystem::GetFileBaseName(StringView filename)
{
    const auto baseName = GetFileName(filename);
    const auto extensionPos = baseName.find_last_of('.');
    return baseName.substr(0, extensionPos);
}

StringView FileSystem::GetFileName(StringView filename)
{
    return filename.substr(filename.find_last_of("\\/") + 1);
}

StringView FileSystem::GetFileDirectory(StringView filename)
{
    return filename.substr(0, filename.find_last_of("\\/"));
}
StringView FileSystem::GetFileExtension(StringView filename)
{
    auto filenameOnly = GetFileName(filename);
    return filenameOnly.substr(filenameOnly.find_last_of('.') + 1);
}

String FileSystem::GetAbsolutePath(StringView filename)
{
    // Absolute from current working folder
    return std::filesystem::absolute(filename).string();
}

String FileSystem::GetExecutablePath()
{
    CHAR path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return String(GetFileDirectory(path));
    
}

String FileSystem::GetFileCreationTime(StringView filename)
{
    String creation, dummy1, dummy2;
    GetFileTimes(filename, creation, dummy1, dummy2);
    return creation;
}

String FileSystem::GetFileLastAccessTime(StringView filename)
{
    String dummy1, access, dummy2;
    GetFileTimes(filename, dummy1, access, dummy2);
    return access;
}

String FileSystem::GetFileLastWriteTime(StringView filename)
{
    String dummy1, dummy2, lastWrite;
    GetFileTimes(filename, dummy1, dummy2, lastWrite);
    return lastWrite;
}

bool FileSystem::GetFileTimes(StringView filename, String& _creation, String& _access,
                              String& _write)
{
    FILETIME ftCreate, ftAccess, ftWrite;
    SYSTEMTIME stUTC, stLocal;

    const HANDLE hFile = CreateFileA( filename.data(), GENERIC_READ, FILE_SHARE_READ, nullptr,
                                      OPEN_EXISTING, 0, nullptr);
    
    if (!GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
        return false;

    {
        FileTimeToSystemTime(&ftCreate, &stUTC);
        SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);
    
        // Build a string showing the date and time.
        _creation = StringUtils::StringFormat( 
            "%02d/%02d/%d  %02d:%02d",
            stLocal.wDay, stLocal.wMonth, stLocal.wYear,
            stLocal.wHour, stLocal.wMinute);
    }

    {
        FileTimeToSystemTime(&ftWrite, &stUTC);
        SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);
    
        // Build a string showing the date and time.
        _write = StringUtils::StringFormat( 
            "%02d/%02d/%d  %02d:%02d",
            stLocal.wDay, stLocal.wMonth, stLocal.wYear,
            stLocal.wHour, stLocal.wMinute);
    }
    {
        FileTimeToSystemTime(&ftAccess, &stUTC);
        SystemTimeToTzSpecificLocalTime(nullptr, &stUTC, &stLocal);
    
        // Build a string showing the date and time.
        _access = StringUtils::StringFormat( 
            "%02d/%02d/%d  %02d:%02d",
            stLocal.wDay, stLocal.wMonth, stLocal.wYear,
            stLocal.wHour, stLocal.wMinute);
    }

    CloseHandle(hFile);    
    return true;
}

void FileSystem::SetCurrentPath(StringView _directory)
{
    std::wstring stemp = std::wstring(_directory.begin(), _directory.end());
    SetCurrentDirectory(stemp.data());
}

String FileSystem::GetRelativePath(StringView filename)
{
    // Convert relative to working folder
    return std::filesystem::relative(filename).string();
}
    
String FileSystem::Windows::OpenFileDialog(const char* _filter, void* _owner)
{
    TCHAR curDir[MAX_PATH];
    DWORD a = GetCurrentDirectory(MAX_PATH, curDir);

    OPENFILENAMEA ofn;
    char fileName[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = (HWND)_owner;
    ofn.lpstrFilter = _filter;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "";
    String fileNameStr;
    if (GetOpenFileNameA(&ofn))
        fileNameStr = fileName;

    SetCurrentDirectory(curDir);
    String relativePath = GetRelativePath(fileNameStr);
    if (!relativePath.empty())
        return relativePath;
    return fileNameStr;
}

String FileSystem::Windows::OpenDirectoryDialog(StringView _title, void* _owner)
{
    BROWSEINFO br;
    ZeroMemory(&br, sizeof(BROWSEINFO));
    br.lpfn = nullptr;

    br.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    br.hwndOwner = (HWND)_owner;
    std::wstring stemp = std::wstring(_title.begin(), _title.end());
    br.lpszTitle = stemp.data();
    
    LPITEMIDLIST pidl = nullptr;
    if ((pidl = SHBrowseForFolder(&br)) != nullptr)
    {
        wchar_t buffer[MAX_PATH];
        if (SHGetPathFromIDList(pidl, buffer))
        {            
            stemp = buffer;

            //setup converter
            
            using convert_type = std::codecvt_utf8<wchar_t>;
            std::wstring_convert<convert_type, wchar_t> converter;
            //use converter (.to_bytes: wstr->str, .from_bytes: str->wstr)
            String converted_str = converter.to_bytes( stemp );
            return converted_str;
        }
    }

    return "";
}
}
