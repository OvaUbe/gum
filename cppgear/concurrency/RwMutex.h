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

#include <shared_mutex>

namespace cppgear {

    namespace detail {

        class RwMutexImpl {
            using Impl = std::shared_timed_mutex;

        private:
            mutable Impl _impl;

        public:
            void lock_exclusive() const {
                _impl.lock();
            }

            void unlock_exclusive() const {
                _impl.unlock();
            }

            void lock_shared() const {
                _impl.lock_shared();
            }

            void unlock_shared() const {
                _impl.unlock_shared();
            }
        };

    }


    class ExclusiveMutex {
        friend class RwMutex;

    private:
        detail::RwMutexImpl const& _impl;

    private:
        explicit ExclusiveMutex(detail::RwMutexImpl const& impl)
            :   _impl(impl)
        { }

    public:
        void lock() const {
            _impl.lock_exclusive();
        }

        void unlock() const {
            _impl.unlock_exclusive();
        }
    };


    class SharedMutex {
        friend class RwMutex;

    private:
        detail::RwMutexImpl const& _impl;

    private:
        explicit SharedMutex(detail::RwMutexImpl const& impl)
            :   _impl(impl)
        { }

    public:
        void lock() const {
            _impl.lock_shared();
        }

        void unlock() const {
            _impl.unlock_shared();
        }
    };


    class RwMutex {
        detail::RwMutexImpl _impl;

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
