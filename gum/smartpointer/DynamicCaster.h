#pragma once

#include <gum/smartpointer/SmartpointerTraits.h>
#include <gum/DynamicCaster.h>

namespace gum {

    namespace detail {

        template < typename From_ >
        class DynamicCasterImpl<From_, std::enable_if_t<IsPtrSmartpointer<From_>::value>> {
            From_ _from;

        public:
            DynamicCasterImpl(From_ from)
                :   _from(from)
            { }

            template < typename To_ >
            operator To_ () const {
                if (auto const p = dynamic_cast<typename To_::pointer>(_from.get()))
                    return To_(_from, p);
                return nullptr;
            }
        };


        template < typename From_ >
        class DynamicCasterImpl<From_, std::enable_if_t<IsReferenceSmartpointer<From_>::value>> {
            From_ _from;

        public:
            DynamicCasterImpl(From_ from)
                :   _from(from)
            { }

            template < typename To_ >
            operator To_ () const {
                auto const p = dynamic_cast<typename To_::pointer>(_from.get());

                using CastException = InvalidCastException<typename From_::value_type, typename To_::value_type>;
                GUM_CHECK(p, CastException(*_from));

                return To_(_from, p);
            }
        };

    }

}
