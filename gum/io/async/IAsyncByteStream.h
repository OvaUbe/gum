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

#include <gum/ErrorOr.h>
#include <gum/async/Signal.h>
#include <gum/io/ByteData.h>
#include <gum/io/Eof.h>
#include <gum/io/ISeekable.h>

#include <boost/variant.hpp>

namespace gum {
namespace asio {

struct IAsyncReadable {
    using ReadResult = ErrorOr<ConstByteData, Eof>;
    using DataReadSignature = void(const ReadResult&);

  public:
    virtual ~IAsyncReadable() {}

    virtual Token read() = 0;
    virtual Token read(u64 size) = 0;

    virtual SignalHandle<DataReadSignature> data_read() const = 0;
};
GUM_DECLARE_PTR(IAsyncReadable);
GUM_DECLARE_REF(IAsyncReadable);

struct IAsyncByteStream : public virtual IAsyncReadable {};
GUM_DECLARE_PTR(IAsyncByteStream);
GUM_DECLARE_REF(IAsyncByteStream);

struct ISeekableAsyncByteStream : public virtual IAsyncByteStream, public virtual ISeekable {};
GUM_DECLARE_PTR(ISeekableAsyncByteStream);
GUM_DECLARE_REF(ISeekableAsyncByteStream);
}
}
