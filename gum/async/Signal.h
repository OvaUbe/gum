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

#include <gum/async/AsyncFunction.h>
#include <gum/async/ITaskQueue.h>
#include <gum/compare/OwnerLess.h>
#include <gum/concurrency/CancellableFunction.h>
#include <gum/concurrency/DummyMutex.h>
#include <gum/concurrency/LifeToken.h>
#include <gum/concurrency/Mutex.h>
#include <gum/token/FunctionToken.h>
#include <gum/token/IGuardedTokenPool.h>
#include <gum/Try.h>

#include <boost/container/flat_set.hpp>

namespace gum {

    namespace signal {

        GUM_ENUM(ThreadSafety,
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

        GUM_LOGGER_SINGLETON(SignalLogger);


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

            virtual Token connect(SlotType&& slot) = 0;
            virtual Token connect(SlotType&& slot, ITaskQueueRef const& task_queue) = 0;

            virtual void connect(SlotType&& slot, IGuardedTokenPool& token_pool) = 0;
            virtual void connect(SlotType&& slot, ITaskQueueRef const& task_queue, IGuardedTokenPool& token_pool) = 0;

            virtual void populate(SlotType const& slot) const = 0;
        };


        template < typename Signature_ >
        class DummySignalHandle : public virtual ISignalHandle<Signature_> {
            using SlotType = signal::SlotType<Signature_>;

        public:
            Token connect(SlotType&&) override {
                return Token();
            }

            Token connect(SlotType&&, ITaskQueueRef const&) override {
                return Token();
            }

            void connect(SlotType&&, IGuardedTokenPool&) override { }

            void connect(SlotType&&, ITaskQueueRef const&, IGuardedTokenPool&) override { }

            void populate(SlotType const&) const override { }
        };


        template < typename Signature_, signal::ThreadSafety::Enum ThreadSafety_ >
        class SignalImpl : public virtual ISignalHandle<Signature_> {
            using Self = SignalImpl<Signature_, ThreadSafety_>;
            GUM_DECLARE_PTR(Self);
            GUM_DECLARE_REF(Self);

            using SlotType = signal::SlotType<Signature_>;
            GUM_DECLARE_PTR(SlotType);
            GUM_DECLARE_REF(SlotType);

            using PopulatorType = signal::PopulatorType<SlotType>;

            using MutexType = signal::MutexType<ThreadSafety_>;
            using MutexLockType = GenericMutexLock<MutexType>;
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

            using Slots = boost::container::flat_set<SlotTypeWeakPtr, OwnerLess>;

        private:
            SelfWeakPtr         _self_weak;
            PopulatorType       _populator;

            Slots               _slots;
            MutexType           _mutex;

        public:
            SignalImpl(PopulatorType&& populator)
                :   _populator(std::move(populator))
            { }

            void set_self(SelfWeakPtr const& self_weak) {
                _self_weak = self_weak;
            }

            Token connect(SlotType&& slot) override {
                LifeToken life_token = LifeTokenFactoryType()();
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_cancellable(std::move(slot), life_token.get_handle()));
                return make_token<Connection>(connect(slot_ref), std::move(life_token));
            }

            Token connect(SlotType&& slot, ITaskQueueRef const& task_queue) override {
                LifeToken life_token = LifeTokenFactoryType()();
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_async(make_cancellable(std::move(slot), life_token.get_handle()), task_queue));
                return make_token<Connection>(connect(slot_ref), std::move(life_token));
            }

            void connect(SlotType&& slot, IGuardedTokenPool& token_pool) override {
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_cancellable(std::move(slot), token_pool.get_handle()));
                token_pool += connect(slot_ref);
            }

            void connect(SlotType&& slot, ITaskQueueRef const& task_queue, IGuardedTokenPool& token_pool) override {
                SlotTypeRef const slot_ref = make_shared_ref<SlotType>(make_async(make_cancellable(std::move(slot), token_pool.get_handle()), task_queue));
                token_pool += connect(slot_ref);
            }


            void populate(SlotType const& slot) const override {
                MutexLockType l(_mutex);
                do_populate(slot);
            }

            template < typename ...Args__ >
            void invoke(Args__&& ...args) const {
                MutexLockType l(_mutex);

                for (auto const& slot : _slots) {
                    GUM_TRY_LOGGER("Uncaught exception from slot", LogLevel::Error, detail::SignalLogger::get(), (*slot.lock())((args)...));
                }
            }

            MutexType const& get_mutex() const {
                return _mutex;
            }

        private:
            Token connect(SlotTypeRef const& slot_ref) {
                do_connect(slot_ref);

                SelfRef const self_ref = _self_weak.lock();
                return make_token<FunctionToken>([self_ref, slot_ref]{ self_ref->pop_slot(slot_ref); });
            }

            void do_connect(SlotTypeRef const& slot_ref) {
                MutexLockType l(_mutex);

                do_populate(*slot_ref);
                push_slot(slot_ref);
            }

            void do_populate(SlotType const& slot) const {
                GUM_TRY_LOGGER("Uncaught exception from populator", LogLevel::Error, detail::SignalLogger::get(),
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

    }


    template < typename Signature_ >
    class SignalHandle {
        using Impl = detail::ISignalHandle<Signature_>;
        GUM_DECLARE_REF(Impl);

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


    template < typename Signature_, signal::ThreadSafety::Enum ThreadSafety_ >
    class BasicSignal {
        using Impl = detail::SignalImpl<Signature_, ThreadSafety_>;
        GUM_DECLARE_REF(Impl);

        using SignalHandleType = SignalHandle<Signature_>;

        using SlotType = signal::SlotType<Signature_>;
        using PopulatorType = signal::PopulatorType<SlotType>;

        using MutexType = signal::MutexType<ThreadSafety_>;

    private:
        ImplRef _impl;

    public:
        BasicSignal(PopulatorType&& populator = [](auto){ })
            :   _impl(make_shared_ref<Impl>(std::move(populator)))
        { _impl->set_self(_impl); }

        BasicSignal(BasicSignal const&) = delete;

        BasicSignal& operator=(BasicSignal const&) = delete;

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
    using Signal = BasicSignal<Signature_, signal::ThreadSafety::Synchronized>;

    template < typename Signature_ >
    using UnsynchronizedSignal = BasicSignal<Signature_, signal::ThreadSafety::Unsynchronized>;

    using SignalMutex = signal::SynchronizedMutexType;
    using SignalLock = GenericMutexLock<SignalMutex>;

}
