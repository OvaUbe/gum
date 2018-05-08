#include <gum/backend/boost/asio/BoostAsyncByteStream.h>

#include <gum/Unit.h>
#include <gum/Visit.h>
#include <gum/backend/boost/BoostSystemException.h>

namespace gum {
namespace asio {

using namespace std::placeholders;

using OperationCancellatorPair = std::pair<Token, Token>;

class BoostAsyncByteStream::Impl {
    using Self = Impl;
    GUM_DECLARE_REF(Self);

    using ReadResult = IAsyncReadable::ReadResult;
    using DataReadSignature = IAsyncReadable::DataReadSignature;

    using Service = BoostAsyncByteStream::Service;
    GUM_DECLARE_REF(Service);

    using Buffer = std::vector<u8>;

    struct ReadUntilEof {};
    using ReadSize = u64;
    using ReadOperation = boost::variant<ReadUntilEof, ReadSize>;

    struct CancellableReadOperation {
        ReadOperation Op;
        std::atomic<bool> Cancelled;

      public:
        CancellableReadOperation(ReadOperation&& op)
            : Op(std::move(op))
            , Cancelled() {}
    };
    GUM_DECLARE_REF(CancellableReadOperation);

  private:
    static Logger _logger;

    BoostStrandRef _strand;
    BoostStreamDescriptor _stream_descriptor;

    Buffer _read_buffer;
    Signal<DataReadSignature> _data_read;

  public:
    Impl(const BoostStrandRef& strand, BoostStreamDescriptor&& stream_descriptor, size_t buffer_size)
        : _strand(strand)
        , _stream_descriptor(std::move(stream_descriptor))
        , _read_buffer(buffer_size) {
        GUM_CHECK(buffer_size, ArgumentException("buffer_size", buffer_size));
    }

    OperationCancellatorPair read(const SelfRef& self) {
        const auto op = make_shared_ref<CancellableReadOperation>(ReadUntilEof());
        _strand->get_handle().post([=]() { self->submit_read(self, op); });

        return make_read_cancellators(op);
    }

    OperationCancellatorPair read(const SelfRef& self, u64 size) {
        GUM_CHECK(size, ArgumentException("size", size));

        const auto op = make_shared_ref<CancellableReadOperation>(ReadSize(size));
        _strand->get_handle().post([=]() { self->submit_read(self, op, size); });

        return make_read_cancellators(op);
    }

    SignalHandle<DataReadSignature> data_read() const {
        return _data_read.get_handle();
    }

  private:
    void on_data_read(const SelfRef& self, const CancellableReadOperationRef& op, const boost::system::error_code& error, size_t length) {
        if (op->Cancelled) {
            _data_read(ExceptionRef(make_shared_ref<OperationCancelledException>()));
            return;
        }

        const bool is_eof = error && error == boost::asio::error::eof;
        if (error && !is_eof) {
            _data_read(ExceptionRef(make_shared_ref<BoostSystemException>(error)));
            return;
        }

        if (length)
            _data_read(ConstByteData(_read_buffer.data(), length));

        if (op->Cancelled)
            return;

        if (is_eof) {
            _data_read(Eof());
            return;
        }

        visit(
            op->Op,
            [&, this](ReadUntilEof) { submit_read(self, op); },
            [&, this](ReadSize& size) {
                GUM_CHECK(size >= length, InternalError(String() << "Read operation size " << size << " is less than read length " << length));
                size -= length;
                if (size)
                    submit_read(self, op, size);
            });
    }

    auto make_on_data_read(const SelfRef& self, const CancellableReadOperationRef& op) {
        return try_(std::bind(&Self::on_data_read, this, self, op, _1, _2), LogLevel::Error, "Exception from on_data_read handler", _logger);
    }

    void submit_read(const SelfRef& self, const CancellableReadOperationRef& op) {
        boost::asio::async_read(
            _stream_descriptor.get_handle(),
            boost::asio::buffer(_read_buffer.data(), _read_buffer.size()),
            _strand->get_handle().wrap(make_on_data_read(self, op)));
    }

    void submit_read(const SelfRef& self, const CancellableReadOperationRef& op, u64 size) {
        GUM_CHECK(size, ArgumentException("size", size));

        boost::asio::async_read(
            _stream_descriptor.get_handle(),
            boost::asio::buffer(_read_buffer.data(), std::min<u64>(_read_buffer.size(), size)),
            _strand->get_handle().wrap(make_on_data_read(self, op)));
    }

    Token make_read_cancellator(const CancellableReadOperationRef& op) const {
        return make_token<FunctionToken>([=] { op->Cancelled = true; });
    }

    OperationCancellatorPair make_read_cancellators(const CancellableReadOperationRef& op) {
        return std::make_pair(make_read_cancellator(op), make_read_cancellator(op));
    }
};
GUM_DEFINE_NAMED_LOGGER(BoostAsyncByteStream::Impl, BoostAsyncByteStream);

BoostAsyncByteStream::BoostAsyncByteStream(const BoostStrandRef& strand, BoostStreamDescriptor&& stream_descriptor, size_t buffer_size)
    : _impl(make_shared_ref<Impl>(strand, std::move(stream_descriptor), buffer_size)) {}

Token BoostAsyncByteStream::read() {
    auto operation_token_pair = _impl->read(_impl);
    _readCancellator = std::move(operation_token_pair.first);
    return std::move(operation_token_pair.second);
}

Token BoostAsyncByteStream::read(u64 size) {
    auto operation_token_pair = _impl->read(_impl, size);
    _readCancellator = std::move(operation_token_pair.first);
    return std::move(operation_token_pair.second);
}

SignalHandle<BoostAsyncByteStream::DataReadSignature> BoostAsyncByteStream::data_read() const {
    return _impl->data_read();
}
}
}
