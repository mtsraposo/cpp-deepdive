#pragma once

#include "types.h"
#include <limits>

namespace std {
    template<>
    class numeric_limits<LimitedInt> {
    public:
        static constexpr bool is_specialized = true;

        static LimitedInt min() noexcept { return LimitedInt(numeric_limits<int>::min()); }

        static LimitedInt max() noexcept { return LimitedInt(numeric_limits<int>::max()); }

        static LimitedInt lowest() noexcept { return LimitedInt(numeric_limits<int>::lowest()); }
    };
}