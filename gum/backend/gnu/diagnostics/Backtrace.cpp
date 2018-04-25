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

#include <gum/backend/gnu/diagnostics/Backtrace.h>

#include <array>
#include <sstream>

#include <unwind.h>

namespace gum {
namespace gnu {

namespace {

using BacktraceArray = std::array<uintptr_t, 64>;

struct BacktraceHolder {
    using Self = BacktraceHolder;

  private:
    BacktraceArray _array;
    size_t _size;
    _Unwind_Word _cfa;

  public:
    BacktraceHolder()
        : _size()
        , _cfa() {
        _Unwind_Backtrace(&Self::UnwindCallback, this);
    }

    BacktraceArray const& get_array() const {
        return _array;
    }

    size_t get_size() const {
        return _size;
    }

  private:
    static _Unwind_Reason_Code UnwindCallback(struct _Unwind_Context* ctx, void* self) {
        return static_cast<BacktraceHolder*>(self)->UnwindFrame(ctx);
    }

    _Unwind_Reason_Code UnwindFrame(struct _Unwind_Context* ctx) {
        _Unwind_Ptr ip = _Unwind_GetIP(ctx);
        _Unwind_Word cfa = _Unwind_GetCFA(ctx);
        if (_size != 0 && _array[_size - 1] == ip && cfa == _cfa)
            return _URC_END_OF_STACK;

        _cfa = cfa;
        _array[_size++] = ip;

        return (_size >= _array.size()) ? _URC_END_OF_STACK : _URC_NO_REASON;
    }
};
}

std::string BacktraceGetter::operator()() const {
    BacktraceHolder backtrace;

    std::stringstream ss;
    for (size_t i = 0; i < backtrace.get_size(); ++i)
        ss << std::hex << "0x" << backtrace.get_array()[i] << " ";

    return ss.str();
}
}
}
