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

#include <cppgear/log/LoggerManager.h>

#include <cppgear/compare/OwnerLess.h>
#include <cppgear/concurrency/Mutex.h>
#include <cppgear/concurrency/RwMutex.h>
#include <cppgear/container/FlatSet.h>
#include <cppgear/token/FunctionToken.h>
#include <cppgear/Optional.h>

#include <unordered_map>

namespace cppgear {

    class LoggerManager::Impl {
        using Sinks = FlatSet<ILoggerSinkRef, OwnerLess>;
        using LogLevels = std::unordered_map<LoggerId, LogLevel>;

    private:
        Sinks                   _sinks;
        RwMutex                 _sinks_mutex;

        LogLevels               _log_levels;
        Mutex                   _log_levels_mutex;

    public:
        void register_logger_sink(ILoggerSinkRef const& logger_sink) {
            ExclusiveMutexLock const l(_sinks_mutex.get_exclusive());

            _sinks.insert(logger_sink);
        }

        void unregister_logger_sink(ILoggerSinkRef const& logger_sink) {
            ExclusiveMutexLock const l(_sinks_mutex.get_exclusive());

            _sinks.erase(logger_sink);
        }

        void register_logger(LoggerId logger_id, LogLevel default_log_level) {
            MutexLock const l(_log_levels_mutex);

            CPPGEAR_CHECK(!_log_levels.count(logger_id), LogicError("Logger already registered"));

            _log_levels[logger_id] = default_log_level;
        }

        void unregister_logger(LoggerId logger_id) {
            MutexLock const l(_log_levels_mutex);

            _log_levels.erase(logger_id);
        }

        void set_logger_level(LoggerId logger_id, LogLevel level) {
            MutexLock const l(_log_levels_mutex);

            auto const iter = _log_levels.find(logger_id);
            if (iter == _log_levels.end())
                return;

            iter->second = level;
        }

        void log(LogMessage const& message) {
            Optional<LogLevel> const level =  get_log_level(message.logger_id);
            if (!level || message.level < *level)
                return;

            SharedMutexLock const l(_sinks_mutex.get_shared());

            for (auto const& sink : _sinks)
                sink->log(message);
        }

    private:
        Optional<LogLevel> get_log_level(LoggerId logger_id) const {
            MutexLock const l(_log_levels_mutex);

            auto const iter = _log_levels.find(logger_id);
            if (iter == _log_levels.end())
                return nullptr;

            return iter->second;
        }
    };


    LoggerManager& LoggerManager::get() {
        static Self instance;
        return instance;
    }


    Token LoggerManager::register_logger_sink(ILoggerSinkRef const& logger_sink) {
        _impl->register_logger_sink(logger_sink);
        return make_token<FunctionToken>([=]{ _impl->unregister_logger_sink(logger_sink); });
    }


    Token LoggerManager::register_logger(LoggerId logger_id, LogLevel default_log_level) {
        _impl->register_logger(logger_id, default_log_level);
        return make_token<FunctionToken>([=]{ _impl->unregister_logger(logger_id); });
    }


    void LoggerManager::set_logger_level(LoggerId logger_id, LogLevel level) {
        _impl->set_logger_level(logger_id, level);
    }


    void LoggerManager::log(LogMessage const& message) {
        _impl->log(message);
    }

}
