
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

    String ToString(const glm::vec3& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f)", value.x, value.y, value.z);
    }

    String ToString(const glm::vec4& value)
    {
        return StringUtils::StringFormat("(%f, %f, %f, %f)", value.x, value.y, value.z, value.w);
    }

    String ToString(const glm::quat& value)
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
    glm::vec3 FromString<glm::vec3>(const String& valueStr)
    {
        glm::vec3 ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f)", &ret.x, &ret.y, &ret.z);
        return ret;
    }

    template <>
    glm::vec4 FromString<glm::vec4>(const String& valueStr)
    {
        glm::vec4 ret;
        sscanf_s(valueStr.c_str(), "(%f, %f, %f, %f)", &ret.x, &ret.y, &ret.z, &ret.w);
        return ret;
    }

    template <>
    glm::quat FromString<glm::quat>(const String& valueStr)
    {
        glm::quat ret;
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

