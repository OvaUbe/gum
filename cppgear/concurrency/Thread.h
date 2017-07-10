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
        using Self = Thread;

        using Impl = std::thread;

    public:
        using TaskSignature = void(ICancellationHandle&);
        using TaskType = std::function<TaskSignature>;

    private:
        std::string         _name;
        TaskType            _task;

        CancellationToken   _cancellation_token;
        Impl                _impl;

    public:
        template < typename String_, typename Callable_ >
        Thread(String_&& name, Callable_&& callable)
            :   _name(std::forward<String_>(name)),
                _task(std::forward<Callable_>(callable)),
                _impl(std::bind(&Self::thread_func, this))
        { }

        ~Thread();

        static std::string get_own_name();

        std::string get_name() const;

    private:
        void thread_func();

        void dtor();
    };
    CPPGEAR_DECLARE_UNIQUE_REF(Thread);
    CPPGEAR_DECLARE_UNIQUE_PTR(Thread);

}
