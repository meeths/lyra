#pragma once

#include <vector>
#include <array>
#include <deque>
#include <memory>
#include <set>
#include <map>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>

using Word = unsigned short;
using Byte = unsigned char;
using SByte = char;
using SWord = short;

namespace lyra
{
    template <typename T>
    using Vector = std::vector<T>;

    template <typename T, size_t S>
    using Array = std::array<T, S>;

    template <typename... Ts>
    using Set = std::set<Ts...>;

    template <typename... Ts>
    using Multimap = std::multimap<Ts...>;

    template <typename T>
    using Deque = std::deque<T>;

    template <typename... Ts>
    using SharedPointer = std::shared_ptr<Ts...>;
    #define MakeSharedPointer std::make_shared

    template<typename... Ts>
    using UniquePointer = std::unique_ptr<Ts...>;
    #define MakeUniquePointer std::make_unique

    template<typename... Ts>
    using Map = std::map<Ts...>;

    template<typename... Ts>
    using UnorderedMap = std::unordered_map<Ts...>;

    template<typename... Ts>
    using UnorderedSet = std::unordered_set<Ts...>;

    template<typename... Ts>
    using Reference = std::reference_wrapper<Ts...>;

    template<typename... Ts>
    using Optional = std::optional<Ts...>;
    

}
