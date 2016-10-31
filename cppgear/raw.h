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
    template <typename ...Args>
    void ctor(Args&&... args) {
        new(ptr()) T(std::forward<T>(args)...);
    }

    void dtor() {
        ref().~T();
    }

    T& ref() {
        return *ptr();
    }

    T* ptr() {
        return reinterpret_cast<T*>(&m_storage);
    }

    /* Fields */
private:
    RawMemory<T> m_storage;

};

} // cppgear
