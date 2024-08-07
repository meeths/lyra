#include <algorithm>
#include <Shaders/ShaderSourceLoader.h>
#include <String/StringUtils.h>
#include <Filesystem/FileSystem.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <Log/Log.h>

namespace details
{
    constexpr lyra::StringView kIncludeIdentifier = "#include ";
}

namespace lyra
{
    Expected<String, String> ShaderSourceLoader::LoadShaderSource(StringView path)
    {

        const auto expectedFileContents = FileSystem::ReadFile(path);

        if (!expectedFileContents)
        {
            Log::Instance().LogError("LoadShaderSource: could not open the file: %s", path.data());
            return Unexpected<String>{"#error Could not open file " + String(path)};
        }

        auto fileContents = StringUtils::FromVector(*expectedFileContents);
        std::stringstream stringStream(fileContents);
        String currentLine;
        String fullSourceCode;
        while (std::getline(stringStream, currentLine))
        {
            if (currentLine.find(details::kIncludeIdentifier) != String::npos)
            {
                currentLine.erase(0, details::kIncludeIdentifier.size());

                String fileDirectory = String(FileSystem::GetFileDirectory(path));
                currentLine = StringUtils::Trim(StringUtils::Replace(currentLine, "\"", " ")); 

                auto includeSource = LoadShaderSource(fileDirectory + "/" + currentLine);
                fullSourceCode += includeSource.value_or(includeSource.error());
                if(!includeSource.has_value())
                    return Unexpected<String>(fullSourceCode);
            }
            else
            {
                fullSourceCode += currentLine + '\n';
            }
        }
        
        return fullSourceCode;
    
    };    
}
