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

#include <gum/concurrency/CancellationToken.h>
#include <gum/concurrency/ThreadInfo.h>
#include <gum/log/Logger.h>
#include <gum/time/Types.h>

namespace gum {

class Thread {
    using Self = Thread;

    using Impl = std::thread;

  public:
    using TaskSignature = void(ICancellationHandle&);
    using TaskType = std::function<TaskSignature>;

  private:
    static Logger _logger;

    StringConstRef _name;
    TaskType _task;

    CancellationToken _cancellation_token;
    Impl _impl;

  public:
    template <typename String_, typename Callable_>
    Thread(String_&& name, Callable_&& callable)
        : _name(make_shared_ref<String>(std::forward<String_>(name)))
        , _task(std::forward<Callable_>(callable))
        , _impl(&Self::thread_func, this) {}

    Thread(Thread&&) = default;
    Thread& operator=(Thread&&) = default;

    ~Thread();

    static void set_own_name(gum::String const& name);
    static ThreadInfoRef get_own_info();

    static void sleep(Duration const& duration);
    static void sleep(Duration const& duration, ICancellationHandle& handle);

    ThreadInfo get_info() const;

    String to_string() const;

  private:
    void thread_func();
    void _thread_func();

    void dtor();
};
GUM_DECLARE_UNIQUE_REF(Thread);
GUM_DECLARE_UNIQUE_PTR(Thread);
}
