
#pragma once
#include <map>

// Created on 2023-12-15 by sisco

namespace lyra
{
    template <typename... Ts>
    using Multimap = std::multimap<Ts...>;
}

