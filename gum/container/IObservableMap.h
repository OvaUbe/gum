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

#include <gum/async/Signal.h>
#include <gum/container/ContainerOp.h>
#include <gum/container/IMap.h>

namespace gum {

template <typename Key_, typename Value_>
struct IReadonlyObservableMap : public virtual IReadonlyMap<Key_, Value_> {
    using ChangedSignature = void(MapOp, Key_ const&, Value_ const&);

  public:
    virtual const SignalMutex& get_mutex() const = 0;

    virtual SignalHandle<ChangedSignature> changed() const = 0;
};

template <typename Key_, typename Value_>
struct IObservableMap : public virtual IMap<Key_, Value_>, public virtual IReadonlyObservableMap<Key_, Value_> {
    using Base = IReadonlyObservableMap<Key_, Value_>;
    using ChangedSignature = typename Base::ChangedSignature;
};
}
