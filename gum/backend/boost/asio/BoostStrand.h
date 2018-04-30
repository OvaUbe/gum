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

#include <gum/async/ITaskQueue.h>

#include <boost/asio/strand.hpp>

namespace gum {
namespace asio {

class BoostStrand : public virtual ITaskQueue {
    using Service = boost::asio::io_service;
    GUM_DECLARE_REF(Service);

    using Impl = boost::asio::strand;

  private:
    ServiceRef _service;
    Impl _impl;

  public:
    BoostStrand(const ServiceRef& service)
        : _service(service)
        , _impl(*_service) {}

    virtual void push(Task&& task) override {
        _impl.post(std::move(task));
    }

    Impl& get_handle() {
        return _impl;
    }
};
GUM_DECLARE_REF(BoostStrand);
}
}
