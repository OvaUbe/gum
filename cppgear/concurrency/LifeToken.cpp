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

#include <cppgear/concurrency/LifeToken.h>

#include <cppgear/concurrency/ConditionVariable.h>
#include <cppgear/concurrency/DummyCancellationHandle.h>
#include <cppgear/concurrency/Mutex.h>
#include <cppgear/concurrency/Thread.h>

#include <atomic>

namespace cppgear {

    namespace {

        CPPGEAR_LOGGER_SINGLETON(LifeTokenLogger);


        struct ILifeHandleImpl : public virtual ILifeHandle {
            virtual void release() = 0;
        };
        CPPGEAR_DECLARE_REF(ILifeHandleImpl);


        class DefaultLifeHandle : public virtual ILifeHandleImpl {
            mutable std::atomic<bool>   _alive;
            mutable ThreadInfoPtr       _owner;

            Mutex                       _mutex;
            ConditionVariable           _condition_varaible;

        public:
            DefaultLifeHandle()
                :   _alive(true)
            { }

            bool lock() const override {
                if (!_alive)
                    return false;

                MutexLock l(_mutex);

                if (!_alive)
                    return false;

                wait();

                _owner = Thread::get_own_info();

                return true;
            }

            void unlock() const override {
                if (!_alive)
                    return;

                MutexLock l(_mutex);

                if (!_alive)
                    return;

                _owner = nullptr;
                _condition_varaible.broadcast();
            }

            void release() override {
                if (!_alive)
                    return;

                MutexLock l(_mutex);

                if (!_alive)
                    return;

                wait();

                _alive = false;
            }

        private:
            void wait() const {
                const Seconds Threshold = Seconds(3);
                const ElapsedTime elapsed;

                while (_owner) {
                    if (!_condition_varaible.wait_for(_mutex, Threshold, *DummyCancellationHandle()))
                        continue;

                    LifeTokenLogger::get().warning()
                        << "Could not acquire life token owned by: " << _owner << " for " << elapsed.elapsed_to<Seconds>() << "."
                        << " There is probably a deadlock.\nBacktrace: " << Backtrace();
                }
            }
        };
        CPPGEAR_DECLARE_REF(DefaultLifeHandle);


        struct ReleasedLifeHandle : public virtual ILifeHandleImpl {
            bool lock() const override {
                return false;
            }

            void unlock() const override { }

            void release() override { }
        };
        CPPGEAR_DECLARE_REF(ReleasedLifeHandle);


        struct DummyLifeHandle : public virtual ILifeHandleImpl {
            bool lock() const override {
                return true;
            }

            void unlock() const override { }

            void release() override { }
        };
        CPPGEAR_DECLARE_REF(DummyLifeHandle);

    }


    class LifeToken::Impl {
        ILifeHandleImplRef _handle;

    public:
        Impl(ILifeHandleImplRef&& handle)
            :   _handle(std::move(handle))
        { }

        ~Impl() {
            _handle->release();
        }

        ILifeHandleRef get_handle() const {
            return _handle;
        }
    };


    LifeToken::LifeToken(ImplUniqueRef&& impl)
        :   _impl(std::move(impl))
    { }


    LifeToken::~LifeToken() { }


    LifeToken::LifeToken(LifeToken&& other)
        :   _impl(std::move(other._impl))
    { }


    LifeToken& LifeToken::operator=(LifeToken&& other) {
        _impl = std::move(other._impl);
        return self;
    }


    LifeToken LifeToken::make_valid() {
        return LifeToken(make_unique_ref<Impl>(DefaultLifeHandleRef()));
    }


    LifeToken LifeToken::make_released() {
        return LifeToken(make_unique_ref<Impl>(ReleasedLifeHandleRef()));
    }


    LifeToken LifeToken::make_dummy() {
        return LifeToken(make_unique_ref<Impl>(DummyLifeHandleRef()));
    }


    ILifeHandleRef LifeToken::get_handle() const {
        return _impl->get_handle();
    }


    void LifeToken::release() {
        _impl = make_unique_ref<Impl>(ReleasedLifeHandleRef());
    }

}
