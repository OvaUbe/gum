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

#include <gum/Types.h>

#include <chrono>

namespace gum {

using SystemClock = std::chrono::system_clock;
using HighResClock = std::chrono::high_resolution_clock;
using SteadyClock = std::chrono::steady_clock;

template <typename Representation_, typename Period_>
using BasicDuration = std::chrono::duration<Representation_, Period_>;
using Duration = SystemClock::duration;

template <typename Clock_, typename Duration_>
using BasicTimePoint = std::chrono::time_point<Clock_, Duration_>;
using TimePoint = SystemClock::time_point;

using Nanoseconds = std::chrono::nanoseconds;
using Microseconds = std::chrono::microseconds;
using Milliseconds = std::chrono::milliseconds;
using Seconds = std::chrono::seconds;
using Minutes = std::chrono::minutes;
using Hours = std::chrono::hours;
using Days = std::chrono::duration<s64, std::ratio<24 * 60 * 60>>;

namespace detail {

template <typename Representation_, typename Period_>
class DurationCastProxy {
    using DurationType = BasicDuration<Representation_, Period_>;

  private:
    DurationType _wrapped;

  public:
    DurationCastProxy(DurationType const& wrapped)
        : _wrapped(wrapped) {}

    template <typename To_>
    operator To_() const {
        return std::chrono::duration_cast<To_>(_wrapped);
    }
};
}

template <typename Representation_, typename Period_>
auto duration_caster(BasicDuration<Representation_, Period_> const& duration) {
    return detail::DurationCastProxy<Representation_, Period_>(duration);
}
}
