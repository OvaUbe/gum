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

#include <cppgear/Enum.h>

#include <cppgear/string/StringLiteral.h>
#include <cppgear/Optional.h>

namespace cppgear {
namespace detail {

    namespace {

        String const EnumToStringStub = "<unavailable>";

    }


    EnumToStringMapping::EnumToStringMapping(char const* mapping_) {
        StringLiteral mapping = mapping_;

        String stream;

        String name;
        Optional<UnderlyingIntType> number(0);
        bool currently_parsing_name(true);

        auto appender = [&] {
            if (currently_parsing_name) {
                name = std::move(stream);
                if (number) {
                    ++*number;
                } else {
                    number = 0;
                }
            } else {
                number = std::stoi(std::string(std::move(stream)));
            }
            _mapping[*number] = std::move(name);

            stream = String();
            name = String();
            currently_parsing_name = true;
        };

        for (auto const ch : mapping) {
            if (ch == '\n' || ch == ' ') {
                continue;
            }
            if (ch == '=') {
                name = std::move(stream);

                stream = String();
                currently_parsing_name = false;
                continue;
            }
            if (ch == ',') {
                appender();
                continue;
            }
            stream << ch;
        }
        if (!stream.empty()) {
            appender();
        }
    }


    String const& EnumToStringMapping::map(UnderlyingIntType i) {
        auto const iter = _mapping.find(i);
        if (iter != _mapping.end()) {
            return iter->second;
        }
        return EnumToStringStub;
    }

}}
