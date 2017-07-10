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

#include <cppgear/concurrency/CancellationToken.h>

#include <thread>

namespace cppgear {

    class Thread {
        using Impl = std::thread;

    public:
        using Signature = void(ICancellationHandle&);

    private:
        CancellationToken   _cancellation_token;
        Impl                _impl;

    public:
        template < typename Callable_ >
        Thread(Callable_&& callable)
            :   _impl(std::bind(std::forward<Callable_>(callable), std::ref(_cancellation_token)))
        { }

        ~Thread();

    private:
        void dtor();
    };
    CPPGEAR_DECLARE_UNIQUE_REF(Thread);
    CPPGEAR_DECLARE_UNIQUE_PTR(Thread);

}