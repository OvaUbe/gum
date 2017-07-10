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

#include <mutex>

namespace cppgear {

    namespace detail {

        template < typename Impl_ >
        class GenericMutexWrapper {
            mutable Impl_ _impl;

        public:
            void lock() const {
                _impl.lock();
            }

            void unlock() const {
                _impl.unlock();
            }
        };

    }


    using Mutex = detail::GenericMutexWrapper<std::mutex>;
    using RecursiveMutex = detail::GenericMutexWrapper<std::recursive_mutex>;


    template < typename Lock_ >
    class GenericMutexLock {
        Lock_ const& _lock;

    public:
        GenericMutexLock(Lock_ const& lock)
            :   _lock(lock)
        { _lock.lock(); }

        ~GenericMutexLock() {
            _lock.unlock();
        }
    };

    using MutexLock = GenericMutexLock<Mutex>;
    using RecursiveMutexLock = GenericMutexLock<RecursiveMutex>;

}
