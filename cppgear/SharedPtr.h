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

#include <cppgear/UniquePtr.h>
#include <cppgear/UniqueReference.h>

namespace cppgear {

    template < typename >
    class SharedPtr;

    template < typename >
    class SharedReference;


    template < typename Value_ >
    class WeakPtr {
        using SmartpointerType = std::weak_ptr<Value_>;

        template < typename >
        friend class SharedPtr;

        template < typename >
        friend class SharedReference;

    private:
        SmartpointerType _wrapped;

    public:
        WeakPtr() { }

        WeakPtr(WeakPtr const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_ >
        WeakPtr(WeakPtr<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        WeakPtr(WeakPtr&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        WeakPtr(WeakPtr<Compatible_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        WeakPtr(SharedPtr<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_ >
        WeakPtr(SharedReference<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        WeakPtr& operator=(WeakPtr const& other) {
            _wrapped = other._wrapped;
            return self;
        }

        template < typename Compatible_ >
        WeakPtr& operator=(WeakPtr<Compatible_> const& other) {
            _wrapped = other._wrapped;
            return self;
        }

        template < typename Compatible_ >
        WeakPtr& operator=(SharedPtr<Compatible_> const& other) {
            _wrapped = other._wrapped;
            return self;
        }

        WeakPtr& operator=(WeakPtr&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_ >
        WeakPtr& operator=(WeakPtr<Compatible_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        void reset() {
            _wrapped.reset();
        }

        void swap(WeakPtr& other) {
            _wrapped.swap(other._wrapped);
        }

        long use_count() const {
            return _wrapped.use_count();
        }

        bool expired() const {
            return _wrapped.expired();
        }

        SharedPtr<Value_> lock() const {
            return SharedPtr<Value_>(_wrapped.lock());
        }

        template < typename Other_ >
        bool owner_before(Other_ const& other) {
            return _wrapped.owner_before(other);
        }

        friend void swap(WeakPtr& lhs, WeakPtr& rhs) {
            lhs.swap(rhs);
        }
    };


    template < typename Value_ >
    class SharedPtr {
        using SmartpointerType = std::shared_ptr<Value_>;

        template < typename >
        friend class WeakPtr;

        template < typename >
        friend class SharedReference;

        template < typename Value__, typename ...Args_ >
        friend SharedPtr<Value__> make_shared(Args_&&...);

    public:
        using pointer = Value_*;
        using reference = Value_&;

    private:
        SmartpointerType _wrapped;

    private:
        SharedPtr(SmartpointerType&& wrapped)
            :   _wrapped(std::move(wrapped))
        { }

    public:
        SharedPtr() { }

        SharedPtr(std::nullptr_t) { }

        template < typename Compatible_ >
        SharedPtr(Compatible_* ptr)
            : _wrapped(ptr)
        { }

        template < typename Compatible_, typename Deleter_ >
        SharedPtr(Compatible_* ptr, Deleter_ deleter)
            :   _wrapped(ptr, deleter)
        { }

        template < typename Deleter_ >
        SharedPtr(std::nullptr_t ptr, Deleter_ deleter)
            :   _wrapped(ptr, deleter)
        { }

        template < typename Compatible_, typename Deleter_, typename Allocator_ >
        SharedPtr(Compatible_* ptr, Deleter_ deleter, Allocator_ allocator)
            :   _wrapped(ptr, deleter, allocator)
        { }

        template < typename Deleter_, typename Allocator_ >
        SharedPtr(std::nullptr_t ptr, Deleter_ deleter, Allocator_ allocator)
            :   _wrapped(ptr, deleter, allocator)
        { }

        template < typename Compatible_ >
        SharedPtr(SharedPtr<Compatible_> const& other, Value_* ptr)
            :   _wrapped(other._wrapped, ptr)
        { }

        template < typename Compatible_ >
        SharedPtr(SharedReference<Compatible_> const& other, Value_* ptr)
            :   _wrapped(other._wrapped, ptr)
        { }

        SharedPtr(SharedPtr const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_ >
        SharedPtr(SharedPtr<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_ >
        SharedPtr(SharedReference<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        SharedPtr(SharedPtr&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        SharedPtr(SharedPtr<Compatible_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        SharedPtr(SharedReference<Compatible_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        explicit SharedPtr(WeakPtr<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_, typename Deleter_ >
        SharedPtr(UniquePtr<Compatible_, Deleter_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_, typename Deleter_ >
        SharedPtr(UniqueReference<Compatible_, Deleter_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        SharedPtr& operator=(const SharedPtr& other) {
            _wrapped = other._wrapped;
            return self;
        }

        template < typename Compatible_ >
        SharedPtr& operator=(SharedPtr<Compatible_> const& other) {
            _wrapped = other._wrapped;
            return self;
        }

        template < typename Compatible_ >
        SharedPtr& operator=(SharedReference<Compatible_> const& other) {
            _wrapped = other._wrapped;
            return self;
        }

        SharedPtr& operator=(SharedPtr&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_ >
        SharedPtr& operator=(SharedPtr<Compatible_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_ >
        SharedPtr& operator=(SharedReference<Compatible_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_, typename Deleter_ >
        SharedPtr& operator=(UniquePtr<Compatible_, Deleter_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_, typename Deleter_ >
        SharedPtr& operator=(UniqueReference<Compatible_, Deleter_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        void reset() {
            _wrapped.reset();
        }

        template < typename Compatible_ >
        void reset(Compatible_* ptr) {
            _wrapped.reset(ptr);
        }

        template < typename Compatible_, typename Deleter_ >
        void reset(Compatible_* ptr, Deleter_ deleter) {
            _wrapped.reset(ptr, deleter);
        }

        template< typename Compatible_, typename Deleter_, typename Allocator_ >
        void reset(Compatible_* ptr, Deleter_ deleter, Allocator_ allocator) {
            _wrapped.reset(ptr, deleter, allocator);
        }

        void swap(SharedPtr& other) {
            _wrapped.swap(other._wrapped);
        }

        pointer get() const {
            return _wrapped.get();
        }

        reference operator*() const {
            check_ptr();
            return *get();
        }

        pointer operator->() const {
            check_ptr();
            return *get();
        }

        long use_count() const {
            return _wrapped.use_count();
        }

        bool unique() const {
            return _wrapped.unique();
        }

        explicit operator bool() const {
            return (bool)_wrapped;
        }

        template < typename Other_ >
        bool owner_before(Other_ const& other) {
            return _wrapped.owner_before(other);
        }

        friend void swap(SharedPtr& lhs, SharedPtr& rhs) {
            lhs.swap(rhs);
        }

    private:
        void check_ptr() const {
            CPPGEAR_CHECK(get(), NullPointerException());
        }
    };


    template < typename Value_, typename ...Args_ >
    SharedPtr<Value_> make_shared(Args_&&... args) {
        return SharedPtr<Value_>(std::make_shared<Value_>(std::forward<Args_>(args)...));
    }


#   define CPPGEAR_DECLARE_PTR(Type_) \
        using Type_##Ptr = SharedPtr<Type_>; \
        using Type_##ConstPtr = SharedPtr<const Type_>; \
        using Type_##WeakPtr = WeakPtr<Type_>; \
        using Type_##ConstWeakPtr = WeakPtr<const Type_>

}

namespace std {

    template < typename Value_ >
    struct hash<cppgear::SharedPtr<Value_>> {
        using SharedPtrType = cppgear::SharedPtr<Value_>;

        using argument_type = SharedPtrType;
        using result_type = size_t;

        result_type operator()(argument_type const& ptr) const {
            return std::hash<typename SharedPtrType::pointer>()(ptr.get());
        }
    };

}
