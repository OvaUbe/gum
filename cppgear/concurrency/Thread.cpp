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
#include <cppgear/Try.h>

#include <iostream>

namespace cppgear {

    namespace {

        thread_local String t_thread_name = "__UndefinedThread";

    }


    CPPGEAR_DEFINE_LOGGER(Thread);


    Thread::~Thread() {
        CPPGEAR_TRY_LEVEL("Join failed", error, dtor());
    }


    String Thread::get_own_name() {
        return t_thread_name;
    }


    ThreadId Thread::get_own_id() {
        return std::this_thread::get_id();
    }


    void Thread::sleep(SystemClock::duration const& duration) {
        std::this_thread::sleep_for(duration);
    }


    String Thread::get_name() const {
        return _name;
    }


    ThreadId Thread::get_id() const {
        return _impl.get_id();
    }


    void Thread::thread_func() {
        CPPGEAR_TRY_LEVEL("Uncaught exception from internal thread function", error, _thread_func());
    }


    void Thread::_thread_func() {
        t_thread_name = _name;

        CPPGEAR_TRY_LEVEL("Uncaught exception from client thread function", error, _task(_cancellation_token));
    }


    void Thread::dtor() {
        _cancellation_token.cancel();
        _impl.join();
    }

}
