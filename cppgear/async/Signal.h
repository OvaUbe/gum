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

#include <cppgear/async/AsyncFunction.h>
#include <cppgear/async/ITaskQueue.h>
#include <cppgear/compare/OwnerLess.h>
#include <cppgear/concurrency/CancellableFunction.h>
#include <cppgear/concurrency/DummyMutex.h>
#include <cppgear/concurrency/LifeToken.h>
#include <cppgear/concurrency/Mutex.h>
#include <cppgear/container/FlatSet.h>
#include <cppgear/token/FunctionToken.h>
#include <cppgear/token/IGuardedTokenPool.h>
#include <cppgear/Try.h>

namespace cppgear {

    namespace signal {

        CPPGEAR_ENUM(ThreadSafety,
            Synchronized,
            Unsynchronized
        );


        template < typename Signature_ >
        using SlotType = std::function<Signature_>;

        template < typename SlotType_ >
        using PopulatorType = std::function<void(SlotType_ const&)>;


        using SynchronizedMutexType = RecursiveMutex;
        using UnsynchronizedMutexType = DummyMutex;

        template < typename ThreadSafety::Enum ThreadSafety_ >
        using MutexType = std::conditional_t<ThreadSafety_ == ThreadSafety::Synchronized, SynchronizedMutexType, UnsynchronizedMutexType>;

    }


    namespace detail {

        CPPGEAR_LOGGER_SINGLETON(SignalLogger);

        template < signal::ThreadSafety::Enum >
        struct LifeTokenFactory;

        template < >
        struct LifeTokenFactory<signal::ThreadSafety::Synchronized> {
            LifeToken operator()() const {
                return LifeToken::make_synchronized();
            }
        };

        template < >
        struct LifeTokenFactory<signal::ThreadSafety::Unsynchronized> {
            LifeToken operator()() const {
                return LifeToken::make_unsynchronized();
            }
        };


        template < typename Signature_ >
        struct ISignalHandle {
            using SlotType = signal::SlotType<Signature_>;

        public:
            virtual ~ISignalHandle() { }

            virtual Token connect(SlotType&& slot) const = 0;
            virtual Token connect(SlotType&& slot, ITaskQueueRef const& task_queue) const = 0;

            virtual void connect(SlotType&& slot, IGuardedTokenPool& token_pool) const = 0;
            virtual void connect(SlotType&& slot, ITaskQueueRef const& task_queue, IGuardedTokenPool& token_pool) const = 0;

            virtual void populate(SlotType const& slot) const = 0;
        };


        template < signal::ThreadSafety::Enum ThreadSafety_, typename Signature_ >
        class SignalImpl {
        public:
            using Signature = Signature_;

            using SlotType = signal::SlotType<Signature>;
            using PopulatorType = signal::PopulatorType<SlotType>;

            using MutexType = signal::MutexType<ThreadSafety_>;
            using MutexLockType = GenericMutexLock<MutexType>;

        private:
            using Self = SignalImpl<ThreadSafety_, Signature>;
            CPPGEAR_DECLARE_REF(Self);

            CPPGEAR_DECLARE_PTR(SlotType);
            CPPGEAR_DECLARE_REF(SlotType);

            using Slots = FlatSet<SlotTypeWeakPtr, OwnerLess>;

        private:
            PopulatorType       _populator;

            Slots               _slots;
            MutexType           _mutex;

        public:
            SignalImpl(PopulatorType&& populator)
                :   _populator(std::move(populator))
            { }

            Token connect(SlotTypeRef const& slot_ref, SelfRef const& self_ref) {
                do_connect(slot_ref);
                return make_token<FunctionToken>([self_ref, slot_ref]{ self_ref->pop_slot(slot_ref); });
            }

            void populate(SlotType const& slot) const {
                MutexLockType l(_mutex);
                do_populate(slot);
            }

            template < typename ...Args__ >
            void invoke(Args__&& ...args) const {
                MutexLockType l(_mutex);

                for (auto const& slot : _slots) {
                    CPPGEAR_TRY_LOGGER("Uncaught exception from slot", LogLevel::Error, detail::SignalLogger::get(), (*slot.lock())((args)...));
                }
            }

            MutexType const& get_mutex() const {
                return _mutex;
            }

        private:
            void do_connect(SlotTypeRef const& slot_ref) {
                MutexLockType l(_mutex);

                do_populate(*slot_ref);
                push_slot(slot_ref);
            }

            void do_populate(SlotType const& slot) const {
                CPPGEAR_TRY_LOGGER("Uncaught exception from populator", LogLevel::Error, detail::SignalLogger::get(),
                    _populator(try_(slot, LogLevel::Error, "Uncaught exception from slot", detail::SignalLogger::get()));
                );
            }

            void push_slot(SlotTypeRef const& slot) {
                _slots.insert(SlotTypeWeakPtr(slot));
            }

            void pop_slot(SlotTypeRef const& slot) {
                MutexLockType l(_mutex);
                _slots.erase(SlotTypeWeakPtr(slot));
            }
        };


        template < typename Signature_ >
        class DummySignalHandle : public virtual ISignalHandle<Signature_> {
            using SlotType = signal::SlotType<Signature_>;

        public:
            Token connect(SlotType&&) const override {
                return Token();
            }

            Token connect(SlotType&&, ITaskQueueRef const&) const override {
                return Token();
            }

            void connect(SlotType&&, IGuardedTokenPool&) const override { }

            void connect(SlotType&&, ITaskQueueRef const&, IGuardedTokenPool&) const override { }

            void populate(SlotType const&) const override { }
        };


