
#pragma once
#include <String/String.h>
#include <Shaders/ShaderConstants.h>
#include <Shaders/ShaderFlags.h>
#include <Containers/Vector.h>
#include <Core/Hashing.h>

// Created on 2021-11-21 by Sisco

namespace lyra
{

class ShaderProgram
{
public:
    ShaderProgram() = default;
    ShaderProgram(ShaderConstants::ShaderType _type, const StringView _filename, const StringView _entryPoint);
    void AddFlag(ShaderFlag _flag);
    void AddDefine(StringView _define);
    void AddDefine(StringView _define, StringView _value);

    ShaderConstants::ShaderType GetType() const { return mType; };
    const String& GetFilename() const { return mFilename; }
    const String& GetEntryPoint() const { return mEntryPoint; }
    const Vector<std::pair<StringView, StringView>>& GetDefines() const { return mDefines; }

    Hashing::Hash GetHash();
private:
    ShaderConstants::ShaderType mType = ShaderConstants::ShaderType::InvalidType;
    String mFilename;
    String mEntryPoint;
    Vector<std::pair<StringView, StringView>> mDefines;

    bool mDirtyHash = true;
    Hashing::Hash mHash = {}; 
};
}

namespace std
{

    template <>
    struct hash<lyra::ShaderProgram>
    {
        std::size_t operator()( lyra::ShaderProgram& k) const noexcept
        {
            return static_cast<size_t>(k.GetHash());
        }
    };

}

