
#pragma once

// Created on 2019-03-15 by fmacias

#include <String/String.h>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/quaternion.hpp>
#include <Core/CoreDefines.h>

namespace lyra
{

namespace StringCast
{
    template <typename T>
    String ToString(const T& value);

    // template specializations

    String ToString(const bool& value);
    String ToString(const int& value);
    String ToString(const unsigned int& value);
    String ToString(const float& value);
    String ToString(const String& value);

    String ToString(const glm::vec3& value);
    String ToString(const glm::vec4& value);
    String ToString(const glm::quat& value);


    template <typename T>
    T FromString(const String& value);

    template <>
    bool FromString(const String& valueStr);

    template <>
    int FromString(const String& valueStr);

    template <>
    unsigned int FromString(const String& valueStr);

    template <>
    float FromString(const String& valueStr);

    template <>
    glm::vec3 FromString(const String& valueStr);
    template <>
    glm::vec4 FromString(const String& valueStr);
    template <>
    glm::quat FromString(const String& valueStr);

    template <>
    String FromString(const String& valueStr);


    // return empty string if no conversion possible
    template <typename T>
    String ToString(const T& /* value */)
    {
        // Not convertible to string (no suitable ToString override) OR
        // Basic type not serializable (no jsonserializer BasicSerializer override) OR
        // Or compound type not meta-described via meta::registerMembers
        // Maybe forgot to run Component code generation script?
        lyraAssert(0);
        return String("##TOSTRING_NOT_IMPLEMENTED##");
    }

    template <typename T>
    T FromString(const String& /* value */)
    {
        return T();
    }
};

}

