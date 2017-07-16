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

#include <cppgear/log/LoggerStream.h>

#include <cppgear/concurrency/Thread.h>
#include <cppgear/log/LoggerManager.h>

namespace cppgear {

    LoggerStream::LoggerStream(LoggerId logger_id, StringLiteral const& logger_name, LogLevel level)
        :   _logger_id(logger_id),
            _logger_name(logger_name),
            _level(level)
    { }


    LoggerStream::~LoggerStream() {
        try {
            try {
                LoggerManager::get().log(LogMessage(_logger_id, SystemClock::now(), _level, Thread::get_own_name(), _logger_name, std::move(_message)));
            } catch (std::exception const& ex) {
                String message = String() << "Caught exception during logging, next failure will be silently swallowed:\n" << ex;
                LoggerManager::get().log(LogMessage(_logger_id, SystemClock::now(), LogLevel::Error, Thread::get_own_name(), _logger_name, std::move(message)));
            } catch (...) {
                String message = String() << "Caught unknown exception during logging, next failure will be silently swallowed.";
                LoggerManager::get().log(LogMessage(_logger_id, SystemClock::now(), LogLevel::Error, Thread::get_own_name(), _logger_name, std::move(message)));
            }
        } catch (...) { }
    }

}
