/*
 * Copyright (c) Vladimir Golubev
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <gum/Slice.h>

#include <cstring>

namespace gum {

template <typename Char_>
class BasicStringLiteral : public Slice<const Char_> {
    using Base = Slice<const Char_>;

  public:
    using value_type = typename Base::value_type;

    using const_pointer = typename Base::const_pointer;
    using pointer = typename Base::pointer;
    using const_reference = typename Base::value_type;
    using reference = typename Base::const_reference;

    using const_iterator = typename Base::const_iterator;
    using iterator = typename Base::iterator;
    using const_reverse_iterator = typename Base::const_reverse_iterator;
    using reverse_iterator = typename Base::reverse_iterator;

  public:
    BasicStringLiteral(pointer str)
        : Base(str, strlen(str)) {}

    pointer c_str() const {
        return Base::data();
    }

    String to_string() const {
        return String(Base::begin(), Base::end());
    }
};

using StringLiteral = BasicStringLiteral<char>;
}
