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

#include <cppgear/diagnostics/Backtrace.h>
#include <cppgear/Core.h>
#include <cppgear/Types.h>

#include <stdexcept>
#include <typeinfo>

namespace cppgear {

    struct Exception : public std::runtime_error {
        Exception(std::string const& message)
            : std::runtime_error(message)
        { }

        Exception(char const* message)
            : std::runtime_error(message)
        { }
    };


    namespace detail {

        std::string get_diagnostics_message(char const* message, std::type_info const& client_type_info, Where const& where, Backtrace const& backtrace);


        template < typename ClientException_ >
        class ExceptionTemplate : public ClientException_ {
            std::string _what;

        public:
            template < typename ClientException__ >
            ExceptionTemplate(ClientException__&& ex, Where const& where, Backtrace const& backtrace)
                :   ClientException_(std::forward<ClientException__>(ex)),
                    _what(get_diagnostics_message(ex.what(), typeid(ClientException_), where, backtrace))
            { }

            char const* what() const noexcept override {
                return _what.c_str();
            }
        };


        template < typename Exception_ >
        inline auto make_exception(Exception_&& ex, Where const& where, Backtrace const& backtrace) {
            return ExceptionTemplate<Exception_>(std::forward<Exception_>(ex), where, backtrace);
        }

        inline auto make_exception(std::string const& message, Where const& where, Backtrace const& backtrace) {
            return make_exception(Exception(message), where, backtrace);
        }

        inline auto make_exception(char const* message, Where const& where, Backtrace const& backtrace) {
            return make_exception(Exception(message), where, backtrace);
        }

    }


#   define CPPGEAR_THROW(Ex_) \
        throw cppgear::detail::make_exception(Ex_, CPPGEAR_WHERE, cppgear::Backtrace())

#   define CPPGEAR_CHECK(Condition_, Otherwise_) \
        if (CPPGEAR_UNLIKELY(!Condition_)) CPPGEAR_THROW(Otherwise_)


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
    CPPGEAR_DECLARE_EXCEPTION(InternalError, "Internal error");
    CPPGEAR_DECLARE_EXCEPTION(LogicError, "Logic error");
    CPPGEAR_DECLARE_EXCEPTION(NotImplementedException, "Not implemented");


    struct IndexOutOfRangeException : public Exception {
        IndexOutOfRangeException(u64 index, u64 begin, u64 end);
        IndexOutOfRangeException(s64 index, s64 begin, s64 end);

        IndexOutOfRangeException(u64 index, u64 size);
        IndexOutOfRangeException(s64 index, s64 size);
    };

#   define CPPGEAR_CHECK_RANGE(Index_, Begin_, End_) \
        CPPGEAR_CHECK((Index_ >= Begin_) && (Index_ < End_), IndexOutOfRangeException(Index_, Begin_, End_))

#   define CPPGEAR_CHECK_INDEX(Index_, Size_) \
        CPPGEAR_CHECK(Index_ < Size_, IndexOutOfRangeException(Index_, Size_))

}
