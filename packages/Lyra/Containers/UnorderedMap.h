
#pragma once
#include <unordered_map>

// Created on 2023-12-15 by sisco

namespace lyra
{
    template<typename... Ts>
    using UnorderedMap = std::unordered_map<Ts...>;
}

