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
#include <memory>
#include <algorithm>

namespace cppgear {

template <
    typename T,
    typename Allocator = std::allocator<T>
    >
class dynarray {
    /* Inner types */
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference  = value_type const&;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    struct _deallocator {
        size_type size;
        Allocator mutable allocator;

        _deallocator(Allocator const& allocator, size_type size) noexcept
            : allocator(allocator), size(size) { }

        void operator()(T* p) const noexcept { allocator.deallocate(p, size); }

    };

    using base_holder = std::unique_ptr<value_type, _deallocator>;

    /* Methods */
public:
    explicit dynarray(size_type count, Allocator alloc = Allocator())
        : m_base(alloc.allocate(count), _deallocator(alloc, count)) { }

    explicit dynarray(size_type count, T const& value, Allocator alloc = Allocator())
        : m_base(alloc.allocate(count), _deallocator(alloc, count)) {
        fill(value);
    }

    explicit dynarray(std::initializer_list<T> init, Allocator alloc = Allocator())
        : m_base(alloc.allocate(init.size()), _deallocator(alloc, init.size())) {
        std::copy(init.begin(), init.end(), m_base.get());
    }

    explicit dynarray(dynarray const& other)
        : m_base(_copy_base(other.m_base)) { }

    explicit dynarray(dynarray&& other) = default;

    ~dynarray() = default;

    dynarray& operator=(dynarray const& other) {
        m_base = _copy_base(other);
        return *this;
    }

    dynarray& operator=(dynarray&& other) = default;

    reference at(size_type pos){
        if (pos >= size()) {
            throw std::out_of_range();
        }
        return *(data() + pos);
    }

    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range();
        }
        return *(data() + pos);
    }

    reference operator[](size_type pos) noexcept {
        return *(data() + pos);
    }

    const_reference operator[](size_type pos) const noexcept {
        return *(data() + pos);
    }

    reference back() noexcept {
        return *(data() + size() - 1);
    }

    const_reference back() const noexcept {
        return *(data() + size() - 1);
    }

    pointer data() noexcept {
        return m_base.get();
    }

    const_pointer data() const noexcept {
        return m_base.get();
    }

    iterator begin() noexcept {
        return data();
    }

    const_iterator begin() const noexcept {
        return data();
    }

    const_iterator cbegin() const noexcept {
        return data();
    }

    iterator end() noexcept {
        return begin() + size();
    }

    const_iterator end() const noexcept {
        return begin() + size();
    }

    const_iterator cend() const noexcept {
        return begin() + size();
    }

    reverse_iterator rbegin() noexcept {
        return end() - 1;
    }

    const_reverse_iterator rbegin() const noexcept {
        return end() - 1;
    }

    const_reverse_iterator crbegin() const noexcept {
        return end() - 1;
    }

    reverse_iterator rend() noexcept {
        return begin() - 1;
    }

    const_reverse_iterator rend() const noexcept {
        return begin() - 1;
    }

    const_reverse_iterator crend() const noexcept {
        return begin() - 1;
    }

    bool empty() const noexcept {
        return 0 == size();
    }

    size_type size() const noexcept {
        return m_base.get_deleter().size;
    }

    void fill(const_reference value) {
        std::fill(begin(), end(), value);
    }

private:
    static base_holder _copy_base(base_holder const& other) {
        _deallocator const& deleter = other.get_deleter();
        pointer p = deleter.allocator.allocate(deleter.size);
        base_holder result(p, deleter);

        pointer begin = other.get();
        std::copy(begin, begin + deleter.size, result.get());

        return result;
    }

    /* Fields */
private:
    base_holder m_base;

};

template <typename T>
class dynarray<T, std::allocator<T>> {
    /* Inner types */
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference  = value_type const&;
    using pointer = value_type*;
    using const_pointer = value_type const*;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    using base_holder = std::unique_ptr<value_type[]>;

    /* Methods */
public:
    explicit dynarray(size_type count)
        : m_base(new value_type[count])
        , m_size(count) { }

    explicit dynarray(size_type count, T const& value)
        : m_base(new value_type[count])
        , m_size(count) {
        fill(value);
    }

    explicit dynarray(std::initializer_list<T> init)
        : m_base(new value_type[init.size()])
        , m_size(init.size()) {
        std::copy(init.begin(), init.end(), m_base.get());
    }

    explicit dynarray(dynarray const& other)
        : m_base(new value_type[other.m_size])
        , m_size(other.m_size) {
        std::copy(other.begin(), other.end(), begin());
    }

    explicit dynarray(dynarray&& other) = default;

    ~dynarray() = default;

    dynarray& operator=(dynarray const& other) {
        m_base.reset(new value_type[other.m_size]);
        m_size = other.m_size;

        std::copy(other.begin(), other.end(), begin());

        return *this;
    }

    dynarray& operator=(dynarray&& other) = default;

    reference at(size_type pos) {
        if (pos >= size()) {
            throw std::out_of_range();
        }
        return m_base[pos];
    }

    const_reference at(size_type pos) const {
        if (pos >= size()) {
            throw std::out_of_range();
        }
        return m_base[pos];
    }

    reference operator[](size_type pos) noexcept {
        return m_base[pos];
    }

    const_reference operator[](size_type pos) const noexcept {
        return m_base[pos];
    }

    reference back() noexcept {
        return m_base[size() - 1];
    }

    const_reference back() const noexcept {
        return m_base[size() - 1];
    }

    pointer data() noexcept {
        return m_base.get();
    }

    const_pointer data() const noexcept {
        return m_base.get();
    }

    iterator begin() noexcept {
        return data();
    }

    const_iterator begin() const noexcept {
        return data();
    }

    const_iterator cbegin() const noexcept {
        return data();
    }

    iterator end() noexcept {
        return begin() + size();
    }

    const_iterator end() const noexcept {
        return begin() + size();
    }

    const_iterator cend() const noexcept {
        return begin() + size();
    }

    reverse_iterator rbegin() noexcept {
        return end() - 1;
    }

    const_reverse_iterator rbegin() const noexcept {
        return end() - 1;
    }

    const_reverse_iterator crbegin() const noexcept {
        return end() - 1;
    }

    reverse_iterator rend() noexcept {
        return begin() - 1;
    }

    const_reverse_iterator rend() const noexcept {
        return begin() - 1;
    }

    const_reverse_iterator crend() const noexcept {
        return begin() - 1;
    }

    bool empty() const noexcept {
        return 0 == size();
    }

    size_type size() const noexcept {
        return m_size;
    }

    void fill(const_reference value) {
        std::fill(begin(), end(), value);
    }

    /* Fields */
private:
    base_holder m_base;
    size_type m_size;

};

} // cppgear
