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

#include <gum/backend/boost/asio/BoostStrand.h>
#include <gum/backend/boost/asio/BoostStreamDescriptor.h>
#include <gum/io/async/IAsyncByteStream.h>

#include <boost/asio.hpp>

namespace gum {
namespace asio {

class BoostAsyncByteStream : public virtual IAsyncByteStream {
    class Impl;
    GUM_DECLARE_REF(Impl);

    using Service = boost::asio::io_service;
    GUM_DECLARE_REF(Service);

  private:
    ImplRef _impl;

    Token _readCancellator;

  public:
    BoostAsyncByteStream(const BoostStrandRef& strand, BoostStreamDescriptor&& stream_descriptor, size_t buffer_size);

    Token read() override;
    Token read(u64 size) override;

    SignalHandle<DataReadSignature> data_read() const override;
};
}
}
