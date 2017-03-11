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
        struct CallChaining {
            template < typename Callable_, typename ...Args_, typename Result_ = typename std::result_of<Callable_(Args_&&...)>::type >
            std::enable_if_t<std::is_void<Result_>::value, ElseType_>
            operator()(Callable_&& callable, Args_&& ...args) {
                callable(std::forward<Args_>(args)...);
                return ElseType_();
            }

            template < typename Callable_, typename ...Args_, typename Result_ = typename std::result_of<Callable_(Args_&&...)>::type >
            std::enable_if_t<!std::is_void<Result_>::value, Result_>
            operator()(Callable_&& callable, Args_&& ...args) {
                return callable(std::forward<Args_>(args)...);
            }
        };

        template < typename Value_ >
        using DereferenceResult = std::remove_reference_t<decltype(*std::declval<Value_>())>;

        template < typename Value_ >
        struct IsMaybe {
            static constexpr bool Value = false;
        };

        template < typename Value_ >
        struct IsMaybe<Maybe<Value_>> {
            static constexpr bool Value = true;
        };

        template < typename Value_ >
        class ToMaybeImpl {
            using Naked = std::remove_const_t<std::remove_reference_t<Value_>>;

        public:
            using Type = std::conditional_t<IsMaybe<Naked>::Value, Naked, Maybe<Naked>>;
        };

        template < typename Value_ >
        using ToMaybe = typename ToMaybeImpl<Value_>::Type;

    }

    template < typename Wrapped_ >
    class Maybe {
    public:
        using Wrapped = Wrapped_;
        using Value = detail::DereferenceResult<Wrapped_>;

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

        template < typename ValueType_, typename Maybe_ = detail::ToMaybe<ValueType_> >
        Maybe_ chain(ValueType_&& other) {
            return std::forward<ValueType_>(other);
        }

        template < typename Callable_ >
        auto and_bind(Callable_&& callable) {
            using Chaining = detail::CallChaining<Maybe>;
            using Result = std::result_of_t<Chaining(Callable_&&, Wrapped&)>;
            using MaybeType = detail::ToMaybe<Result>;

            return m_wrapped ? MaybeType(Chaining()(callable, *m_wrapped)) : MaybeType();
        }

        template < typename Callable_ >
        auto or_bind(Callable_&& callable) {
            using Chaining = detail::CallChaining<Maybe>;

            return m_wrapped ? std::move(self) : Maybe(Chaining()(callable));
        }

        Value& unwrap() {
            if (m_wrapped) {
                return *m_wrapped;
            }
            throw EmptyMaybeException();
        }

        Wrapped take() {
            return std::move(m_wrapped);
        }

    private:
        Wrapped m_wrapped;
    };

    template < typename Wrapped_, typename Maybe_ = detail::ToMaybe<Wrapped_> >
    Maybe_ maybe(Wrapped_&& value) {
        return std::forward<Wrapped_>(value);
    }

}
