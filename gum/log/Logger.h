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

#include <gum/log/LoggerStream.h>
#include <gum/token/Token.h>

namespace gum {

class Logger {
    StringLiteral _name;
    Token _registration;

  public:
    Logger(StringLiteral const& name, LogLevel default_log_level = LogLevel::Info);

    LoggerStream trace() const {
        return log(LogLevel::Trace);
    }

    LoggerStream debug() const {
        return log(LogLevel::Debug);
    }

    LoggerStream info() const {
        return log(LogLevel::Info);
    }

    LoggerStream warning() const {
        return log(LogLevel::Warning);
    }

    LoggerStream error() const {
        return log(LogLevel::Error);
    }

    LoggerStream highlight() const {
        return log(LogLevel::Highlight);
    }

    LoggerStream log(LogLevel level) const {
        return LoggerStream(get_id(), _name, level);
    }

    void set_log_level(LogLevel level) const;

  private:
    LoggerId get_id() const {
        return LoggerId(this);
    }
};

#define GUM_DEFINE_LOGGER(Type_) gum::Logger Type_::_logger(#Type_)

#define GUM_DEFINE_NAMED_LOGGER(Type_, Name_) gum::Logger Type_::_logger(#Name_)
}
