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

namespace gum {

#if __GNUC__ >= 3 || defined(__clang__)
#	define GUM_FUNCTION __PRETTY_FUNCTION__
#else
#	define GUM_FUNCTION __func__
#endif


#if defined(__GNUC__) || defined(__clang__)
#	define GUM_LIKELY(x) __builtin_expect((x), 1)
#	define GUM_UNLIKELY(x) __builtin_expect((x), 0)
#else
#	define GUM_LIKELY(x) (x)
#	define GUM_UNLIKELY(x) (x)
#endif


    namespace detail {

        class Where {
            char const*   _file;
            size_t        _line;
            char const*   _function;

        public:
            Where(char const* file, size_t line, char const* function);

            std::string to_string() const;
        };

    }


#   define GUM_WHERE \
    gum::detail::Where(__FILE__, __LINE__, GUM_FUNCTION)

}
