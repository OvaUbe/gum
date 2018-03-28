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
#include <gum/maybe/MaybeTraits.h>

#include <memory>

namespace gum {

    template < typename, typename >
    class UniquePtr;


    template < typename Value_, typename Deleter_ = std::default_delete<Value_> >
    class UniqueReference {
        using SmartpointerType = std::unique_ptr<Value_, Deleter_>;

        template < typename, typename >
        friend class UniquePtr;

        template < typename, typename >
        friend class UniqueReference;

        template < typename >
        friend class SharedPtr;

        template < typename >
        friend class SharedReference;

    public:
        using value_type = Value_;
        using pointer = Value_*;
        using reference = Value_&;

    private:
        SmartpointerType _wrapped;

    public:
        UniqueReference(pointer ptr)
            :   _wrapped(check_ptr(ptr))
        { }

        UniqueReference(pointer ptr, Deleter_ const& deleter)
            :   _wrapped(check_ptr(ptr), deleter)
        { }

        UniqueReference(UniqueReference&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_, typename Deleter__ >
        UniqueReference(UniqueReference<Compatible_, Deleter__>&& other)
            :   _wrapped(std::move(other._wrapped))
        { }

        template < typename Compatible_, typename Deleter__ >
        UniqueReference(UniquePtr<Compatible_, Deleter__>&& other)
            :   _wrapped(std::move(check_ptr(other)._wrapped))
        { }

        UniqueReference& operator=(UniqueReference&& other) {
            _wrapped = std::move(other._wrapped);
            return *this;
        }

        template < typename Compatible_, class Deleter__ >
        UniqueReference& operator=(UniqueReference<Compatible_, Deleter__>&& other) {
            _wrapped = std::move(other._wrapped);
            return *this;
        }

        template < typename Compatible_, class Deleter__ >
        UniqueReference& operator=(UniquePtr<Compatible_, Deleter__>&& other) {
            _wrapped = std::move(check_ptr(other)._wrapped);
            return *this;
        }

        pointer release() {
            return _wrapped.release();
        }

        void reset(pointer ptr) {
            _wrapped.reset(check_ptr(ptr));
        }

        void swap(UniqueReference& other) {
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

        reference operator*() const {
            return *get();
        }

        pointer operator->() const {
            return get();
        }

        friend void swap(UniqueReference& lhs, UniqueReference& rhs) {
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
    UniqueReference<Value_> make_unique_ref(Args_&&... args) {
        return UniqueReference<Value_>(new Value_(std::forward<Args_>(args)...));
    }


#   define GUM_DECLARE_UNIQUE_REF(Type_) \
        using Type_##UniqueRef = gum::UniqueReference<Type_>; \
        using Type_##ConstUniqueRef = gum::UniqueReference<const Type_>

}

namespace std {

template < typename Value_, typename Deleter_ >
struct hash<gum::UniqueReference<Value_, Deleter_>> {
    using UniqueReferenceType = gum::UniqueReference<Value_, Deleter_>;

    using argument_type = UniqueReferenceType;
    using result_type = size_t;

    result_type operator()(argument_type const& ptr) const {
        return std::hash<typename UniqueReferenceType::pointer>()(ptr.get());
    }
};

}
