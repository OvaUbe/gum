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

#include <cppgear/string/String.h>
#include <cppgear/Singleton.h>

#include <unordered_map>

namespace cppgear {

    namespace detail {

        class EnumToStringMapping {
        public:
            using UnderlyingIntType = s32;

        private:
            using Impl = std::unordered_map<UnderlyingIntType, String>;

        private:
            Impl _mapping;

        public:
            EnumToStringMapping(char const* mapping_);

            String const& map(UnderlyingIntType e);
        };

    }


#   define CPPGEAR_ENUM(Name_, ...) \
        class Name_ { \
            using UnderlyingIntType = s32; \
        \
        public: \
            enum Enum : detail::EnumToStringMapping::UnderlyingIntType { \
                __VA_ARGS__ \
            }; \
        \
        private: \
            struct StringMapping : public detail::EnumToStringMapping, public Singleton<StringMapping> { \
                StringMapping() \
                    :   detail::EnumToStringMapping(#__VA_ARGS__) \
                { } \
            }; \
            \
        private: \
            Enum _e; \
            \
        public: \
            Name_() = default; \
            \
            Name_(Enum e) \
                :   _e(e) \
            { } \
            \
            operator Enum() const { \
                return _e; \
            } \
            \
            String const& to_string() const { \
                return StringMapping::get().map(_e); \
            } \
        }

}
