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

#include <gum/log/sinks/AnsiTerminalLoggerSink.h>

namespace gum {

namespace {

StringLiteral const BlackColorStart("\033[30m");
StringLiteral const RedColorStart("\033[31m");
StringLiteral const GreenColorStart("\033[32m");
StringLiteral const YellowColorStart("\033[33m");
StringLiteral const BlueColorStart("\033[34m");
StringLiteral const MagentaColorStart("\033[35m");
StringLiteral const CyanColorStart("\033[36m");
StringLiteral const WhiteColorStart("\033[37m");

StringLiteral const GreyColorStart("\033[30;1m");
StringLiteral const BrightRedColorStart("\033[31;1m");
StringLiteral const BrightGreenColorStart("\033[32;1m");
StringLiteral const BrightYellowColorStart("\033[33;1m");
StringLiteral const BrightBlueColorStart("\033[34;1m");
StringLiteral const BrightMagentaColorStart("\033[35;1m");
StringLiteral const BrightCyanColorStart("\033[36;1m");
StringLiteral const BrightWhiteColorStart("\033[37;1m");

StringLiteral const AttributeReset("\033[0m");

struct LogLevelColorMapper {
    StringLiteral operator()(LogLevel level) const {
        switch (level) {
        case LogLevel::Trace:
            return GreyColorStart;
        case LogLevel::Debug:
            return GreyColorStart;
        case LogLevel::Info:
            return WhiteColorStart;
        case LogLevel::Warning:
            return YellowColorStart;
        case LogLevel::Error:
            return RedColorStart;
        case LogLevel::Highlight:
            return BrightCyanColorStart;
        default:
            GUM_THROW(NotImplementedException());
        }
    }
};
}

void AnsiTerminalLoggerSink::log(LogMessage const& message) {
    MutexLock l(_mutex);

    StringLiteral const WhenColorStart = BlueColorStart;
    StringLiteral const ThreadNameColorStart = MagentaColorStart;
    StringLiteral const AuthorColorStart = GreenColorStart;
    StringLiteral const LogLevelColorStart = LogLevelColorMapper()(message.level);

    printf("%s[%s]%s %s{%s}%s %s[%s]%s %s[%s]%s %s%s%s\n",
           WhenColorStart.c_str(),
           to_string(message.when).c_str(),
           AttributeReset.c_str(),
           ThreadNameColorStart.c_str(),
           message.thread->c_str(),
           AttributeReset.c_str(),
           AuthorColorStart.c_str(),
           message.author.c_str(),
           AttributeReset.c_str(),
           LogLevelColorStart.c_str(),
           message.level.to_string().c_str(),
           AttributeReset.c_str(),
           LogLevelColorStart.c_str(),
           message.message.c_str(),
           AttributeReset.c_str());
}
}
