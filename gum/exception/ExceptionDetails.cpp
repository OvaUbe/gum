#include <gum/exception/ExceptionDetails.h>

#include <gum/diagnostics/Demangle.h>
#include <gum/string/ToString.h>

namespace gum {

    namespace detail {

        std::string get_diagnostics_message(char const* message, std::type_info const& client_type_info, Where const& where, Backtrace const& backtrace) {
            return std::string(String() << demangle(client_type_info.name()) << ": " << message << "\nAt: " << where << "\nBacktrace: " << backtrace);
        }

    }

}
