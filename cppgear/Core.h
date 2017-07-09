/*
 * Copyright (c) Vladimir Golubev
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

#include <string>
#include <type_traits>

namespace cppgear {

#   define SelfType \
        std::remove_reference<decltype(self)>::type

#   define self \
        (*this)

#   define const_self \
        (static_cast<typename SelfType const&>(self))

#   define const_this \
        (&const_self)

#   define derived_self \
        (static_cast<typename std::conditional \
                        <std::is_const<typename SelfType>::value, \
                         Derived_ const&, \
                         Derived_& \
                        >::type \
                    >(self))


#if __GNUC__ >= 3 || defined(__clang__)
#	define CPPGEAR_FUNCTION __PRETTY_FUNCTION__
#else
#	define CPPGEAR_FUNCTION __func__
#endif


#if defined(__GNUC__) || defined(__clang__)
#	define CPPGEAR_LIKELY(x) __builtin_expect((x), 1)
#	define CPPGEAR_UNLIKELY(x) __builtin_expect((x), 0)
#else
#	define CPPGEAR_LIKELY(x) (x)
#	define CPPGEAR_UNLIKELY(x) (x)
#endif


    namespace detail {

        class Where {
            char const*   _file;
            size_t        _line;
            char const*   _function;

        public:
            Where(char const* file, size_t line, char const* function);

            std::string ToString() const;
        };

    }


#   define CPPGEAR_WHERE \
    cppgear::detail::Where(__FILE__, __LINE__, CPPGEAR_FUNCTION)

}
