
#pragma once

// Created on 2019-03-15 by fmacias

#include <String/String.h>

#include <Math/Vector3f.h>
#include <Math/Vector4f.h>
#include <Math/Quaternion.h>
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

    String ToString(const Math::Vector3f& value);
    String ToString(const Math::Vector4f& value);
    String ToString(const Math::Quaternion& value);


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
    Math::Vector3f FromString(const String& valueStr);
    template <>
    Math::Vector4f FromString(const String& valueStr);
    template <>
    Math::Quaternion FromString(const String& valueStr);

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

