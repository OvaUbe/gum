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

#include <cppgear/string/String.h>

#include <cstring>
#include <iterator>

namespace cppgear {

    template < typename Char_ >
    class BasicStringLiteral {
    public:
        using value_type = Char_ const;

        using const_pointer = value_type*;
        using pointer = const_pointer;
        using const_reference = value_type&;
        using reference = const_reference;

        using const_iterator = pointer;
        using iterator = const_iterator;
        using const_reverse_iterator = std::reverse_iterator<const_iterator>;
        using reverse_iterator = std::reverse_iterator<iterator>;

    public:
        value_type*     _str;
        size_t          _size;

    public:
        BasicStringLiteral(pointer str)
            :   _str(str),
                _size(strlen(_str))
        { }

        reference at(size_t index) const {
            CPPGEAR_CHECK_INDEX(index, size());
            return data()[index];
        }

        reference operator[](size_t index) const {
            return at(index);
        }

        reference front() const {
            return at(0);
        }

        reference back() const {
            CPPGEAR_CHECK_INDEX(0, size());
            return data()[size() - 1];
        }

        pointer data() const {
            return _str;
        }

        pointer c_str() const {
            return data();
        }

        size_t size() const {
            return _size;
        }

        size_t length() const {
            return size();
        }

        bool empty() const {
            return !size();
        }

        iterator begin() const {
            return data();
        }

        const_iterator cbegin() const {
            return data();
        }

        iterator end() const {
            return data() + size();
        }

        const_iterator cend() const {
            return data() + size();
        }

        reverse_iterator rbegin() const {
            return reverse_iterator(begin());
        }

        const_reverse_iterator crbegin() const {
            return const_reverse_iterator(cbegin());
        }

        reverse_iterator rend() const {
            return reverse_iterator(end());
        }

        const_reverse_iterator crend() const {
            return const_reverse_iterator(cend());
        }

        String to_string() const {
            return String(begin(), end());
        }
    };


    using StringLiteral = BasicStringLiteral<char>;

}
