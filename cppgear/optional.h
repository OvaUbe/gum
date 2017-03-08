/*
 * Copyright (c) 2016 Vladimir Golubev
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

#include <cppgear/misc.h>
#include <cppgear/raw.h>

namespace cppgear {

    namespace detail {

        template < typename >
        struct OptionalBuilder;

        struct ChainingHelper {
            template < typename Callable_, typename ...Args_, typename Result_ = typename std::result_of<Callable_(Args_&&...)>::type >
            typename std::enable_if<std::is_void<Result_>::value, std::nullptr_t>::type
            operator()(Callable_&& callable, Args_&& ...args) {
                callable(std::forward<Args_>(args)...);
                return nullptr;
            }

            template < typename Callable_, typename ...Args_, typename Result_ = typename std::result_of<Callable_(Args_&&...)>::type >
            typename std::enable_if<!std::is_void<Result_>::value, Result_>::type
            operator()(Callable_&& callable, Args_&& ...args) {
                return callable(std::forward<Args_>(args)...);
            }
        };

    }

    template < typename Value_ >
    class Optional {
    public:
        using value_type = Value_;
        using const_value_type = Value_ const;

        template < typename >
        friend struct detail::OptionalBuilder;

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
            return m_storage.ptr();
        }

        const_value_type* operator->() const {
            return m_storage.ptr();
        }

        value_type& operator*() & {
            return m_storage.ref();
        }

        value_type&& operator*() && {
            return m_storage.ref();
        }

        const_value_type& operator*() const& {
            return m_storage.ref();
        }

        explicit operator bool() const {
            return m_valid;
        }

        template < typename Default_ >
        value_type value_or(Default_&& def) {
            if (self) {
                return *self;
            }
            return std::forward<Default_>(def);
        }

        template < typename Default_ >
        value_type value_or(Default_&& def) const {
            if (self) {
                return *self;
            }
            return std::forward<Default_>(def);
        }

        template < typename Callable_ >
        Optional map(Callable_&& callable) {
            if (self) {
                return detail::ChainingHelper()(callable, *self);
            }
            return self;
        }

        template < typename Callable_ >
        Optional map(Callable_&& callable) const {
            if (self) {
                return detail::ChainingHelper()(callable, *self);
            }
            return self;
        }

        template < typename Callable_ >
        Optional or_else(Callable_&& callable) {
            if (!self) {
                return detail::ChainingHelper()(callable);
            }
            return self;
        }

        template < typename Callable_ >
        Optional or_else(Callable_&& callable) const {
            if (!self) {
                return detail::ChainingHelper()(callable);
            }
            return self;
        }

        template < typename Callable_, typename Default_ >
        value_type map_or(Callable_&& callable, Default_&& def) {
            if (self) {
                return callable(*self);
            }
            return std::forward<Default_>(def);
        }

        template < typename Callable_, typename Default_ >
        value_type map_or(Callable_&& callable, Default_&& def) const {
            if (self) {
                return callable(*self);
            }
            return std::forward<Default_>(def);
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
        friend struct detail::OptionalBuilder;

    public:
        Optional(std::nullptr_t ptr = nullptr) :
            m_ptr(ptr) { }

        Value_* operator->() {
            return m_ptr;
        }

        Value_ const* operator->() const {
            return m_ptr;
        }

        value_type operator*() & {
            return *m_ptr;
        }

        const_value_type operator*() const& {
            return *m_ptr;
        }

        explicit operator bool() const {
            return m_ptr;
        }

        value_type value_or(value_type def) {
            if (self) {
                return *self;
            }
            return def;
        }

        const_value_type value_or(const_value_type def) const {
            if (self) {
                return *self;
            }
            return def;
        }

        template < typename Callable_ >
        Optional map(Callable_&& callable) {
            if (self) {
                return detail::ChainingHelper()(callable, *self);
            }
            return self;
        }

        template < typename Callable_ >
        Optional map(Callable_&& callable) const {
            if (self) {
                return detail::ChainingHelper()(callable, *self);
            }
            return self;
        }

        template < typename Callable_ >
        Optional or_else(Callable_&& callable) {
            if (!self) {
                return detail::ChainingHelper()(callable);
            }
            return self;
        }

        template < typename Callable_ >
        Optional or_else(Callable_&& callable) const {
            if (!self) {
                return detail::ChainingHelper()(callable);
            }
            return self;
        }

        template < typename Callable_ >
        value_type map_or(Callable_&& callable, value_type def) {
            if (self) {
                return callable(*self);
            }
            return def;
        }

        template < typename Callable_ >
        const_value_type map_or(Callable_&& callable, const_value_type def) const {
            if (self) {
                return callable(*self);
            }
            return def;
        }

        void swap(Optional& other) {
            std::swap(m_ptr, other.m_ptr);
        }

    private:
        Value_* m_ptr;
    };

    namespace detail {

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

    }

    template < typename Value_, typename ...Args_ >
    Optional<Value_> make_optional(Args_&&... args) {
        return detail::OptionalBuilder<Value_>()(std::forward<Args_>(args)...);
    }

}
