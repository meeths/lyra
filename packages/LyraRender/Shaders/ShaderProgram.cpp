
#include <Shaders/ShaderProgram.h>

// Created on 2021-11-21 by Sisco

namespace details
{
    lyra::Vector<lyra::StringView> kFlagToDefine =
    {
        "USE_VERTEX_COLOR"
    };
}

namespace lyra
{
    ShaderProgram::ShaderProgram(ShaderConstants::ShaderType _type, const StringView _filename, const StringView _entryPoint) :
    mType(_type), mFilename(_filename), mEntryPoint(_entryPoint), mDirtyHash(true)
    {
    }
    //----------------------------------------------------------------
    void ShaderProgram::AddFlag(ShaderFlag _flag)
    {
        AddDefine(details::kFlagToDefine[static_cast<size_t>(_flag)]);
    }

    void ShaderProgram::AddDefine(StringView _define)
    {
        AddDefine(_define, StringView(""));
    }

    void ShaderProgram::AddDefine(StringView _define, StringView _value)
    {
        mDefines.push_back({_define, _value});
        mDirtyHash = true;
    }

    Hashing::Hash ShaderProgram::GetHash()
    {
        if(mDirtyHash)
        {
            Hashing::DefaultHasher hasher(&mType, sizeof(mType));
            hasher.Concatenate(mFilename.c_str(), static_cast<int>(mFilename.length()));
            hasher.Concatenate(mEntryPoint.c_str(), static_cast<int>(mEntryPoint.length()));
            for (auto& [def, val] : mDefines)
            {
                hasher.Concatenate(def.data(), static_cast<int>(def.length()));
                if(val.length() > 0)
                    hasher.Concatenate(val.data(), static_cast<int>(val.length()));
            }
            mHash = hasher;
            mDirtyHash = false;
        }
        return mHash;
    }
}

