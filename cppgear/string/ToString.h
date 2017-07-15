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

#include <cppgear/metaprogramming/MethodDetector.h>
#include <cppgear/string/String.h>

#include <sstream>
#include <type_traits>

namespace cppgear {

    CPPGEAR_DECLARE_METHOD_DETECTOR(to_string);


    inline String to_string(String str) {
        return str;
    }

    inline String to_string(char const* str) {
        return String(str);
    }

    template < typename Value_ >
    std::enable_if_t<HasMethod_to_string<std::decay_t<Value_>>::value, String> to_string(Value_&& value) {
        return std::forward<Value_>(value).to_string();
    }

    template < typename Value_ >
    std::enable_if_t<std::is_fundamental<Value_>::value, String> to_string(Value_ value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    inline String to_string(std::exception const& ex) {
        return ex.what();
    }

    template < typename First_, typename Second_ >
    String to_string(std::pair<First_, Second_> const& pair) {
        return String() << "{ " << pair.first << ", " << pair.second << " }";
    }

    template < typename Value_ >
    String& operator<<(String& string, Value_&& object) {
        return string << to_string(std::forward<Value_>(object));
    }

}
