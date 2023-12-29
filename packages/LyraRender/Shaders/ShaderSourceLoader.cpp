#include <algorithm>
#include <Shaders/ShaderSourceLoader.h>
#include <String/StringUtils.h>
#include <Filesystem/FileSystem.h>
#include <fstream>
#include <iostream>
#include <Log/Log.h>

namespace details
{
    constexpr lyra::StringView kIncludeIdentifier = "#include ";
}

namespace lyra
{
    Expected<String, String> ShaderSourceLoader::LoadShaderSource(StringView path)
    {

        String fullSourceCode;
        std::ifstream file(path.data());

        if (!file.is_open())
        {
            Log::Instance().LogError("LoadShaderSource: could not open the file: %s", path.data());
            return Unexpected{"#error Could not open file " + String(path)};
        }

        String currentLine;
        while (std::getline(file, currentLine))
        {
            if (currentLine.find(details::kIncludeIdentifier) != currentLine.npos)
            {
                currentLine.erase(0, details::kIncludeIdentifier.size());

                String fileDirectory = String(FileSystem::GetFileDirectory(path));
                currentLine = StringUtils::Trim(StringUtils::Replace(currentLine, "\"", " ")); 

                auto includeSource = LoadShaderSource(fileDirectory + "/" + currentLine);
                fullSourceCode += includeSource.value_or(includeSource.error());
                if(!includeSource.has_value())
                    return Unexpected(fullSourceCode);
            }
            else
            {
                fullSourceCode += currentLine + '\n';
            }
        }
        
        file.close();

        return fullSourceCode;
    
    };    
}
