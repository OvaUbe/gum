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

#include <gum/io/filesystem/FilesystemService.h>

#ifdef GUM_USES_POSIX
#include <gum/backend/posix/filesystem/FileDescriptor.h>
#endif

#ifdef GUM_USES_BOOST_ASIO
#include <gum/backend/boost/asio/BoostAsyncByteStream.h>
#include <gum/backend/boost/asio/BoostIoWorker.h>
#endif

namespace gum {

namespace {

#ifdef GUM_USES_POSIX
using FileDescriptor = posix::FileDescriptor;
#else
#error File is not implemented
#endif

#ifdef GUM_USES_BOOST_ASIO
using AsyncByteStreamDescriptor = asio::BoostStreamDescriptor;
using AsyncByteStream = asio::BoostAsyncByteStream;
using IoWorker = asio::BoostIoWorker;
#else
#error Asio is not implemented
#endif

class File : public virtual IFile {
    FileDescriptor _fd;
    asio::IAsyncByteStreamRef _stream;

  public:
    File(FileDescriptor&& fd, asio::IAsyncByteStreamRef&& stream)
        : _fd(std::move(fd))
        , _stream(std::move(stream)) {}

    asio::IAsyncByteStreamRef get_stream() override {
        return _stream;
    }
};
}

class FilesystemService::Impl {
    using Self = Impl;
    GUM_DECLARE_REF(Self);

  private:
    IoWorker _worker;

  public:
    Impl(const String& name, size_t concurrency_hint)
        : _worker(name, concurrency_hint) {}

    IFileRef open_file(const String& path, const FileOpenFlags& flags, size_t async_buffer_size) {
        FileDescriptor fd(path, flags);
        AsyncByteStreamDescriptor sd(_worker.get_service(), fd.get_handle());
        asio::IAsyncByteStreamRef stream = make_shared_ref<AsyncByteStream>(_worker.get_service(), std::move(sd), async_buffer_size);

        return make_shared_ref<File>(std::move(fd), std::move(stream));
    }
};

FilesystemService::FilesystemService(const String& name, size_t concurrency_hint)
    : _impl(make_unique_ref<Impl>(name, concurrency_hint)) {}

FilesystemService::~FilesystemService() {}

IFileRef FilesystemService::open_file(const String& path, const FileOpenFlags& flags, size_t async_buffer_size) {
    return _impl->open_file(path, flags, async_buffer_size);
}

IFileRef FilesystemService::open_file(const String& path, FileMode mode, size_t async_buffer_size) {
    return open_file(path, FileOpenFlags(mode), async_buffer_size);
}
}
