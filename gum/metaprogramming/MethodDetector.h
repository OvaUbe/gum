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

#include <type_traits>

namespace gum {

#   define GUM_DECLARE_METHOD_DETECTOR(Method_) \
        template < typename T > \
        class HasMethod_##Method_ { \
            template < typename Type_ > \
            class Impl { \
                typedef char (&YesType)[1]; \
                typedef char (&NoType)[2]; \
                \
                struct BaseMixin { void Method_() { } }; \
                struct Base : public Type_, public BaseMixin { Base(); }; \
                \
                template < typename V, V t > class Helper { }; \
                \
                template < typename U > \
                static NoType deduce(U*, Helper<void (BaseMixin::*)(), &U::Method_>* = 0); \
                static YesType deduce(...); \
                \
            public: \
                static constexpr bool value = sizeof(YesType) == sizeof(deduce((Base*)0)); \
            }; \
            \
        public: \
            static constexpr bool value = std::conditional_t<std::is_class<T>::value, Impl<T>, std::false_type>::value; \
        }

}
