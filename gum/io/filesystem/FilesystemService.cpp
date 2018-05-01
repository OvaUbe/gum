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
GUM_DECLARE_REF(FileDescriptor);
#else
#error File is not implemented
#endif

#ifdef GUM_USES_BOOST_ASIO
using AsyncByteStreamDescriptor = asio::BoostStreamDescriptor;
using AsyncByteStream = asio::BoostAsyncByteStream;
using IoWorker = asio::BoostIoWorker;
using Strand = asio::BoostStrand;
GUM_DECLARE_REF(Strand);
#else
#error Asio is not implemented
#endif

class SeekableStreamAdaptor : public virtual asio::ISeekableAsyncByteStream {
    ITaskQueueRef _task_queue;
    ISeekableRef _seekable;
    asio::IAsyncByteStreamRef _wrapped_stream;

    LifeToken _life_token;

  public:
    SeekableStreamAdaptor(const ITaskQueueRef& task_queue, const ISeekableRef& seekable, asio::IAsyncByteStreamRef&& wrapped_stream)
        : _task_queue(task_queue)
        , _seekable(seekable)
        , _wrapped_stream(std::move(wrapped_stream)) {}

    Token read() {
        return _wrapped_stream->read();
    }

    Token read(u64 size) {
        return _wrapped_stream->read(size);
    }

    SignalHandle<DataReadSignature> data_read() const {
        return _wrapped_stream->data_read();
    }

    void seek(s64 offset, SeekMode mode) {
        _task_queue->push(make_cancellable([this, offset, mode] { _seekable->seek(offset, mode); }, _life_token));
    }
};

class File : public virtual IFile {
    asio::ISeekableAsyncByteStreamRef _stream;

  public:
    File(asio::ISeekableAsyncByteStreamRef&& stream)
        : _stream(std::move(stream)) {}

    asio::ISeekableAsyncByteStreamRef get_stream() override {
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
        const FileDescriptorRef fd = make_shared_ref<FileDescriptor>(path, flags);
        AsyncByteStreamDescriptor sd(_worker.get_service(), fd->get_handle());

        const StrandRef strand = make_shared_ref<Strand>(_worker.get_service());
        asio::IAsyncByteStreamRef stream = make_shared_ref<AsyncByteStream>(strand, std::move(sd), async_buffer_size);
        asio::ISeekableAsyncByteStreamRef seekable_stream = make_shared_ref<SeekableStreamAdaptor>(strand, fd, std::move(stream));

        return make_shared_ref<File>(std::move(seekable_stream));
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
