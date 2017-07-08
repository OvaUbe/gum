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

#include <cppgear/Exception.h>
#include <cppgear/Raw.h>

namespace cppgear {

    CPPGEAR_DECLARE_EXCEPTION(EmptyOptionalException, "Empty optional");


    template < typename >
    struct OptionalBuilder;

    template < typename Value_ >
    class Optional {
    public:
        using value_type = Value_;
        using const_value_type = Value_ const;

        template < typename >
        friend struct OptionalBuilder;

    public:
        Optional(std::nullptr_t = nullptr) :
            m_valid(false) { }

        Optional(Optional const& copy) {
            _initialize(copy);
        }

        Optional(Optional&& move) {
            _initialize(std::move(move));
        }

        ~Optional() {
            _destroy();
        }

        Optional& operator=(Optional const& copy) {
            return _assign(copy);
        }

        Optional& operator=(Optional&& move) {
            return _assign(std::move(move));
        }

        void destroy() {
            _destroy();
            m_valid = false;
        }

        value_type* operator->() {
            check();
            return m_storage.ptr();
        }

        const_value_type* operator->() const {
            check();
            return m_storage.ptr();
        }

        value_type& operator*() & {
            check();
            return m_storage.ref();
        }

        value_type&& operator*() && {
            check();
            return m_storage.ref();
        }

        const_value_type& operator*() const& {
            check();
            return m_storage.ref();
        }

        explicit operator bool() const {
            return m_valid;
        }

        void swap(Optional& other) {
            if (self) {
                if (other) {
                    return std::swap(*self, *other);
                }
                other._consume(self);
            }
            if (other) {
                self._consume(other);
            }
        }

    private:
        void _destroy() {
            if (self) {
                m_storage.dtor();
            }
        }

        template < typename Optional_ >
        void _initialize(Optional_&& other) {
            m_valid = (bool)other;
            if (self) {
                m_storage.ctor(std::forward<Optional_>(other).m_storage.ref());
            }
        }

        template < typename Optional_ >
        Optional_& _assign(Optional_&& other) {
            _destroy();
            _initialize(std::forward<Optional_>(other));
            return self;
        }

        void _consume(Optional& other) {
            m_storage.ctor(std::move(other.m_storage.ref()));
            other.destroy();
        }

        void check() const {
            CPPGEAR_CHECK(m_valid, EmptyOptionalException());
        }

     private:
        StorageFor<value_type> m_storage;
        bool m_valid;
    };

    template < typename Value_ >
    class Optional<Value_&> {
    public:
        using value_type = Value_&;
        using const_value_type = Value_ const&;

        template < typename >
        friend struct OptionalBuilder;

    public:
        Optional(std::nullptr_t ptr = nullptr) :
            m_ptr(ptr) { }

        Value_* operator->() {
            check();
            return m_ptr;
        }

        Value_ const* operator->() const {
            check();
            return m_ptr;
        }

        value_type operator*() & {
            check();
            return *m_ptr;
        }

        const_value_type operator*() const& {
            check();
            return *m_ptr;
        }

        explicit operator bool() const {
            return m_ptr;
        }

        void swap(Optional& other) {
            std::swap(m_ptr, other.m_ptr);
        }

    private:
        void check() const {
            CPPGEAR_CHECK(m_ptr, EmptyOptionalException());
        }

    private:
        Value_* m_ptr;
    };

    template < typename Value_ >
    struct OptionalBuilder {
        using Result = Optional<Value_>;

        template < typename ...Args_ >
        Result operator()(Args_&&... args) const  {
            Result result;
            result.m_valid = true;
            result.m_storage.ctor(std::forward<Args_>(args)...);
            return result;
        }
    };

    template < typename Value_ >
    struct OptionalBuilder<Value_&> {
        using Result = Optional<Value_&>;

        Result operator()(Value_& value) const  {
            Result result;
            result.m_ptr = &value;
            return result;
        }
    };

    template < typename Value_, typename ...Args_ >
    Optional<Value_> make_optional(Args_&&... args) {
        return OptionalBuilder<Value_>()(std::forward<Args_>(args)...);
    }

}
