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
            using type = std::remove_reference_t<decltype(*std::declval<Value_>())>;
        };

    }

    template < typename Value_ >
    class Maybe {
    public:
        using value_type = Value_;
        using wrapped_type = typename detail::DereferenceType<Value_>::type;

    public:
        Maybe() { }

        template < typename ValueType_ >
        Maybe(ValueType_&& value) :
            m_value(std::forward<ValueType_>(value)) { }

        template < typename Maybe_ >
        Maybe and_(Maybe_&& other, std::enable_if_t<std::is_same<value_type, typename Maybe_::value_type>::value>* = 0) {
            return m_value ? std::forward<Maybe_>(other) : Maybe(m_value);
        }

        template < typename Maybe_ >
        Maybe or_(Maybe_&& other, std::enable_if_t<std::is_same<value_type, typename Maybe_::value_type>::value>* = 0) {
            return m_value ? Maybe(m_value) : std::forward<Maybe_>(other);
        }

        template < typename Maybe_ >
        Maybe_ and_(Maybe_&& other, std::enable_if_t<!std::is_same<value_type, typename Maybe_::value_type>::value>* = 0) {
            return m_value ? std::forward<Maybe_>(other) : Maybe_();
        }

        template < typename Maybe_ >
        Maybe_ or_(Maybe_&& other, std::enable_if_t<!std::is_same<value_type, typename Maybe_::value_type>::value>* = 0) {
            return m_value ? Maybe_() : std::forward<Maybe_>(other);
        }

        template < typename Callable_ >
        Maybe and_bind(Callable_&& callable) {
            return m_value ? value_type(detail::ChainingHelper<value_type>()(callable, *m_value)) : m_value;
        }

        template < typename Callable_ >
        Maybe or_bind(Callable_&& callable) {
            return m_value ? m_value : value_type(detail::ChainingHelper<value_type>()(callable));
        }

        template < typename Default_ >
        wrapped_type unwrap_or(Default_&& default_) {
            return m_value ? *m_value : wrapped_type(std::forward<Default_>(default_));
        }

        wrapped_type& unwrap() {
            if (m_value) {
                return *m_value;
            }
            throw EmptyMaybeException();
        }

        operator value_type() const {
            return m_value;
        }

    private:
        value_type m_value;
    };

    template < typename Value_, typename Maybe_ = Maybe<typename std::remove_reference<Value_>::type> >
    Maybe_ maybe(Value_&& value) {
        return Maybe_(std::forward<Value_>(value));
    }

}
