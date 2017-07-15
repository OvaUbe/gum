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

#include <cppgear/concurrency/GenericMutexLock.h>
#include <cppgear/concurrency/ICancellationToken.h>
#include <cppgear/time/Types.h>

#include <condition_variable>

namespace cppgear {

    class ConditionVariable {
        using Impl = std::condition_variable_any;

        mutable Impl _impl;

    public:
        template < typename Lock_ >
        void wait(Lock_ const& lock, ICancellationHandle& handle) const {
            Token const t = handle.on_cancelled([&]{ self.cancel(lock); });

            _impl.wait(lock);
        }

        template < typename Lock_, typename Predicate_ >
        void wait(Lock_ const& lock, Predicate_ const& predicate, ICancellationHandle& handle) const {
            Token const t = handle.on_cancelled([&]{ self.cancel(lock); });

            _impl.wait(lock, [&]{ return !handle || predicate(); });
        }

        template < typename Lock_ >
        bool wait_for(Lock_ const& lock, SystemClock::duration const& duration, ICancellationHandle& handle) const {
            Token const t = handle.on_cancelled([&]{ self.cancel(lock); });

            return _impl.wait_for(lock, duration) == std::cv_status::timeout;
        }

        template < typename Lock_, typename Predicate_ >
        bool wait_for(Lock_ const& lock, SystemClock::duration const& duration, Predicate_ const& predicate, ICancellationHandle& handle) const {
            Token const t = handle.on_cancelled([&]{ self.cancel(lock); });

            return _impl.wait_for(lock, duration, [&]{ return !handle || predicate(); });
        }

        void broadcast() const {
            _impl.notify_all();
        }

    private:
        template < typename Lock_ >
        void cancel(Lock_ const& lock) const {
            GenericMutexLock<Lock_> const l(lock);
            broadcast();
        }
    };

}
