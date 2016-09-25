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
    struct Deallocator {
        size_type const size;
        Allocator const allocator;

        Deallocator(Allocator const& allocator, size_type size) noexcept
            : allocator(allocator), size(size) { }

        void operator()(T* p) const noexcept { allocator.deallocate(p, size); }

    };

    using base_holder = std::unique_ptr<value_type[], Deallocator>;

    /* Methods */
public:
    explicit dynarray(size_type count, Allocator const& alloc = Allocator());

    explicit dynarray(size_type count, T const& value, Allocator const& alloc = Allocator());

    explicit dynarray(std::initializer_list<T> init, Allocator const& alloc = Allocator());

    explicit dynarray(dynarray const& other);

    explicit dynarray(dynarray&& other) = default;

    ~dynarray() = default;

    dynarray& operator=(dynarray const& other);

    dynarray& operator=(dynarray&& other) = default;

    reference at(size_type pos);
    const_reference at(size_type pos) const;

    reference operator[](size_type pos) noexcept;
    const_reference operator[](size_type pos) const noexcept;

    reference back() noexcept;
    const_reference back() const noexcept;

    pointer data() noexcept;
    const_pointer data() const noexcept;

    iterator begin() noexcept;
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;

    iterator end() noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;

    reverse_iterator rbegin() noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;

    reverse_iterator rend() noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    bool empty() const noexcept;
    size_type size() const noexcept;

    void fill(const_reference value);

private:
    static base_holder _copy_base(base_holder const& other);

    /* Fields */
private:
    base_holder m_base;

};

template <typename T, typename Allocator>
dynarray<T, Allocator>::dynarray(size_type count, Allocator const& alloc)
    : m_base(alloc.allocate(count), Deallocator(alloc, count)) {
}

template <typename T, typename Allocator>
dynarray<T, Allocator>::dynarray(size_type count, T const& value, Allocator const& alloc)
    : m_base(alloc.allocate(count), Deallocator(alloc, count)) {

    pointer begin = m_base.get();
    std::fill(begin, begin + count, value);
}

template <typename T, typename Allocator>
dynarray<T, Allocator>::dynarray(dynarray const& other)
    : m_base(_copy_base(other.m_base)) { }

template <typename T, typename Allocator>
dynarray<T, Allocator>::dynarray(std::initializer_list<T> init, Allocator const& alloc)
    : m_base(alloc.allocate(init.size()), Deallocator(alloc, init.size())) {

    std::copy(init.begin(), init.end(), m_base.get());
}

template <typename T, typename Allocator>
dynarray<T, Allocator>& dynarray<T, Allocator>::operator=(dynarray const& other) {
    m_base.reset(_copy_base(other));
    return *this;
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::reference dynarray<T, Allocator>::at(size_type pos) {
    if (pos >= size()) {
        throw std::out_of_range();
    }
    return m_base[pos];
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reference dynarray<T, Allocator>::at(size_type pos) const {
    if (pos >= size()) {
        throw std::out_of_range();
    }
    return m_base[pos];
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::reference dynarray<T, Allocator>::operator[](size_type pos) noexcept {
    return m_base[pos];
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reference dynarray<T, Allocator>::operator[](size_type pos) const noexcept {
    return m_base[pos];
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::reference dynarray<T, Allocator>::back() noexcept {
    return m_base[size() - 1];
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reference dynarray<T, Allocator>::back() const noexcept {
    return m_base[size() - 1];
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::pointer dynarray<T, Allocator>::data() noexcept {
    return m_base.get();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_pointer dynarray<T, Allocator>::data() const noexcept {
    return m_base.get();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::iterator dynarray<T, Allocator>::begin() noexcept {
    return data();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_iterator dynarray<T, Allocator>::begin() const noexcept {
    return data();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_iterator dynarray<T, Allocator>::cbegin() const noexcept {
    return data();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::iterator dynarray<T, Allocator>::end() noexcept {
    return data() + size();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_iterator dynarray<T, Allocator>::end() const noexcept {
    return data() + size();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_iterator dynarray<T, Allocator>::cend() const noexcept {
    return data() + size();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::reverse_iterator dynarray<T, Allocator>::rbegin() noexcept {
    return &back();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reverse_iterator dynarray<T, Allocator>::rbegin() const noexcept {
    return &back();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reverse_iterator dynarray<T, Allocator>::crbegin() const noexcept {
    return &back();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::reverse_iterator dynarray<T, Allocator>::rend() noexcept {
    return data() - 1;
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reverse_iterator dynarray<T, Allocator>::rend() const noexcept {
    return data() - 1;
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::const_reverse_iterator dynarray<T, Allocator>::crend() const noexcept {
    return data() - 1;
}

template <typename T, typename Allocator>
bool dynarray<T, Allocator>::empty() const noexcept {
    return 0 == size();
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::size_type dynarray<T, Allocator>::size() const noexcept {
    return m_base.get_deleter().size;
}

template <typename T, typename Allocator>
void dynarray<T, Allocator>::fill(const_reference value) {
    std::fill(begin(), end(), value);
}

template <typename T, typename Allocator>
typename dynarray<T, Allocator>::base_holder dynarray<T, Allocator>::_copy_base(base_holder const& other) {
    Deallocator const& d = other.get_deleter();
    base_holder result(d.allocator.allocate(d.size), d);

    pointer begin = other.get();
    std::copy(begin, begin + d.size, result.get());

    return result;
}

} // cppgear
