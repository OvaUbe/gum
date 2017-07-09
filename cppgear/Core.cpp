#include <cppgear/Core.h>

#include <cppgear/Defer.h>

#if CPPGEAR_USES_GCC_COMPILER || CPPGEAR_USES_CLANG_COMPILER
#   include <cxxabi.h>
#endif

namespace cppgear {

#if CPPGEAR_USES_GCC_COMPILER || CPPGEAR_USES_CLANG_COMPILER

    std::string demangle(const std::string& s) {
        int status = 0;
        char* result = abi::__cxa_demangle(s.c_str(), 0, 0, &status);
        defer { free(result); };

        return (status != 0) ? s : std::string(result);
    }

#else

    std::string demangle(const std::string& s) {
        return s;
    }

#endif

}
