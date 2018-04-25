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

#include <gum/Enum.h>
#include <gum/concurrency/GenericMutexLock.h>
#include <gum/concurrency/ICancellationToken.h>
#include <gum/time/Types.h>

#include <condition_variable>

namespace gum {

class ConditionVariable {
    using Impl = std::condition_variable_any;

  public:
    GUM_ENUM(WaitResult, TimedOut, Woken);

  private:
    mutable Impl _impl;

  public:
    template <typename Mutex_>
    void wait(Mutex_ const& mutex, ICancellationHandle& handle) const {
        Token t = handle.on_cancelled([this, &mutex] { cancel(mutex); });
        if (!handle)
            return;

        _impl.wait(mutex);
    }

    template <typename Mutex_, typename Predicate_>
    void wait(Mutex_ const& mutex, Predicate_ const& predicate, ICancellationHandle& handle) const {
        Token t = handle.on_cancelled([this, &mutex] { cancel(mutex); });
        if (!handle)
            return;

        _impl.wait(mutex, [&] { return !handle || predicate(); });
    }

    template <typename Mutex_>
    WaitResult wait_for(Mutex_ const& mutex, Duration const& duration, ICancellationHandle& handle) const {
        Token t = handle.on_cancelled([this, &mutex] { cancel(mutex); });
        if (!handle)
            return WaitResult::Woken;

        return _impl.wait_for(mutex, duration) == std::cv_status::timeout ? WaitResult::TimedOut : WaitResult::Woken;
    }

    template <typename Mutex_, typename Predicate_>
    bool wait_for(Mutex_ const& mutex, Duration const& duration, Predicate_ const& predicate, ICancellationHandle& handle) const {
        Token t = handle.on_cancelled([this, &mutex] { cancel(mutex); });
        if (!handle)
            return predicate();

        return _impl.wait_for(mutex, duration, [&] { return !handle || predicate(); });
    }

    void broadcast() const {
        _impl.notify_all();
    }

  private:
    template <typename Mutex_>
    void cancel(Mutex_ const& mutex) const {
        GenericMutexLock<Mutex_> l(mutex);
        broadcast();
    }
};
}
