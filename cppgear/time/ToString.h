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

#include <cppgear/string/ToString.h>
#include <cppgear/time/Types.h>

namespace cppgear {

    String to_string(TimePoint const& point);


    template < typename Representation_, typename Period_ >
    String to_string(BasicDuration<Representation_, Period_> const& duration) {
        s64 const ms = duration_cast<Milliseconds>(duration).count();
        s64 const sec = duration_cast<Seconds>(duration).count();
        s64 const min = duration_cast<Minutes>(duration).count();

        return String() << "[" << min << ":" << sec << ":" << ms << "";
    }

    inline String to_string(Seconds const& duration) {
        return String() << duration.count() << " seconds";
    }

    inline String to_string(Minutes const& duration) {
        return String() << duration.count() << " minutes";
    }

    inline String to_string(Hours const& duration) {
        return String() << duration.count() << " hours";
    }

}
