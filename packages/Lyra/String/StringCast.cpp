
#include <String/StringCast.h>
#include <String/StringUtils.h>

// Created on 2019-03-15 by fmacias

namespace lyra
{
namespace StringCast
{
    String ToString(const bool& value)
    {
        return value ? "true" : "false";
    }

    String ToString(const int& value)
    {
        return std::to_string(value);
    }

    String ToString(const unsigned int& value)
    {
        return std::to_string(value);
    }

    String ToString(const float& value)
    {
        return std::to_string(value);
    }

    String ToString(const String& value)
    {
        return value;
    }

    String ToString(const Math::Vector3f& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f)", value.x, value.y, value.z);
    }

    String ToString(const Math::Vector4f& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    String ToString(const Math::Quaternion& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    // From string

    template <>
    bool FromString(const String& valueStr)
    {
        return valueStr == "true";
    }

    template <>
    int FromString(const String& valueStr)
    {
        return std::stoi(valueStr);
    }

    template <>
    unsigned int FromString(const String& valueStr)
    {
        return std::stoul(valueStr);
    }

    template <>
    float FromString(const String& valueStr)
    {
        return std::stof(valueStr);
    }

    template <>
    Math::Vector3f FromString<Math::Vector3f>(const String& valueStr)
    {
        Math::Vector3f ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f)", &ret.x, &ret.y, &ret.z);
        return ret;
    }

    template <>
    Math::Vector4f FromString<Math::Vector4f>(const String& valueStr)
    {
        Math::Vector4f ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    Math::Quaternion FromString<Math::Quaternion>(const String& valueStr)
    {
        Math::Quaternion ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    String FromString(const String& valueStr)
    {
        return valueStr;
    }
}
}

