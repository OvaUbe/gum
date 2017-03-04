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

#include <iterator>

namespace cppgear {

    template < typename Wrapped_ >
    class KeyIterator {
    public:
        using value_type = typename std::iterator_traits<Wrapped_>::value_type::first_type;
        using difference_type = typename std::iterator_traits<Wrapped_>::difference_type;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = typename std::iterator_traits<Wrapped_>::iterator_category;

    public:
        KeyIterator(Wrapped_ const& wrapped) :
            m_wrapped(wrapped) { }

        KeyIterator(KeyIterator const& copy) = default;
        KeyIterator(KeyIterator&& move) = default;

        ~KeyIterator() = default;

        KeyIterator& operator=(KeyIterator const& copy) = default;
        KeyIterator& operator=(KeyIterator&& move) = default;

        bool operator==(KeyIterator const& other) {
            return m_wrapped->first == other.m_wrapped->first;
        }

        bool operator!=(KeyIterator const& other) {
            return !operator==(other);
        }

        value_type& operator*() {
            return m_wrapped->first;
        }

        value_type& operator++() {
            return (++m_wrapped)->first;
        }

        value_type operator++(int) {
            return (m_wrapped++)->first;
        }

        value_type& operator--() {
            return (--m_wrapped)->first;
        }

        value_type operator--(int) {
            return (m_wrapped--)->first;
        }

        value_type* operator->() {
            return &operator*();
        }

        Wrapped_ get_wrapped() const {
            return m_wrapped;
        }

    private:
        Wrapped_ m_wrapped;

    };

    template < typename Wrapped_ >
    KeyIterator<Wrapped_> make_key_iterator(Wrapped_ const& wrapped) {
        return KeyIterator<Wrapped_>(wrapped);
    }

}

namespace std {

    template < typename Wrapped_ >
    struct iterator_traits<cppgear::KeyIterator<Wrapped_>> {
        using Iterator = cppgear::KeyIterator<Wrapped_>;

        using value_type = typename Iterator::value_type;
        using difference_type = typename Iterator::difference_type;
        using pointer = typename Iterator::pointer;
        using reference = typename Iterator::reference;
        using iterator_category = typename Iterator::iterator_category;
    };

}
