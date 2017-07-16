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

#include <cppgear/concurrency/Thread.h>
#include <cppgear/string/ToString.h>
#include <cppgear/Try.h>

namespace cppgear {

    namespace {

        thread_local ThreadInfoRef t_thread_info = make_shared_ref<ThreadInfo>(ThreadId(), make_shared_ref<String>("__UndefinedThread"));

    }


    CPPGEAR_DEFINE_LOGGER(Thread);


    Thread::~Thread() {
        CPPGEAR_TRY_LEVEL("Join failed", error, dtor());
    }


    ThreadInfoRef Thread::get_own_info() {
        return t_thread_info;
    }


    void Thread::sleep(Duration const& duration) {
        std::this_thread::sleep_for(duration);
    }


    void Thread::sleep(Duration const& duration, ICancellationHandle& handle) {
        handle.sleep(duration);
    }


    ThreadInfoRef Thread::get_info() const {
        return _info;
    }


    String Thread::to_string() const {
        return String() << "Thread: " << get_info();
    }


    void Thread::thread_func() {
        CPPGEAR_TRY_LEVEL("Uncaught exception from internal thread function", error, _thread_func());
    }


    void Thread::_thread_func() {
        t_thread_info = _info;

        s_logger.info() << get_info() << " spawned.";

        CPPGEAR_TRY_LEVEL("Uncaught exception from client thread function", error, _task(_cancellation_token));
    }


    void Thread::dtor() {
        ThreadInfoRef const info = get_info();

        _cancellation_token.cancel();
        _impl.join();

        s_logger.info() << info << " joined.";
    }

}
