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

#include <gum/log/LoggerManager.h>

#include <gum/Optional.h>
#include <gum/compare/OwnerLess.h>
#include <gum/concurrency/Mutex.h>
#include <gum/concurrency/RwMutex.h>
#include <gum/log/Logger.h>
#include <gum/token/FunctionToken.h>

#include <boost/container/flat_set.hpp>

#include <functional>
#include <iostream>
#include <unordered_map>

namespace gum {

class LoggerManager::Impl {
    using Sinks = boost::container::flat_set<ILoggerSinkRef, OwnerLess>;
    using Loggers = std::unordered_map<LoggerId, std::reference_wrapper<Logger>>;

  private:
    Sinks _sinks;
    RwMutex _sinks_mutex;

    Loggers _loggers;
    Mutex _log_levels_mutex;

  public:
    void register_logger_sink(ILoggerSinkRef const& logger_sink) {
        ExclusiveMutexLock const l(_sinks_mutex.get_exclusive());

        _sinks.insert(logger_sink);
    }

    void unregister_logger_sink(ILoggerSinkRef const& logger_sink) {
        ExclusiveMutexLock const l(_sinks_mutex.get_exclusive());

        _sinks.erase(logger_sink);
    }

    void register_logger(LoggerId logger_id, Logger& logger) {
        MutexLock const l(_log_levels_mutex);

        GUM_CHECK(!_loggers.count(logger_id), LogicError("Logger already registered"));

        _loggers.emplace(logger_id, logger);
    }

    void unregister_logger(LoggerId logger_id) {
        MutexLock const l(_log_levels_mutex);

        _loggers.erase(logger_id);
    }

    void set_logger_level(LoggerId logger_id, LogLevel level) {
        MutexLock const l(_log_levels_mutex);

        auto const iter = _loggers.find(logger_id);
        if (iter == _loggers.end())
            return;

        iter->second.get().set_log_level(level);
    }

    void log(LogMessage const& message) {
        SharedMutexLock const l(_sinks_mutex.get_shared());

        for (auto const& sink : _sinks) {
            try {
                sink->log(message);
            } catch (std::exception const& ex) {
                std::cerr << "Uncaught exception from logger sink:\n" << ex.what() << std::endl;
            } catch (...) {
                std::cerr << "Uncaught exception from logger sink:\n<unknown exception>" << std::endl;
            }
        }
    }
};

LoggerManager::LoggerManager()
    : _impl(make_shared_ref<Impl>()) {}

LoggerManager& LoggerManager::get() {
    static Self instance;
    return instance;
}

Token LoggerManager::register_logger_sink(ILoggerSinkRef const& logger_sink) {
    _impl->register_logger_sink(logger_sink);
    return make_token<FunctionToken>([=, impl = _impl] { impl->unregister_logger_sink(logger_sink); });
}

Token LoggerManager::register_logger(LoggerId logger_id, Logger& logger) {
    _impl->register_logger(logger_id, logger);
    return make_token<FunctionToken>([=, impl = _impl] { impl->unregister_logger(logger_id); });
}

void LoggerManager::set_logger_level(LoggerId logger_id, LogLevel level) {
    _impl->set_logger_level(logger_id, level);
}

void LoggerManager::log(LogMessage const& message) {
    _impl->log(message);
}
}