        template < signal::ThreadSafety::Enum ThreadSafety_, typename Signature_ >
        class DefaultSignalHandle : public virtual ISignalHandle<Signature_> {
            using Impl = detail::SignalImpl<ThreadSafety_, Signature_>;
            CPPGEAR_DECLARE_REF(Impl);

            using SlotType = signal::SlotType<Signature_>;
            CPPGEAR_DECLARE_REF(SlotType);

            using PopulatorType = typename Impl::PopulatorType;

            using MutexType = typename Impl::MutexType;
            using LifeTokenFactoryType = LifeTokenFactory<ThreadSafety_>;

            class Connection : public virtual IToken {
                Token           _slot_token;
                LifeToken       _life_token;

            public:
                Connection(Token&& slot_token, LifeToken&& life_token)
                    :   _slot_token(std::move(slot_token)),
                        _life_token(std::move(life_token))
                { }
            };

        private:
            ImplRef _impl;

        public:
            DefaultSignalHandle(ImplRef const& impl)
                :   _impl(impl)
            { }

            Token connect(SlotType&& slot) const override {
                LifeToken life_token = LifeTokenFactoryType()();
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_cancellable(std::move(slot), life_token.get_handle()));
                return make_token<Connection>(_impl->connect(slot_ref, _impl), std::move(life_token));
            }


            Token connect(SlotType&& slot, ITaskQueueRef const& task_queue) const override {
                LifeToken life_token = LifeTokenFactoryType()();
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_async(make_cancellable(std::move(slot), life_token.get_handle()), task_queue));
                return make_token<Connection>(_impl->connect(slot_ref, _impl), std::move(life_token));
            }

            void connect(SlotType&& slot, IGuardedTokenPool& token_pool) const override {
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_cancellable(std::move(slot), token_pool.get_handle()));
                token_pool += _impl->connect(slot_ref, _impl);
            }

            void connect(SlotType&& slot, ITaskQueueRef const& task_queue, IGuardedTokenPool& token_pool) const override {
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_async(make_cancellable(std::move(slot), token_pool.get_handle()), task_queue));
                token_pool += _impl->connect(slot_ref, _impl);
            }

            void populate(SlotType const& slot) const override {
                _impl->populate(slot);
            }

            template < typename ...Args__ >
            void invoke(Args__&& ...args) const {
                _impl->invoke(std::forward<Args__>(args)...);
            }

            MutexType const& get_mutex() const {
                return _impl->get_mutex();
            }
        };

    }


    template < typename Signature_ >
    class SignalHandle {
        using Impl = detail::ISignalHandle<Signature_>;
        CPPGEAR_DECLARE_REF(Impl);

        using DummyHandleType = detail::DummySignalHandle<Signature_>;

        using SlotType = typename Impl::SlotType;

    private:
        ImplRef _impl;

    public:
        SignalHandle(ImplRef const& impl = make_shared_ref<DummyHandleType>())
            :   _impl(impl)
        { }

        Token connect(SlotType&& slot) const {
            return _impl->connect(std::move(slot));
        }

        Token connect(SlotType&& slot, ITaskQueueRef const& task_queue) const {
            return _impl->connect(std::move(slot), task_queue);
        }

        void connect(SlotType&& slot, IGuardedTokenPool& token_pool) const {
            _impl->connect(std::move(slot), token_pool);
        }

        void connect(SlotType&& slot, ITaskQueueRef const& task_queue, IGuardedTokenPool& token_pool) const {
            _impl->connect(std::move(slot), task_queue, token_pool);
        }

        void populate(SlotType const& slot) const {
            _impl->populate(slot);
        }
    };


    template < signal::ThreadSafety::Enum ThreadSafety_, typename Signature_ >
    class BasicSignal {
        using Impl = detail::SignalImpl<ThreadSafety_, Signature_>;
        CPPGEAR_DECLARE_REF(Impl);

        using SignalHandleImpl = detail::DefaultSignalHandle<ThreadSafety_, Signature_>;
        CPPGEAR_DECLARE_REF(SignalHandleImpl);

        using SignalHandleType = SignalHandle<Signature_>;

        using SlotType = typename Impl::SlotType;
        using PopulatorType = typename Impl::PopulatorType;

        using MutexType = typename Impl::MutexType;

    private:
        SignalHandleImplRef _impl;

    public:
        BasicSignal(PopulatorType&& populator = [](auto){ })
            :   _impl(make_shared_ref<SignalHandleImpl>(make_shared_ref<Impl>(std::move(populator))))
        { }

        Token connect(SlotType&& slot) const {
            return _impl->connect(std::move(slot));
        }

        Token connect(SlotType&& slot, ITaskQueueRef const& task_queue) const {
            return _impl->connect(std::move(slot), task_queue);
        }

        void connect(SlotType&& slot, IGuardedTokenPool& token_pool) const {
            _impl->connect(std::move(slot), token_pool);
        }

        void connect(SlotType&& slot, ITaskQueueRef const& task_queue, IGuardedTokenPool& token_pool) const {
            _impl->connect(std::move(slot), task_queue, token_pool);
        }

        void populate(SlotType const& slot) const {
            _impl->populate(slot);
        }

        template < typename ...Args__ >
        void operator()(Args__&& ...args) const {
            _impl->invoke(std::forward<Args__>(args)...);
        }

        MutexType const& get_mutex() const {
            return _impl->get_mutex();
        }

        SignalHandleType get_handle() const {
            return SignalHandleType(_impl);
        }
    };


    template < typename Signature_ >
    using Signal = BasicSignal<signal::ThreadSafety::Synchronized, Signature_>;

    template < typename Signature_ >
    using UnsynchronizedSignal = BasicSignal<signal::ThreadSafety::Unsynchronized, Signature_>;


    using SignalLock = GenericMutexLock<signal::SynchronizedMutexType>;

}
