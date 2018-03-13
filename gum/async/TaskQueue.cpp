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

#include <gum/async/TaskQueue.h>

#include <gum/functional/Invoker.h>
#include <gum/Maybe.h>
#include <gum/Try.h>

namespace gum {

    GUM_DEFINE_LOGGER(TaskQueue);


    void TaskQueue::push(Task&& task) {
        MutexLock l(_mutex);
        _queue.push_back([task = std::move(task)]{ GUM_TRY_LEVEL("Uncaught exception in queued task", LogLevel::Error, task()); });
    }


    void TaskQueue::run() {
        Queue queue;
        {
            MutexLock l(_mutex);

            if (_queue.empty())
                return;
            _queue.swap(queue);
        }

        while (!queue.empty()) {
            Task task = std::move(queue.front());
            queue.pop_front();
            task();
        }
    }

}
