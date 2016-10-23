#pragma once

#include <stdint.h>
#include <cassert>

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
    Split() : m_value() { }
    Split(ValueType value) : m_value(value) { }

    operator ValueType&() { return m_value; }

    constexpr ByteType& operator[](uint8_t index) {
        constexpr uint8_t final = sizeof(ValueType) - 1;
        switch (byte_order()) {
            case ByteOrder::Little : return m_split[final - index];
            case ByteOrder::Big    : return m_split[index];
            /* If index is odd, final - (index - 1), else final - (index + 1) */
            case ByteOrder::PDP    : return m_split[final - ((index & 1) ? (index - 1) : (index + 1))];
            default                : assert(false);
        }
    }

private:
    ValueType m_value;
    ByteType  m_split[sizeof(ValueType)];

};

} // cppgear
