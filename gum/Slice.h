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

#include <iterator>

namespace gum {

template <typename Value_>
class Slice {
  public:
    using value_type = Value_;

    using const_pointer = const value_type*;
    using pointer = const_pointer;
    using const_reference = const value_type&;
    using reference = const_reference;

    using const_iterator = const_pointer;
    using iterator = pointer;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;
    using reverse_iterator = std::reverse_iterator<iterator>;

  private:
    pointer _data;
    size_t _size;

  public:
    template <typename Container_>
    Slice(const Container_& container)
        : Slice(container.begin(), container.end()) {}

    template <typename Iter_>
    Slice(Iter_ begin, Iter_ end)
        : _data(&*begin)
        , _size(std::distance(begin, end)) {}

    Slice(pointer data, size_t size)
        : _data(data)
        , _size(size) {}

    reference at(size_t index) const {
        GUM_CHECK_INDEX(index, size());
        return data()[index];
    }

    reference operator[](size_t index) const {
        return at(index);
    }

    reference front() const {
        return at(0);
    }

    reference back() const {
        GUM_CHECK_INDEX(0, size());
        return data()[size() - 1];
    }

    pointer data() const {
        return _data;
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
};
}
