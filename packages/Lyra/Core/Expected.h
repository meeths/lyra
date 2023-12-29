
#pragma once

// Created on 2023-12-29 by sisco
#include <Core/StdImpl/tlExpected.h>

namespace lyra
{

    template<class T, class E>
    using Expected = tl::expected<T, E>;
    template<class T>
    using Unexpected = tl::unexpected<T>;

}

