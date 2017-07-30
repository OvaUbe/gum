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

#include <cppgear/concurrency/Worker.h>

#include <cppgear/functional/Invoker.h>
#include <cppgear/Maybe.h>
#include <cppgear/Try.h>

namespace cppgear {

    CPPGEAR_DEFINE_LOGGER(Worker);


    void Worker::push(Task&& task) {
        MutexLock l(_mutex);

        _queue.push_back([task = std::move(task)]{ CPPGEAR_TRY("Uncaught exception in worker task", task()); });
        _condition_variable.broadcast();
    }


    void Worker::thread_func(ICancellationHandle& handle) {
        while (maybe(pop(handle)).and_(Invoker()));

        while (maybe(pop()).and_(Invoker()));
    }


    Optional<Worker::Task> Worker::pop(ICancellationHandle& handle) {
        MutexLock l(_mutex);

        _condition_variable.wait(_mutex, [this]{ return !_queue.empty(); }, handle);
        if (!handle)
            return nullptr;

        return do_pop();
    }


    Optional<Worker::Task> Worker::pop() {
        MutexLock l(_mutex);

        if (_queue.empty())
            return nullptr;

        return do_pop();
    }


    Worker::Task Worker::do_pop() {
        Task task = std::move(_queue.front());
        _queue.pop_front();
        return task;
    }

}