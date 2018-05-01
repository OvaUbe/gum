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

#include <utility>

namespace gum {

template <typename...>
struct Overload;

template <typename Head_, typename... Tail_>
struct Overload<Head_, Tail_...> : Head_, Overload<Tail_...> {
    Overload(Head_&& head, Tail_&&... tail)
        : Head_(std::forward<Head_>(head))
        , Overload<Tail_...>(std::forward<Tail_>(tail)...) {}

    using Head_::operator();
    using Overload<Tail_...>::operator();
};

template <typename Head_>
struct Overload<Head_> : Head_ {
    Overload(Head_&& head)
        : Head_(std::forward<Head_>(head)) {}

    using Head_::operator();
};

template <typename... List_>
auto overload(List_&&... list) {
    return Overload<List_...>(std::forward<List_>(list)...);
}
}
