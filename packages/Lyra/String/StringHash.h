#pragma once
#include <Core/Hash/CE_MurmurHash3.h>
#include <String/String.h>

// Need integer overflows for this
// Need these warnings disabled for constexpr eval
#pragma warning( disable: 4307 ) 

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
        :mHash(murmur::StaticHashValueInternal32(_string, StringView(_string).length(), defaultHashSeed))
        {};
        constexpr ConstStringHash(const StringView _string)
        :mHash(murmur::StaticHashValueInternal32(_string.data(), _string.length(), defaultHashSeed))
        {};
        constexpr ConstStringHash(const StringHashType _id) : mHash(_id) {}
        constexpr ConstStringHash(const ConstStringHash& _other) : mHash(_other.mHash){};

        operator StringHash() const;

        friend bool operator ==(const ConstStringHash& a, const StringHash& b);
    protected:

        const StringHashType mHash;
    };
    
    class StringHash
    {
    public:
        friend class ConstStringHash;
        
        StringHash(const char* _string);
        constexpr StringHash(ConstStringHash _constStringHash) { mId = _constStringHash.mHash; };
        constexpr StringHash(const StringView _view) : StringHash(ConstStringHash(_view))
        {
#ifdef _DEBUG
            _debugOriginalString = _view;
#endif
        };
        StringHash(const String& _string);
        StringHash(const StringHashType _id) : mId(_id){}
        
        friend bool operator ==(const StringHash& a, const StringHash& b)
        {
            return a.mId == b.mId;
        }

        friend bool operator !=(const StringHash& a, const StringHash& b)
        {
            return a.mId != b.mId;
        }

        StringHashType GetHash() const { return mId; }

#ifdef _DEBUG
        const StringView& GetOriginalString() const { return _debugOriginalString; };
#endif
    private:
        constexpr StringHash() = default;

        StringHashType mId = 0;
#ifdef _DEBUG
        StringView _debugOriginalString = "-empty-";
#endif
    };
}

namespace std {

  template <>
  struct hash<lyra::StringHash>
  {
    std::size_t operator()(const lyra::StringHash& k) const noexcept
    {
        return static_cast<size_t>(k.GetHash());
    }
  };

}

