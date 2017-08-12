#include <gum/exception/Exception.h>

#include <gum/string/ToString.h>

namespace gum {

    IndexOutOfRangeException::IndexOutOfRangeException(u64 index, u64 begin, u64 end)
        :   Exception(String() << "Index " << index << " out of range [ " << begin << ", " << end << " )")
    { }

    IndexOutOfRangeException::IndexOutOfRangeException(s64 index, s64 begin, s64 end)
        :   Exception(String() << "Index " << index << " out of range [ " << begin << ", " << end << " )")
    { }


    IndexOutOfRangeException::IndexOutOfRangeException(u64 index, u64 size)
        :   Exception(String() << "Index: " << index << ". Size: " << size << ".")
    { }

    IndexOutOfRangeException::IndexOutOfRangeException(s64 index, s64 size)
        :   Exception(String() << "Index:" << index << ". Size: " << size << ".")
    { }

}
