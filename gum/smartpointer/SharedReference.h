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

#include <gum/smartpointer/UniquePtr.h>
#include <gum/smartpointer/UniqueReference.h>

namespace gum {

    template < typename >
    class WeakPtr;

    template < typename >
    class SharedPtr;


    template < typename Value_ >
    class SharedReference {
        using SmartpointerType = std::shared_ptr<Value_>;

        template < typename >
        friend class WeakPtr;

        template < typename >
        friend class SharedReference;

        template < typename >
        friend class SharedPtr;

        template < typename Value__, typename ...Args_ >
        friend SharedReference<Value__> make_shared_ref(Args_&&...);

    public:
        using value_type = Value_;
        using pointer = Value_*;
        using reference = Value_&;

    private:
        SmartpointerType _wrapped;

    private:
        SharedReference(SmartpointerType&& wrapped)
            :   _wrapped(std::move(wrapped))
        { }

    public:
        SharedReference()
            :   _wrapped(std::make_shared<Value_>())
        { }

        template < typename Compatible_ >
        SharedReference(Compatible_* ptr)
            : _wrapped(check_ptr(ptr))
        { }

        template < typename Compatible_, typename Deleter_ >
        SharedReference(Compatible_* ptr, Deleter_ deleter)
            :   _wrapped(check_ptr(ptr), deleter)
        { }

        template < typename Compatible_, typename Deleter_, typename Allocator_ >
        SharedReference(Compatible_* ptr, Deleter_ deleter, Allocator_ allocator)
            :   _wrapped(check_ptr(ptr), deleter, allocator)
        { }

        template < typename Compatible_ >
        SharedReference(SharedReference<Compatible_> const& other, Value_* ptr)
            :   _wrapped(other._wrapped, check_ptr(ptr))
        { }

        template < typename Compatible_ >
        SharedReference(SharedPtr<Compatible_> const& other, Value_* ptr)
            :   _wrapped(check_ptr(other)._wrapped, check_ptr(ptr))
        { }

        SharedReference(SharedReference const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_ >
        SharedReference(SharedReference<Compatible_> const& other)
            :   _wrapped(other._wrapped)
        { }

        template < typename Compatible_ >
        SharedReference(SharedPtr<Compatible_> const& other)
            :   _wrapped(check_ptr(other)._wrapped)
        { }

        SharedReference(SharedReference&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        SharedReference(SharedReference<Compatible_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_ >
        SharedReference(SharedPtr<Compatible_>&& other)
            :   _wrapped(std::move(check_ptr(other)._wrapped))
        { }

        template < typename Compatible_, typename Deleter_ >
        SharedReference(UniqueReference<Compatible_, Deleter_>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_, typename Deleter_ >
        SharedReference(UniquePtr<Compatible_, Deleter_>&& other)
            :   _wrapped(std::move(check_ptr(other)._wrapped))
        { }

        SharedReference& operator=(const SharedReference& other) {
            _wrapped = other._wrapped;
            return self;
        }

        template < typename Compatible_ >
        SharedReference& operator=(SharedReference<Compatible_> const& other) {
            _wrapped = other._wrapped;
            return self;
        }

        template < typename Compatible_ >
        SharedReference& operator=(SharedPtr<Compatible_> const& other) {
            _wrapped = check_ptr(other)._wrapped;
            return self;
        }

        SharedReference& operator=(SharedReference&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_ >
        SharedReference& operator=(SharedReference<Compatible_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_ >
        SharedReference& operator=(SharedPtr<Compatible_>&& other) {
            _wrapped = std::move(check_ptr(other)._wrapped);
            return self;
        }

        template < typename Compatible_, typename Deleter_ >
        SharedReference& operator=(UniqueReference<Compatible_, Deleter_>&& other) {
            _wrapped = std::move(other._wrapped);
            return self;
        }

        template < typename Compatible_, typename Deleter_ >
        SharedReference& operator=(UniquePtr<Compatible_, Deleter_>&& other) {
            _wrapped = std::move(check_ptr(other)._wrapped);
            return self;
        }

        template < typename Compatible_ >
        void reset(Compatible_* ptr) {
            _wrapped.reset(check_ptr(ptr));
        }

        template < typename Compatible_, typename Deleter_ >
        void reset(Compatible_* ptr, Deleter_ deleter) {
            _wrapped.reset(check_ptr(ptr), deleter);
        }

        template< typename Compatible_, typename Deleter_, typename Allocator_ >
        void reset(Compatible_* ptr, Deleter_ deleter, Allocator_ allocator) {
            _wrapped.reset(check_ptr(ptr), deleter, allocator);
        }

        void swap(SharedReference& other) {
            _wrapped.swap(other._wrapped);
        }

        pointer get() const {
            return _wrapped.get();
        }

        reference operator*() const {
            return *get();
        }

        pointer operator->() const {
            return get();
        }

        long use_count() const {
            return _wrapped.use_count();
        }

        bool unique() const {
            return _wrapped.unique();
        }

        template < typename Other_ >
        bool owner_before(Other_ const& other) const {
            return _wrapped.owner_before(other._wrapped);
        }

        friend void swap(SharedReference& lhs, SharedReference& rhs) {
            lhs.swap(rhs);
        }

    private:
        template < typename Ptr_ >
        static Ptr_&& check_ptr(Ptr_&& ptr) {
            GUM_CHECK(ptr, NullPointerException());
            return std::forward<Ptr_>(ptr);
        }
    };


    template < typename Value_, typename ...Args_ >
    SharedReference<Value_> make_shared_ref(Args_&&... args) {
        return SharedReference<Value_>(std::make_shared<Value_>(std::forward<Args_>(args)...));
    }


#   define GUM_DECLARE_REF(Type_) \
        using Type_##Ref = gum::SharedReference<Type_>; \
        using Type_##ConstRef = gum::SharedReference<const Type_>

}

namespace std {

    template < typename Value_ >
    struct hash<gum::SharedReference<Value_>> {
        using SharedReferenceType = gum::SharedReference<Value_>;

        using argument_type = SharedReferenceType;
        using result_type = size_t;

        result_type operator()(argument_type const& ptr) const {
            return std::hash<typename SharedReferenceType::pointer>()(ptr.get());
        }
    };

}
