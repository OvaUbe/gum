#include <cppgear/Exception.h>

#include <cppgear/diagnostics/Demangle.h>

#include <sstream>

namespace cppgear {

    namespace detail {

        std::string get_diagnostics_message(char const* message, std::type_info const& client_type_info, Where const& where, Backtrace const& backtrace) {
            std::stringstream ss;
            ss << demangle(client_type_info.name()) << ": " << message << "\nAt: " << where.to_string() << "\nBacktrace: " << backtrace.to_string();
            return ss.str();
        }

    }

}
