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

#include <cppgear/Core.h>

#include <stdexcept>

namespace cppgear {

    struct Exception : public std::runtime_error {
        Exception(const std::string& message)
            : std::runtime_error(message)
        { }
    };


#   define CPPGEAR_DECLARE_EXCEPTION(Type_, DefaultMessage_) \
        struct Type_ : public Exception { \
            Type_() \
                :   Exception(DefaultMessage_) \
            { } \
            \
            Type_(const std::string& message) \
                :   Exception(std::string(DefaultMessage_) + ": " + message) \
            { } \
        }


    CPPGEAR_DECLARE_EXCEPTION(NullPointerException, "Accessing null pointer");


    namespace detail {

        template < typename Exception_ >
        inline Exception make_exception(Exception_&& ex) {
            return std::forward<Exception_>(ex);
        }

        inline Exception make_exception(const std::string& message) {
            return Exception(message);
        }

    }


#   define CPPGEAR_THROW(Ex_) \
        throw cppgear::detail::make_exception(Ex_)

#   define CPPGEAR_CHECK(Condition_, Otherwise_) \
        if (CPPGEAR_UNLIKELY(!Condition_)) CPPGEAR_THROW(Otherwise_)

}
