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

#include <gum/concurrency/CancellationToken.h>

#include <gum/concurrency/ConditionVariable.h>
#include <gum/concurrency/DummyCancellationHandle.h>
#include <gum/concurrency/Mutex.h>
#include <gum/token/FunctionToken.h>
#include <gum/Optional.h>

#include <atomic>

namespace gum {

    class CancellationToken::Impl {
        using CancellationHandler = ICancellationToken::CancellationHandler;

    public:
        Mutex                               _mutex;
        ConditionVariable                   _cancel_condition;
        Optional<CancellationHandler>       _cancellation_handler;
        std::atomic<bool>                   _is_cancelled;
        bool                                _cancel_in_progress;

    public:
        Impl()
            :   _is_cancelled(false),
                _cancel_in_progress(false)
        { }

        bool is_cancelled() const {
            return _is_cancelled;
        }

        void sleep(Duration const& duration) const {
            if (_is_cancelled)
                return;

            MutexLock l(_mutex);

            while (!_is_cancelled)
                if (_cancel_condition.wait_for(_mutex, duration, *DummyCancellationHandle()) == ConditionVariable::WaitResult::TimedOut)
                    break;
        }

        bool on_cancelled(CancellationHandler const& cancellation_handler) {
            if (_is_cancelled)
                return false;

            MutexLock l(_mutex);

            if (_is_cancelled)
                return false;

            GUM_CHECK(!_cancellation_handler, "Cancellation handler already registered");
            _cancellation_handler = cancellation_handler;
            return true;
        }

        void unregister_cancellation_handler() {
            MutexLock l(_mutex);

            _cancellation_handler.reset();

            while (_cancel_in_progress)
                _cancel_condition.wait(_mutex, *DummyCancellationHandle());
        }

        void cancel() {
            if (_is_cancelled)
                return;

            Optional<CancellationHandler> cancellation_handler;
            {
                MutexLock l(_mutex);

                if (_is_cancelled)
                    return;

                _is_cancelled = true;

                _cancel_in_progress = true;
                _cancellation_handler.swap(cancellation_handler);
            }

            if (cancellation_handler)
                (*cancellation_handler)();

            {
                MutexLock l(_mutex);

                _cancel_in_progress = false;
                _cancel_condition.broadcast();
            }
        }

        void reset() {
            MutexLock l(_mutex);

            GUM_CHECK(!_cancellation_handler && !_cancel_in_progress, "reset() called while cancellation token is being used");
            _is_cancelled = false;
        }
    };


    CancellationToken::CancellationToken()
        :   _impl(make_shared_ref<Impl>()) { }


    CancellationToken::operator bool() const {
        return !_impl->is_cancelled();
    }


    void CancellationToken::sleep(Duration const& duration) const {
        _impl->sleep(duration);
    }


    Token CancellationToken::on_cancelled(CancellationHandler const& cancellation_handler) {
        if (_impl->on_cancelled(cancellation_handler))
            return make_token<FunctionToken>([impl=_impl]{ impl->unregister_cancellation_handler(); });
        return Token();
    }


    void CancellationToken::cancel() {
        _impl->cancel();
    }


    void CancellationToken::reset() {
        _impl->reset();
    }


    Token CancellationToken::get_cancellator() const {
        return make_token<FunctionToken>([impl=_impl]{ impl->cancel(); });
    }

}
