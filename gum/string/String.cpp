#include <gum/string/String.h>

#include <gum/exception/Exception.h>

namespace gum {
namespace detail {

void check_string_index(size_t index, size_t size) {
    GUM_CHECK_INDEX(index, size);
}
}
}
