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

#include <gum/diagnostics/Demangle.h>
#include <gum/exception/ExceptionDetails.h>

#include <gum/Types.h>
#include <gum/string/String.h>

namespace gum {

struct Exception : public detail::Exception {
    Exception(String const& message)
        : detail::Exception(std::string(message)) {}

    Exception(char const* message)
        : detail::Exception(message) {}
};

namespace detail {

inline auto make_exception(std::exception const& ex, Where const& where, Backtrace const& backtrace) {
    return do_make_exception(ex, where, backtrace);
}

inline auto make_exception(String const& message, Where const& where, Backtrace const& backtrace) {
    return do_make_exception(gum::Exception(message), where, backtrace);
}

inline auto make_exception(String&& message, Where const& where, Backtrace const& backtrace) {
    return do_make_exception(gum::Exception(message), where, backtrace);
}

inline auto make_exception(char const* message, Where const& where, Backtrace const& backtrace) {
    return do_make_exception(gum::Exception(message), where, backtrace);
}
}

#define GUM_THROW(Ex_) throw gum::detail::make_exception(Ex_, GUM_WHERE, gum::Backtrace())

#define GUM_CHECK(Condition_, Otherwise_)                                                                                                                      \
    if (GUM_UNLIKELY(!(Condition_)))                                                                                                                           \
    GUM_THROW(Otherwise_)

#define GUM_DECLARE_EXCEPTION(Type_, DefaultMessage_)                                                                                                          \
    struct Type_ : public gum::Exception {                                                                                                                     \
        Type_()                                                                                                                                                \
            : gum::Exception(DefaultMessage_) {}                                                                                                               \
                                                                                                                                                               \
        Type_(const String& message)                                                                                                                           \
            : gum::Exception(String() << DefaultMessage_ << ": " << message) {}                                                                                \
    }

GUM_DECLARE_EXCEPTION(NullPointerException, "Accessing null pointer");
GUM_DECLARE_EXCEPTION(InternalError, "Internal error");
GUM_DECLARE_EXCEPTION(LogicError, "Logic error");
GUM_DECLARE_EXCEPTION(NotImplementedException, "Not implemented");

struct IndexOutOfRangeException : public Exception {
    IndexOutOfRangeException(u64 index, u64 begin, u64 end);
    IndexOutOfRangeException(s64 index, s64 begin, s64 end);
    IndexOutOfRangeException(u64 index, u64 size);
    IndexOutOfRangeException(s64 index, s64 size);
};

#define GUM_CHECK_RANGE(Index_, Begin_, End_) GUM_CHECK((Index_ >= Begin_) && (Index_ < End_), gum::IndexOutOfRangeException(Index_, Begin_, End_))

#define GUM_CHECK_INDEX(Index_, Size_) GUM_CHECK((Index_ < Size_), gum::IndexOutOfRangeException(Index_, Size_))

template <typename From_, typename To_>
struct InvalidCastException : public Exception {
    InvalidCastException(From_ const& from)
        : Exception(String() << "Invalid cast from '" << demangle(typeid(from).name()) << "' to '" << demangle(typeid(To_).name()) << "'") {}
};

struct ArgumentException : public Exception {
    template <typename Arg_>
    ArgumentException(String const& name, Arg_ const& arg)
        : Exception(String() << "Unwanted argument '" << name << "':" << arg) {}
};
}
