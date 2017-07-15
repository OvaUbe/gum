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

#include <cppgear/concurrency/ImmutableMutexWrapper.h>
#include <cppgear/concurrency/GenericMutexLock.h>

#include <shared_mutex>

namespace cppgear {

    namespace detail {

        using RwMutexImpl = std::shared_timed_mutex;


        class ExclusiveMutex {
            detail::RwMutexImpl& _impl;

        public:
            explicit ExclusiveMutex(detail::RwMutexImpl& impl)
                :   _impl(impl)
            { }

            void lock() {
                _impl.lock();
            }

            void unlock() {
                _impl.unlock();
            }
        };


        class SharedMutex {
            detail::RwMutexImpl& _impl;

        public:
            explicit SharedMutex(detail::RwMutexImpl& impl)
                :   _impl(impl)
            { }

            void lock() {
                _impl.lock_shared();
            }

            void unlock() {
                _impl.unlock_shared();
            }
        };

    }


    using SharedMutex = ImmutableMutexWrapper<detail::SharedMutex>;
    using ExclusiveMutex = ImmutableMutexWrapper<detail::ExclusiveMutex>;


    class RwMutex {
        mutable detail::RwMutexImpl _impl;

    public:
        ExclusiveMutex get_exclusive() const {
            return ExclusiveMutex(_impl);
        }

        SharedMutex get_shared() const {
            return SharedMutex(_impl);
        }
    };


    using ExclusiveMutexLock = GenericMutexLock<ExclusiveMutex>;
    using SharedMutexLock = GenericMutexLock<SharedMutex>;

}
