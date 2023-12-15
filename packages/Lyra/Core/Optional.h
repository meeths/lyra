
#pragma once
#include <optional>

// Created on 2023-12-15 by sisco

namespace lyra
{
    template<typename... Ts>
    using Optional = std::optional<Ts...>;
}

