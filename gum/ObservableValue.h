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

#include <gum/IObservableValue.h>

namespace gum {

    template < typename Value_, typename Equals_ = std::equal_to<> >
    class ObservableValue : public virtual IObservableValue<Value_> {
        using Self = ObservableValue;
        using Base = IObservableValue<Value_>;

        using PassingValue = typename Base::PassingValue;
        using ChangedSignature = typename Base::ChangedSignature;

    private:
        Equals_                     _equals;

        Value_                      _value;
        Signal<ChangedSignature>    _changed;

    public:
        ObservableValue(Value_ const& value)
            :   _value(value),
                _changed([this](auto const& slot) { slot(_value); }) { }

        ObservableValue(Value_&& value = Value_())
            :   _value(std::move(value)),
                _changed([this](auto const& slot) { slot(_value); }) { }

        void set(PassingValue value) override {
            SignalLock l(get_mutex());

            if (_equals(_value, value))
                return;

            _value = value;
            _changed(_value);
        }

        Value_ get() const override {
            SignalLock l(get_mutex());

            return _value;
        }

        const SignalMutex& get_mutex() const override {
            return _changed.get_mutex();
        }

        SignalHandle<ChangedSignature> changed() const override {
            return _changed.get_handle();
        }

        Self& operator=(PassingValue value) {
            set(value);
            return *this;
        }

        operator Value_() const {
            return get();
        }
    };

}
