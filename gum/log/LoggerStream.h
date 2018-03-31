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

#include <gum/log/LogLevel.h>
#include <gum/log/LoggerId.h>
#include <gum/string/StringLiteral.h>
#include <gum/string/ToString.h>

#include <sstream>

namespace gum {

    class LoggerStream {
        using Self = LoggerStream;

    private:
        LoggerId        _logger_id;
        StringLiteral   _logger_name;
        LogLevel        _level;

        String          _message;

    public:
        LoggerStream(LoggerId logger_id, StringLiteral const& logger_name, LogLevel level);
        ~LoggerStream();

        template < typename Value_ >
        Self& operator<<(const Value_& value) {
            _message << value;
            return *this;
        }
    };

}
