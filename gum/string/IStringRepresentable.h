#pragma once

#include <gum/string/String.h>

namespace gum {

    struct IStringRepresentable {
        virtual ~IStringRepresentable() { }

        virtual String to_string() const = 0;
    };

}
