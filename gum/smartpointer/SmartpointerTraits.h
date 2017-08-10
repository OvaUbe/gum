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

#include <gum/smartpointer/SharedPtr.h>
#include <gum/smartpointer/SharedReference.h>
#include <gum/smartpointer/UniquePtr.h>
#include <gum/smartpointer/UniqueReference.h>

namespace gum {

    template < typename Type_ >
    struct IsSharedPtr {
        static constexpr bool value = false;
    };

    template < typename Type_ >
    struct IsSharedPtr<SharedPtr<Type_>> {
        static constexpr bool value = true;
    };

    template < typename Type_ >
    struct IsSharedReference {
        static constexpr bool value = false;
    };

    template < typename Type_ >
    struct IsSharedReference<SharedReference<Type_>> {
        static constexpr bool value = true;
    };

    template < typename Type_ >
    struct IsUniquePtr {
        static constexpr bool value = false;
    };

    template < typename Type_ >
    struct IsUniquePtr<UniquePtr<Type_>> {
        static constexpr bool value = true;
    };

    template < typename Type_ >
    struct IsUniqueReference {
        static constexpr bool value = false;
    };

    template < typename Type_ >
    struct IsUniqueReference<UniqueReference<Type_>> {
        static constexpr bool value = true;
    };


    template < typename Type_ >
    struct IsSharedSmartpointer {
        static constexpr bool value = IsSharedPtr<Type_>::value || IsSharedReference<Type_>::value;
    };

    template < typename Type_ >
    struct IsUniqueSmartpointer {
        static constexpr bool value = IsUniquePtr<Type_>::value || IsUniqueReference<Type_>::value;
    };


    template < typename Type_ >
    struct IsPtrSmartpointer {
        static constexpr bool value = IsSharedPtr<Type_>::value || IsUniquePtr<Type_>::value;
    };

    template < typename Type_ >
    struct IsReferenceSmartpointer {
        static constexpr bool value = IsSharedReference<Type_>::value || IsUniqueReference<Type_>::value;
    };


    template < typename Type_ >
    struct IsSmartpointer {
        static constexpr bool value = IsSharedSmartpointer<Type_>::value || IsUniqueSmartpointer<Type_>::value;
    };

}
