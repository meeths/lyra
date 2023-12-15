
#pragma once

// Created on 2019-03-18 by sisco
#include <String/String.h>
#include <cstdarg>
#include <Core/Types.h>
#include <Core/CoreDefines.h>

namespace lyra
{

namespace StringUtils
{
    inline String StringFormat(const char* _format, ...)
    {
        va_list args;
        va_start(args, _format);

        int size = _vscprintf(_format, args) + 1;
        //dafuq is dis
        Vector<char> buf;
        buf.resize(size);

#ifndef _MSC_VER
        vsnprintf(buf.data(), buf.size(), _format, args);
#else
        vsnprintf_s(buf.data(), buf.size(), _TRUNCATE, _format, args);
#endif

        va_end(args);

        return String(buf.data());
    }

	String ToLower(const String& _string);
	String ToUpper(const String& _string);

    bool FuzzyMatch(const char* _pattern, const char* _string, int& _score);
    bool FuzzyMatch(const char* _pattern, const char* _string);

    inline String LTrim(const String &s)
    {
        size_t start = s.find_first_not_of(" \n\r\t\f\v");
        return (start == String::npos) ? "" : s.substr(start);
    }
 
    inline String RTrim(const String &s)
    {
        size_t end = s.find_last_not_of(" \n\r\t\f\v");
        return (end == String::npos) ? "" : s.substr(0, end + 1);
    }
 
    inline String Trim(const String &s) {
        return RTrim(LTrim(s));
    }

    inline String Replace(String str, const String& from, const String& to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != String::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
    
    template<std::size_t SIZE>
    std::array<char, SIZE> ToArray(const String& _src)
    {
        lyraAssert(_src.size() + 1 <= SIZE);
        std::array<char, SIZE> dst;
        std::copy(_src.begin(), _src.end(), dst.data());
        dst[_src.size()] = 0; // null terminator
        return dst;
    }

    inline std::vector<char> ToVector(const String& _src)
    {
        std::vector<char> strVector(_src.begin(), _src.end());
        return strVector;
    }
};

}
