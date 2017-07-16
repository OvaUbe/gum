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

#include <cppgear/concurrency/ThreadInfo.h>
#include <cppgear/log/LoggerSingleton.h>
#include <cppgear/string/ToString.h>

namespace cppgear {

    CPPGEAR_LOGGER_SINGLETON(MutexLogger);


    template < typename TimedMutex_ >
    class TimedMutexWrapper {
        TimedMutex_         _impl;

        ThreadInfo          _thread_info;

    public:
        TimedMutexWrapper() = default;

        TimedMutexWrapper(TimedMutexWrapper&& other)
            :   _impl(std::move(other._impl))
        { }

        TimedMutexWrapper(TimedMutex_&& impl)
            :   _impl(std::move(impl))
        { }

        void lock() {
            const Seconds Threshold = Seconds(3);

            Seconds duration;
            while (!_impl.try_lock_for(Threshold)) {
                duration += Threshold;

                MutexLogger::get().warning()
                    << "Could not lock mutex owned by: '" << _thread_info << "' for " << duration << "."
                    << " There is probably a deadlock.\nBacktrace: " << Backtrace();
            }

            _thread_info.acquire();
        }

        void unlock() {
            _impl.unlock();
        }
    };

}
