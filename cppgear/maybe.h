/*
 * Copyright (c) 2016 Vladimir Golubev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <exception>
#include <utility>

namespace cppgear {

    class EmptyMaybeException : public std::exception {
        const char* what() const noexcept override {
            return "Empty maybe!";
        }
    };

    template < typename >
    class Maybe;

    namespace detail {

        template < typename ElseType_ >
        struct ChainingHelper {
            template < typename Callable_, typename ...Args_, typename Result_ = typename std::result_of<Callable_(Args_&&...)>::type >
            typename std::enable_if<std::is_void<Result_>::value, ElseType_>::type
            operator()(Callable_&& callable, Args_&& ...args) {
                callable(std::forward<Args_>(args)...);
                return ElseType_();
            }

            template < typename Callable_, typename ...Args_, typename Result_ = typename std::result_of<Callable_(Args_&&...)>::type >
            typename std::enable_if<!std::is_void<Result_>::value, Result_>::type
            operator()(Callable_&& callable, Args_&& ...args) {
                return callable(std::forward<Args_>(args)...);
            }
        };

        template < typename Value_ >
        struct DereferenceType {
            using Type = std::remove_reference_t<decltype(*std::declval<Value_>())>;
        };

        template < typename Value_ >
        struct IsMaybe {
            static constexpr bool Value = false;
        };

        template < typename Value_ >
        struct IsMaybe<Maybe<Value_>> {
            static constexpr bool Value = true;
        };

        template < typename Value_ >
        class ToMaybe {
            using Raw = std::remove_const_t<std::remove_reference_t<Value_>>;

        public:
            using Type = std::conditional_t<IsMaybe<Raw>::Value, Raw, Maybe<Raw>>;
        };

    }

    template < typename Value_ >
    class Maybe {
    public:
        using wrapped_type = Value_;
        using value_type = typename detail::DereferenceType<Value_>::Type;

    public:
        Maybe() { }

        template < typename ValueType_ >
        Maybe(ValueType_&& value) :
            m_wrapped(std::forward<ValueType_>(value)) { }

        template < typename ValueType_ >
        auto and_(ValueType_ other) {
            return and_bind([&](auto){ return other; });
        }

        template < typename ValueType_ >
        auto or_(ValueType_ other) {
            return or_bind([&]{ return other; });
        }

        template < typename ValueType_, typename Maybe_ = typename detail::ToMaybe<ValueType_>::Type >
        Maybe_ chain(ValueType_&& other) {
            return std::forward<ValueType_>(other);
        }

        template < typename Callable_ >
        auto and_bind(Callable_&& callable) {
            using Chaining = detail::ChainingHelper<Maybe>;
            using Result = std::result_of_t<Chaining(Callable_&&, wrapped_type&)>;
            using Maybe_ = typename detail::ToMaybe<Result>::Type;

            return m_wrapped ? Maybe_(Chaining()(callable, *m_wrapped)) : Maybe_();
        }

        template < typename Callable_ >
        auto or_bind(Callable_&& callable) {
            using Chaining = detail::ChainingHelper<Maybe>;

            return m_wrapped ? std::move(self) : Maybe(Chaining()(callable));
        }

        wrapped_type& unwrap() {
            if (m_wrapped) {
                return *m_wrapped;
            }
            throw EmptyMaybeException();
        }

        wrapped_type take() {
            return std::move(m_wrapped);
        }

    private:
        wrapped_type m_wrapped;
    };

    template < typename Value_, typename Maybe_ = typename detail::ToMaybe<Value_>::Type >
    Maybe_ maybe(Value_&& value) {
        return std::forward<Value_>(value);
    }

}
