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

#include <gum/exception/Exception.h>

#include <memory>

namespace gum {

    template < typename, typename >
    class UniqueReference;


    template < typename Value_, typename Deleter_ = std::default_delete<Value_> >
    class UniquePtr {
        using SmartpointerType = std::unique_ptr<Value_, Deleter_>;

        template < typename, typename >
        friend class UniqueReference;

        template < typename, typename >
        friend class UniquePtr;

        template < typename >
        friend class SharedReference;

        template < typename >
        friend class SharedPtr;

    public:
        using value_type = Value_;
        using pointer = Value_*;
        using reference = Value_&;

    private:
        SmartpointerType _wrapped;

    public:
        UniquePtr() { }

        UniquePtr(std::nullptr_t) { }

        UniquePtr(pointer ptr)
            :   _wrapped(ptr)
        { }

        UniquePtr(pointer ptr, Deleter_ const& deleter)
            :   _wrapped(ptr, deleter)
        { }

        UniquePtr(UniquePtr&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_, typename Deleter__ >
        UniquePtr(UniquePtr<Compatible_, Deleter__>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_, typename Deleter__ >
        UniquePtr(UniqueReference<Compatible_, Deleter__>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        UniquePtr& operator=(UniquePtr&& other) {
            _wrapped = std::move(other._wrapped);
            return *this;
        }

        template < typename Compatible_, class Deleter__ >
        UniquePtr& operator=(UniquePtr<Compatible_, Deleter__>&& other) {
            _wrapped = std::move(other._wrapped);
            return *this;
        }

        template < typename Compatible_, class Deleter__ >
        UniquePtr& operator=(UniqueReference<Compatible_, Deleter__>&& other) {
            _wrapped = std::move(other._wrapped);
            return *this;
        }

        UniquePtr& operator=(std::nullptr_t) {
            _wrapped.reset();
            return *this;
        }

        pointer release() {
            return _wrapped.release();
        }

        void reset(pointer ptr = pointer()) {
            _wrapped.reset(ptr);
        }

        void swap(UniquePtr& other) {
            _wrapped.swap(other._wrapped);
        }

        pointer get() const {
            return _wrapped.get();
        }

        Deleter_& get_deleter() {
            return _wrapped.get_deleter();
        }

        const Deleter_& get_deleter() const {
            return _wrapped.get_deleter();
        }

        explicit operator bool() const {
            return (bool)_wrapped;
        }

        reference operator*() const {
            check_ptr();
            return *get();
        }

        pointer operator->() const {
            check_ptr();
            return get();
        }

        friend void swap(UniquePtr& lhs, UniquePtr& rhs) {
            lhs.swap(rhs);
        }

    private:
        void check_ptr() const {
            GUM_CHECK(get(), NullPointerException());
        }
    };


    template < typename Value_, typename ...Args_ >
    UniquePtr<Value_> make_unique(Args_&&... args) {
        return UniquePtr<Value_>(new Value_(std::forward<Args_>(args)...));
    }


#   define GUM_DECLARE_UNIQUE_PTR(Type_) \
        using Type_##UniquePtr = gum::UniquePtr<Type_>; \
        using Type_##ConstUniquePtr = gum::UniquePtr<const Type_>

}

namespace std {

    template < typename Value_, typename Deleter_ >
    struct hash<gum::UniquePtr<Value_, Deleter_>> {
        using UniquePtrType = gum::UniquePtr<Value_, Deleter_>;

        using argument_type = UniquePtrType;
        using result_type = size_t;

        result_type operator()(argument_type const& ptr) const {
            return std::hash<typename UniquePtrType::pointer>()(ptr.get());
        }
    };

}
