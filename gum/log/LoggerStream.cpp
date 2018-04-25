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

#include <gum/log/LoggerStream.h>

#include <gum/concurrency/Thread.h>
#include <gum/log/LoggerManager.h>

#include <iostream>

namespace gum {

LoggerStream::LoggerStream(LoggerId logger_id, StringLiteral const& logger_name, LogLevel level, bool mute)
    : _logger_id(logger_id)
    , _logger_name(logger_name)
    , _level(level)
    , _message(mute ? Optional<String>() : make_optional<String>()) {}

LoggerStream::~LoggerStream() {
    if (!_message)
        return;

    try {
        LogMessage message(_logger_id, SystemClock::now(), _level, Thread::get_own_info()->get_name(), _logger_name, std::move(*_message));
        try {
            LoggerManager::get().log(message);
        } catch (std::exception const& ex) {
            message.message = String() << "Uncaught exception in LoggerStream::~LoggerStream(), next failure will be dumped to stderr:\n" << ex;
            LoggerManager::get().log(message);
        } catch (...) {
            message.message = String() << "Uncaught exception in LoggerStream::~LoggerStream(), next failure will be dumped to stderr:\n<unknown exception>";
            LoggerManager::get().log(message);
        }
    } catch (std::exception const& ex) {
        std::cerr << "Uncaught exception in LoggerStream::~LoggerStream():\n" << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Uncaught exception in LoggerStream::~LoggerStream():\n<unknown exception>" << std::endl;
    }
}
}
