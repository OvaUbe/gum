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

#include <iostream>

namespace cppgear {

    namespace {

        thread_local std::string t_thread_name = "__UndefinedThread";

    }


    Thread::~Thread() {
        try {
            dtor();
        }
        catch (std::exception const& ex) {
            std::cout << "Thread join failed: " << ex.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception in thread destructor." << std::endl;
        }
    }


    std::string Thread::get_own_name() {
        return t_thread_name;
    }


    std::string Thread::get_name() const {
        return _name;
    }


    void Thread::thread_func() {
        t_thread_name = _name;

        try {
            _task(_cancellation_token);
        }
        catch (std::exception const& ex) {
            std::cout << "Uncaught exception in Thread::thread_func(): " << ex.what() << std::endl;
        } catch (...) {
            std::cout << "Unknown exception in Thread::thread_func()." << std::endl;
        }
    }


    void Thread::dtor() {
        _cancellation_token.cancel();
        _impl.join();
    }

}
