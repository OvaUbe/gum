#pragma once

#include <gum/exception/Exception.h>

namespace gum {

namespace detail {

template <typename From_, typename Enabler_ = void>
class DynamicCasterImpl;
}

template <typename From_>
auto dynamic_caster(From_&& from) {
    return detail::DynamicCasterImpl<From_>(std::forward<From_>(from));
}

namespace detail {

template <typename From_>
class DynamicCasterImpl<From_*> {
    From_* _from;

  public:
    DynamicCasterImpl(From_* from)
        : _from(from) {}

    template <typename To_>
    operator To_*() const {
        return dynamic_cast<To_*>(_from);
    }
};

template <typename From_>
class DynamicCasterImpl<From_&> {
    From_* _from;

  public:
    DynamicCasterImpl(From_& from)
        : _from(&from) {}

    template <typename To_>
    operator To_&() const {
        To_* to = dynamic_cast<To_*>(_from);

        using CastException = InvalidCastException<From_, To_>;
        GUM_CHECK(to, CastException(*_from));

        return *to;
    }
};
}
}
