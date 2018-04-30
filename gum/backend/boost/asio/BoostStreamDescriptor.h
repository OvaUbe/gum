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

#include <gum/smartpointer/SharedReference.h>

#include <boost/asio/io_service.hpp>

#ifdef GUM_USES_POSIX
#include <boost/asio/posix/stream_descriptor.hpp>
#endif

namespace gum {
namespace asio {

class BoostStreamDescriptor {
#ifdef GUM_USES_POSIX
    using Impl = boost::asio::posix::stream_descriptor;
#else
#error Boost stream descriptor is not implemented
#endif

    using Service = boost::asio::io_service;
    GUM_DECLARE_REF(Service);

  private:
    ServiceRef _service;
    Impl _impl;

  public:
    template <typename LowLevelDescriptor_>
    BoostStreamDescriptor(const ServiceRef& service, LowLevelDescriptor_&& low_level_descriptor)
        : _service(service)
        , _impl(*_service, std::move(low_level_descriptor)) {}

    Impl& get() {
        return _impl;
    }
};
}
}
