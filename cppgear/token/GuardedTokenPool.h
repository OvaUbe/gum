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

#include <cppgear/concurrency/LifeToken.h>
#include <cppgear/token/IGuardedTokenPool.h>
#include <cppgear/token/TokenPool.h>

namespace cppgear {

    template < bool IsSynchronized_ >
    class BasicGuardedTokenPool : public virtual IGuardedTokenPool {
        using TokenPoolType = BasicTokenPool<IsSynchronized_>;

    private:
        TokenPoolType       _tokens;
        LifeToken           _life_token;

    public:
        BasicGuardedTokenPool(LifeToken&& life_token = LifeToken())
            :   _life_token(std::move(life_token))
        { }

        void operator += (Token&& token) override {
            _tokens += std::move(token);
        }

        LifeHandle get_handle() const override {
            return _life_token.get_handle();
        }

        void release() {
            _life_token.release();
            _tokens.release();
        }
    };

    using GuardedTokenPool = BasicGuardedTokenPool<false>;
    using SynchronizedGuardedTokenPool = BasicGuardedTokenPool<true>;

}
