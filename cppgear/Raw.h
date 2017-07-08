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

#include <stdint.h>
#include <cassert>
#include <utility>
#include <type_traits>

#ifdef __GNUC__
    #include <endian.h>
#else
    #error "Please define your byte order macros"
#endif

namespace cppgear {

enum class ByteOrder {
    Little,
    Big,
    PDP,

};

constexpr ByteOrder byte_order() {
#if (BYTE_ORDER == LITTLE_ENDIAN)
    return ByteOrder::Little;
#elif (BYTE_ORDER == BIG_ENDIAN)
    return ByteOrder::Big;
#elif (BYTE_ORDER == PDP_ENDIAN)
    return ByteOrder::PDP;
#else
    #error "Byte order is unknown"
#endif
}

template <typename T>
union Split {
public:
    using ValueType = T;
    using ByteType  = uint8_t;

public:
    template <typename ...Args>
    Split(Args&&... args) : m_value(std::forward<Args>(args)...) { }

    operator ValueType&() { return m_value; }

    constexpr ByteType& operator[](uint8_t index) {
        constexpr uint8_t final = sizeof(ValueType) - 1;
        switch (byte_order()) {
            case ByteOrder::Little : return m_split[final - index];
            case ByteOrder::Big    : return m_split[index];
            case ByteOrder::PDP    : return m_split[final - ((index & 1) ? (index - 1) : (index + 1))];
            default                : assert(false);
        }
    }

private:
    ValueType m_value;
    ByteType  m_split[sizeof(ValueType)];

};

template <typename T>
using RawMemory = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

template <typename T>
class StorageFor
{
    /* Methods */
public:
    template <typename ...Args_>
    void ctor(Args_&&... args) {
        new(ptr()) T(std::forward<Args_>(args)...);
    }

    void dtor() {
        ref().~T();
    }

    T& ref() & {
        return *ptr();
    }

    T&& ref() && {
        return std::move(*ptr());
    }

    T const& ref() const& {
        return *ptr();
    }

    T* ptr() {
        return reinterpret_cast<T*>(&m_storage);
    }

    T const* ptr() const {
        return reinterpret_cast<T const*>(&m_storage);
    }

    /* Fields */
private:
    RawMemory<T> m_storage;

};

} // cppgear
