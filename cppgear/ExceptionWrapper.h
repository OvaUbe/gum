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

#include <exception>
#include <iostream>

namespace cppgear {

    template < typename Wrapped_ >
    class ExceptionWrapper {
    public:
        template < typename Wrapped__ >
        ExceptionWrapper(Wrapped__&& wrapped) :
            m_wrapped(std::forward<Wrapped__>(wrapped)) { }

        template < typename ...Args_ >
        void operator()(Args_&& ...args) {
            try {
                m_wrapped(std::forward<Args_>(args)...);
            } catch (std::exception const& ex) {
                std::cout << "Uncaught exception in exception wrapper: " << ex.what() << std::endl;
            } catch (...) {
                std::cout << "Unknown exception caught in exception wrapper" << std::endl;
            }
        }

    private:
        Wrapped_ m_wrapped;
    };

    template < typename Wrapped_ >
    ExceptionWrapper<Wrapped_> make_exception_wrapper(Wrapped_&& wrapped) {
        return ExceptionWrapper<Wrapped_>(std::forward<Wrapped_>(wrapped));
    }

}
