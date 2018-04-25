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

#include <gum/Try.h>

#include <functional>

namespace gum {

class Defer {
    using Deferrable = std::function<void()>;

  private:
    Deferrable _deferrable;

  public:
    template <typename Deferrable_>
    Defer(Deferrable_&& deferrable)
        : _deferrable(std::forward<Deferrable_>(deferrable)) {}

    Defer(Defer const&) = default;
    Defer(Defer&&) = default;

    ~Defer() {
        _deferrable();
    }

    Defer& operator=(Defer const&) = default;
    Defer& operator=(Defer&&) = default;
};

#define defer gum::Defer __defer__##__LINE__ = [&]()
}
