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

#include <gum/concurrency/GenericMutexLock.h>
#include <gum/concurrency/ImmutableMutexWrapper.h>
#include <gum/concurrency/TimedMutexWrapper.h>

#include <shared_mutex>

namespace gum {

namespace detail {

using RwMutexImpl = std::shared_timed_mutex;

class ExclusiveTimedMutex {
    detail::RwMutexImpl& _impl;

  public:
    ExclusiveTimedMutex(detail::RwMutexImpl& impl)
        : _impl(impl) {}

    bool try_lock_for(Duration const& duration) {
        return _impl.try_lock_for(duration);
    }

    void unlock() {
        _impl.unlock();
    }
};
using ExclusiveMutex = TimedMutexWrapper<detail::ExclusiveTimedMutex>;

class SharedTimedMutex {
    detail::RwMutexImpl& _impl;

  public:
    SharedTimedMutex(detail::RwMutexImpl& impl)
        : _impl(impl) {}

    bool try_lock_for(Duration const& duration) {
        return _impl.try_lock_shared_for(duration);
    }

    void unlock() {
        _impl.unlock_shared();
    }
};
using SharedMutex = TimedMutexWrapper<detail::SharedTimedMutex>;
}

using ExclusiveMutex = ImmutableMutexWrapper<detail::ExclusiveMutex>;
using SharedMutex = ImmutableMutexWrapper<detail::SharedMutex>;

class RwMutex {
    mutable detail::RwMutexImpl _impl;

  public:
    ExclusiveMutex get_exclusive() const {
        return detail::ExclusiveMutex(_impl);
    }

    SharedMutex get_shared() const {
        return detail::SharedMutex(_impl);
    }
};

using ExclusiveMutexLock = GenericMutexLock<ExclusiveMutex>;
using SharedMutexLock = GenericMutexLock<SharedMutex>;
}
