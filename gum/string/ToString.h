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

#include <gum/Optional.h>
#include <gum/container/ContainerTraits.h>
#include <gum/metaprogramming/MethodDetector.h>
#include <gum/smartpointer/SmartpointerTraits.h>
#include <gum/string/String.h>
#include <gum/time/Types.h>

#include <type_traits>

namespace gum {

GUM_DECLARE_METHOD_DETECTOR(to_string);

template <typename Value_, typename Enabler_ = void>
struct StringRepresentableTrait : std::false_type {};

template <typename Value_>
std::enable_if_t<StringRepresentableTrait<Value_>::value, String> to_string(const Value_& value) {
    return StringRepresentableTrait<Value_>::to_string(value);
}

template <typename Value_>
std::enable_if_t<StringRepresentableTrait<Value_>::value && !StringRepresentableIntrinsicTrait<Value_>::value, String&>
operator<<(String& string, const Value_& object) {
    return string << to_string(object);
}

template <typename Value_>
std::enable_if_t<StringRepresentableTrait<Value_>::value && !StringRepresentableIntrinsicTrait<Value_>::value, String&&>
operator<<(String&& string, const Value_& object) {
    return std::move(string).operator<<(to_string(object));
}

template <typename Value_>
struct StringRepresentableTrait<Value_, std::enable_if_t<StringRepresentableIntrinsicTrait<Value_>::value>> : std::true_type {
    static String to_string(const Value_& value) {
        return String() << value;
    }
};

template <typename Value_>
struct StringRepresentableTrait<Value_, std::enable_if_t<HasMethod_to_string<Value_>::value>> : std::true_type {
    static String to_string(const Value_& value) {
        return value.to_string();
    }
};

template <typename Value_>
struct StringRepresentableTrait<Value_, std::enable_if_t<std::is_fundamental<Value_>::value && !StringRepresentableIntrinsicTrait<Value_>::value>>
    : std::true_type {
    static String to_string(Value_ value) {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }
};

template <typename Value_>
struct StringRepresentableTrait<Value_, std::enable_if_t<std::is_pointer<Value_>::value && !StringRepresentableIntrinsicTrait<Value_>::value>>
    : std::true_type {
    static String to_string(Value_ value) {
        std::stringstream ss;
        ss << std::hex << value;
        return ss.str();
    }
};

template <>
struct StringRepresentableTrait<bool> : std::true_type {
    static String to_string(bool b) {
        return b ? "true" : "false";
    }
};

template <>
struct StringRepresentableTrait<std::exception> : std::true_type {
    static String to_string(std::exception const& ex) {
        return ex.what();
    }
};

template <typename First_, typename Second_>
struct StringRepresentableTrait<std::pair<First_, Second_>> : std::true_type {
    static String to_string(std::pair<First_, Second_> const& pair) {
        return String() << "{ " << pair.first << ", " << pair.second << " }";
    }
};

template <>
struct StringRepresentableTrait<std::nullptr_t> : std::true_type {
    static String to_string(std::nullptr_t) {
        return "null";
    }
};

template <>
struct StringRepresentableTrait<TimePoint> : std::true_type {
    static String to_string(TimePoint const& point);
};

template <typename Representation_, typename Period_>
struct StringRepresentableTrait<BasicDuration<Representation_, Period_>> : std::true_type {
    static String to_string(BasicDuration<Representation_, Period_> const& duration) {
        Milliseconds const ms = duration_caster(duration);
        Seconds const sec = duration_caster(duration);
        Minutes const min = duration_caster(duration);

        return String() << "[" << min.count() << ":" << sec.count() << ":" << ms.count() << "]";
    }
};

template <>
struct StringRepresentableTrait<Seconds> : std::true_type {
    static String to_string(Seconds const& duration) {
        return String() << duration.count() << " seconds";
    }
};

template <>
struct StringRepresentableTrait<Minutes> : std::true_type {
    static String to_string(Minutes const& duration) {
        return String() << duration.count() << " minutes";
    }
};

template <>
struct StringRepresentableTrait<Hours> : std::true_type {
    static String to_string(Hours const& duration) {
        return String() << duration.count() << " hours";
    }
};

template <typename Value_>
struct StringRepresentableTrait<Value_, std::enable_if_t<IsPtrSmartpointer<Value_>::value>> : std::true_type {
    static String to_string(Value_ const& value) {
        return value ? gum::to_string(*value) : gum::to_string(nullptr);
    }
};

template <typename Value_>
struct StringRepresentableTrait<Optional<Value_>> : std::true_type {
    static String to_string(Optional<Value_> const& value) {
        return value ? gum::to_string(*value) : gum::to_string(nullptr);
    }
};

template <typename Value_>
struct StringRepresentableTrait<Value_, std::enable_if_t<IsReferenceSmartpointer<Value_>::value>> : std::true_type {
    static String to_string(Value_ const& value) {
        return gum::to_string(*value);
    }
};

template <typename Value_>
struct StringRepresentableTrait<
    Value_,
    std::enable_if_t<IsStlIterable<Value_>::value && !HasMethod_to_string<Value_>::value && !StringRepresentableIntrinsicTrait<Value_>::value>>
    : std::true_type {
    static String to_string(Value_ const& value) {
        String result;
        result << "[";

        for (auto iter = value.begin(); iter != value.end();) {
            result << *iter;

            ++iter;
            if (iter != value.end())
                result << ", ";
        }

        result << "]";
        return result;
    }
};
}
