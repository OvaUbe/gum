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

#include <cppgear/concurrency/Thread.h>
#include <cppgear/log/LoggerSingleton.h>
#include <cppgear/smartpointer/ToString.h>
#include <cppgear/time/ToString.h>

#include <mutex>

namespace cppgear {

    CPPGEAR_LOGGER_SINGLETON(MutexLogger);


    template < typename TimedMutex_ >
    class TimedMutexWrapper {
        using OwnerIdGuard = std::mutex;
        using OwnerIdLock = std::lock_guard<OwnerIdGuard>;

    private:
        TimedMutex_         _impl;

        StringConstPtr      _ownerId;
        OwnerIdGuard        _ownerIdGuard;

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
                    << "Could not lock mutex owned by: '" << get_owner_id() << "' for \n."
                    << "There is probably a deadlock.\nBacktrace: " << Backtrace();
            }

            set_owner_id(Thread::get_own_name());
        }

        void unlock() {
            _impl.unlock();
        }

    private:
        void set_owner_id(StringConstRef const& id) {
            OwnerIdLock const l(_ownerIdGuard);
            _ownerId = id;
        }

        StringConstPtr get_owner_id() {
            OwnerIdLock const l(_ownerIdGuard);
            return _ownerId;
        }
    };

}
