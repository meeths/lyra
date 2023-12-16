#pragma once
#include <Core/Hash/CE_MurmurHash3.h>
#include <String/String.h>

// Need integer overflows for this
// Need these warnings disabled for constexpr eval
#pragma warning( disable: 4307 ) 

#ifdef _DEBUG
#define KEEP_ORIGINAL_STRING
#endif

namespace lyra
{
    class ConstStringHash;

    constexpr uint32_t defaultHashSeed = 0x9747b28c;
    typedef uint32_t StringHashType;

    class ConstStringHash
    {
    public:
        friend class StringHash;
        constexpr ConstStringHash(const char* _string)
        :m_Hash(murmur::StaticHashValueInternal32(_string, StringView(_string).length(), defaultHashSeed))
        {}
        
        constexpr ConstStringHash(const StringView _string)
        :m_Hash(murmur::StaticHashValueInternal32(_string.data(), _string.length(), defaultHashSeed))
        {}
        
        constexpr ConstStringHash(const StringHashType _id) : m_Hash(_id) {}
        constexpr ConstStringHash(const ConstStringHash& _other) = default;

        operator StringHash() const;

        friend bool operator ==(const ConstStringHash& a, const StringHash& b);
    protected:

        const StringHashType m_Hash;
    };
    
    class StringHash
    {
    public:
        friend class ConstStringHash;
        
        StringHash(const char* _string);
        constexpr StringHash(const ConstStringHash& _constStringHash) { m_Id = _constStringHash.m_Hash; };
        constexpr StringHash(const StringView _view) : StringHash(ConstStringHash(_view))
        {
#ifdef KEEP_ORIGINAL_STRING
            _debugOriginalString = _view;
#endif
        };
        StringHash(const String& _string);
        StringHash(const StringHashType _id) : m_Id(_id){}
        
        friend bool operator ==(const StringHash& a, const StringHash& b)
        {
            return a.m_Id == b.m_Id;
        }

        friend bool operator !=(const StringHash& a, const StringHash& b)
        {
            return a.m_Id != b.m_Id;
        }

        StringHashType GetHash() const { return m_Id; }

#ifdef KEEP_ORIGINAL_STRING
        const StringView& GetOriginalString() const { return _debugOriginalString; };
#endif
    private:
        constexpr StringHash() = default;

        StringHashType m_Id = 0;
#ifdef KEEP_ORIGINAL_STRING
        StringView _debugOriginalString = "-empty-";
#endif
    };
}

template <>
struct std::hash<lyra::StringHash>
{
    std::size_t operator()(const lyra::StringHash& k) const noexcept
    {
        return static_cast<size_t>(k.GetHash());
    }
};

