
#pragma once
#include <String/String.h>

// Created on 2021-11-21 by Sisco

namespace lyra
{

class Hashing
{
public:
    using Hash = uint32_t;
    static constexpr uint32_t kDefaultHashSeed = 0x9747b28c;
    
    class IHasher
    {
    public:
        virtual ~IHasher() = default;
        virtual void Concatenate(const void* _data, int _len) = 0;
        virtual Hash Get() const = 0;

        template <typename T>
        void Concatenate(T& _data){ Concatenate(&_data, static_cast<int>(sizeof(T))); }
        
        operator Hash() const { return Get(); };
    };
    
    class FNVHasher : public IHasher
    {
    public:
        FNVHasher(const void* _data, int _len);
        void Concatenate(const void* _data, int _len) override;
        Hash Get() const override { return mHash; };
    private:
        Hash mHash;
        
        
    };

    using DefaultHasher = FNVHasher;
};

template <>
inline void Hashing::IHasher::Concatenate<String>(String& _data){ Concatenate(_data.c_str(), static_cast<int>(_data.length())); }

}

