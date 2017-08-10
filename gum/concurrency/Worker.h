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

#include <gum/async/ITaskQueue.h>
#include <gum/concurrency/ConditionVariable.h>
#include <gum/concurrency/Mutex.h>
#include <gum/concurrency/Thread.h>
#include <gum/functional/Types.h>
#include <gum/Optional.h>

#include <deque>

namespace gum {

    class Worker : public virtual ITaskQueue {
        using Self = Worker;

        using Queue = std::deque<Task>;

    private:
        static Logger       s_logger;

        Queue               _queue;
        Mutex               _mutex;
        ConditionVariable   _condition_variable;

        Thread              _thread;

    public:
        template < typename String_ >
        Worker(String_&& name)
            :   _thread(std::forward<String_>(name), std::bind(&Self::thread_func, this, _1))
        { }

        void push(Task&& task) override;

    private:
        void thread_func(ICancellationHandle& handle);

        Optional<Task> pop(ICancellationHandle& handle);
        Optional<Task> pop();
        Task do_pop();
    };

}
