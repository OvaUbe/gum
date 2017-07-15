#include <cppgear/Exception.h>

#include <cppgear/diagnostics/Demangle.h>
#include <cppgear/string/ToString.h>

namespace cppgear {

    namespace detail {

        std::string get_diagnostics_message(char const* message, std::type_info const& client_type_info, Where const& where, Backtrace const& backtrace) {
            return std::string(String() << demangle(client_type_info.name()) << ": " << message << "\nAt: " << where << "\nBacktrace: " << backtrace);
        }

    }


    IndexOutOfRangeException::IndexOutOfRangeException(u64 index, u64 begin, u64 end)
        :   Exception(std::string(String() << "Index " << index << " out of range [ " << begin << ", " << end << " )"))
    { }


    IndexOutOfRangeException::IndexOutOfRangeException(s64 index, s64 begin, s64 end)
        :   Exception(std::string(String() << "Index " << index << " out of range [ " << begin << ", " << end << " )"))
    { }


    IndexOutOfRangeException::IndexOutOfRangeException(u64 index, u64 size)
        :   Exception(std::string(String() << "Index: " << index << ". Size: " << size << "."))
    { }


    IndexOutOfRangeException::IndexOutOfRangeException(s64 index, s64 size)
        :   Exception(std::string(String() << "Index:" << index << ". Size: " << size << "."))
    { }

}
