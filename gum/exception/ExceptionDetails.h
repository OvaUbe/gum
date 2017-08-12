#pragma once

#include <gum/diagnostics/Backtrace.h>
#include <gum/Core.h>

#include <stdexcept>
#include <typeinfo>

namespace gum {

    namespace detail {

        struct Exception : public std::runtime_error {
            Exception(std::string const& message)
                : std::runtime_error(message)
            { }

            Exception(char const* message)
                : std::runtime_error(message)
            { }
        };


        std::string get_diagnostics_message(char const* message, std::type_info const& client_type_info, Where const& where, Backtrace const& backtrace);


        template < typename ClientException_ >
        class ExceptionTemplate : public ClientException_ {
            std::string _what;

        public:
            template < typename ClientException__ >
            ExceptionTemplate(ClientException__&& ex, Where const& where, Backtrace const& backtrace)
                :   ClientException_(std::forward<ClientException__>(ex)),
                    _what(get_diagnostics_message(ex.what(), typeid(ClientException_), where, backtrace))
            { }

            char const* what() const noexcept override {
                return _what.c_str();
            }
        };


        template < typename Exception_ >
        inline auto make_exception(Exception_&& ex, Where const& where, Backtrace const& backtrace) {
            return ExceptionTemplate<Exception_>(std::forward<Exception_>(ex), where, backtrace);
        }

    }

}
