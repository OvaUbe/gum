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

#include <gum/Core.h>

#include <iterator>

namespace gum {

template <typename Derived_,
          typename Category_,
          typename Value_,
          typename Difference_ = std::ptrdiff_t,
          typename Pointer_ = Value_*,
          typename Reference_ = Value_&,
          typename Base_ = std::iterator<Category_, Value_, Difference_, Pointer_, Reference_>>
class IteratorBase : public Base_ {
  public:
    using value_type = typename Base_::value_type;
    using difference_type = typename Base_::difference_type;
    using pointer = typename Base_::value_type*;
    using reference = typename Base_::value_type&;
    using iterator_category = typename Base_::iterator_category;

  public:
    bool operator==(Derived_ const& other) const {
        return static_cast<Derived_ const&>(*this).equals(other);
    }

    bool operator!=(Derived_ const& other) const {
        return !operator==(other);
    }

    reference operator*() {
        return static_cast<Derived_&>(*this).dereference();
    }

    pointer operator->() {
        return &operator*();
    }

    Derived_& operator++() {
        static_cast<Derived_&>(*this).increment();
        return static_cast<Derived_&>(*this);
    }

    Derived_ operator++(int) {
        Derived_ tmp(static_cast<Derived_&>(*this));
        static_cast<Derived_&>(*this).increment();
        return tmp;
    }

    Derived_& operator--() {
        static_cast<Derived_&>(*this).decrement();
        return static_cast<Derived_&>(*this);
    }

    Derived_ operator--(int) {
        Derived_ tmp(static_cast<Derived_&>(*this));
        static_cast<Derived_&>(*this).decrement();
        return tmp;
    }
};

namespace detail {

template <typename Iterator_>
class IteratorToValue {
    using Pointer = typename Iterator_::pointer;
    using Pair = typename std::remove_pointer<Pointer>::type;
    using PairFirst = typename Pair::first_type;

  public:
    using Type = typename std::conditional<std::is_const<Pair>::value, PairFirst const, PairFirst>::type;
};
}

template <typename Wrapped_>
class KeyIterator : public IteratorBase<KeyIterator<Wrapped_>,
                                        std::bidirectional_iterator_tag,
                                        typename detail::IteratorToValue<Wrapped_>::Type,
                                        typename Wrapped_::difference_type> {
    using Base = IteratorBase<KeyIterator<Wrapped_>,
                              std::bidirectional_iterator_tag,
                              typename detail::IteratorToValue<Wrapped_>::Type,
                              typename Wrapped_::difference_type>;

  public:
    using value_type = typename Base::value_type;
    using difference_type = typename Base::difference_type;
    using pointer = typename Base::value_type*;
    using reference = typename Base::value_type&;
    using iterator_category = typename Base::iterator_category;

  public:
    KeyIterator(Wrapped_ const& wrapped)
        : m_wrapped(wrapped) {}

    bool equals(KeyIterator const& other) const {
        return m_wrapped == other.m_wrapped;
    }

    reference dereference() {
        return m_wrapped->first;
    }

    void increment() {
        ++m_wrapped;
    }

    void decrement() {
        --m_wrapped;
    }

    Wrapped_ get_wrapped() const {
        return m_wrapped;
    }

  private:
    Wrapped_ m_wrapped;
};

template <typename Wrapped_>
KeyIterator<Wrapped_> make_key_iterator(Wrapped_ const& wrapped) {
    return KeyIterator<Wrapped_>(wrapped);
}
}
