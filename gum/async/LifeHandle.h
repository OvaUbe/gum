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

#include <gum/smartpointer/SharedPtr.h>
#include <gum/smartpointer/SharedReference.h>

namespace gum {

struct ILifeHandle {
    virtual ~ILifeHandle() {}

    virtual bool lock() const = 0;
    virtual void unlock() const = 0;
};
GUM_DECLARE_PTR(ILifeHandle);
GUM_DECLARE_REF(ILifeHandle);

class LifeHandle {
    using Impl = ILifeHandle;
    GUM_DECLARE_REF(Impl);

    struct DummyImpl : public virtual ILifeHandle {
        bool lock() const override {
            return true;
        }

        void unlock() const override {}
    };

  private:
    ImplRef _impl;

  public:
    LifeHandle(ImplRef const& impl = make_shared_ref<DummyImpl>())
        : _impl(impl) {}

    bool lock() const {
        return _impl->lock();
    }

    void unlock() const {
        _impl->unlock();
    }
};

class LifeHandleLock {
    LifeHandle _handle;
    bool _alive;

  public:
    LifeHandleLock(LifeHandle const& handle)
        : _handle(handle)
        , _alive(_handle.lock()) {}

    ~LifeHandleLock() {
        _handle.unlock();
    }

    explicit operator bool() const {
        return _alive;
    }
};
}
