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

#include <cppgear/concurrency/DummyMutex.h>
#include <cppgear/concurrency/GenericMutexLock.h>
#include <cppgear/concurrency/Mutex.h>
#include <cppgear/token/ITokenPool.h>

#include <vector>

namespace cppgear {

    template < bool IsSynchronized_ >
    class BasicTokenPool : public virtual ITokenPool {
        using Tokens = std::vector<Token>;

        using MutexType = std::conditional_t<IsSynchronized_, Mutex, DummyMutex>;
        using MutexLockType = GenericMutexLock<MutexType>;

    private:
        Tokens      _tokens;
        MutexType   _mutex;

    public:
        void operator += (Token&& token) override {
            MutexLockType l(_mutex);
            _tokens.push_back(std::move(token));
        }

        void release() {
            Tokens tokens;

            MutexLockType l(_mutex);
            _tokens.swap(tokens);
        }
    };

    using TokenPool = BasicTokenPool<false>;
    using SynchronizedTokenPool = BasicTokenPool<true>;

}
