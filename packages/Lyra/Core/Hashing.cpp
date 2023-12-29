
#include <Core/Hashing.h>
#include <Core/hash/Hashes.h>

// Created on 2021-11-21 by Sisco

namespace lyra
{
    Hashing::FNVHasher::FNVHasher(const void* _data, int _len)
    {
        FNV(_data, _len, kDefaultHashSeed, &mHash);
    }

    void Hashing::FNVHasher::Concatenate(const void* _data, int _len)
    {
        FNV_concat(_data, _len, &mHash);
    }
}

