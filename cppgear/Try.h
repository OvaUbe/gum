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

#include <cppgear/log/GlobalLogger.h>

namespace cppgear {

#   define CPPGEAR_TRY_LOGGER(Message_, LogLevel_, Logger_, ...) \
        try {\
            __VA_ARGS__; \
        } \
        catch (std::exception const& ex) { \
            Logger_.LogLevel_() << Message_ << ":\n" << ex; \
        } \
        catch (...) { \
            Logger_.LogLevel_() << Message_ << ":\n<unknown exception>"; \
        }

#   define CPPGEAR_TRY_LEVEL(Message_, LogLevel_, ...) \
        CPPGEAR_TRY_LOGGER(Message_, LogLevel_, s_logger, __VA_ARGS__)

#   define CPPGEAR_TRY(Message_, ...) \
        CPPGEAR_TRY_LEVEL(Message_, warning, __VA_ARGS__)


    template < typename Wrapped_ >
    class Try {
        Wrapped_ _wrapped;

    public:
        template < typename Wrapped__ >
        Try(Wrapped__&& wrapped)
            :   _wrapped(std::forward<Wrapped__>(wrapped))
        { }

        template < typename ...Args_ >
        auto operator()(Args_&& ...args) {
            CPPGEAR_TRY_LOGGER("Uncaught exception", warning, GlobalLogger::get(), return _wrapped(std::forward<Args_>(args)...));
        }
    };

    template < typename Wrapped_ >
    auto try_(Wrapped_&& wrapped) {
        return Try<std::decay_t<Wrapped_>>(std::forward<Wrapped_>(wrapped));
    }

}
