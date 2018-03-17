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

#include <gum/container/IObservableMap.h>

namespace gum {

    template < typename Key_, typename Value_, typename Impl_ >
    class ObservableStlMap : public virtual IObservableMap<Key_, Value_> {
        using Base = IObservableMap<Key_, Value_>;
        using ChangedSignature = typename Base::ChangedSignature;

    private:
        Impl_                       _impl;
        Signal<ChangedSignature>    _changed;

    public:
        template < typename ...Args_ >
        ObservableStlMap(Args_&& ...args)
            :   _impl(std::forward<Args_>(args)...),
                _changed([this](auto const& slot){ changedPopulator(slot); }) { }

        void set(const Key_& key, const Value_& value) override {
            gum::SignalLock l(get_mutex());

            MapOp op = MapOp::Added;
            const auto insertionResult = _impl.emplace(key, value);
            if (insertionResult.second) {
                insertionResult.first->second = value;
                op = MapOp::Updated;
            }

            _changed(op, key, value);
        }

        bool remove(const Key_& key) override {
            gum::SignalLock l(get_mutex());

            const auto iter = _impl.find(key);
            if (iter == _impl.end())
                return false;

            const Value_ value = iter->second;
            _impl.erase(iter);
            _changed(MapOp::Removed, key, value);

            return true;
        }

        void clear() override {
            gum::SignalLock l(get_mutex());

            Impl_ swapped;
            _impl.swap(swapped);

            for (const auto& kv : swapped)
                _changed(MapOp::Removed, kv.first, kv.second);
        }

        gum::Optional<Value_> get(const Key_& key) const override {
            gum::SignalLock l(get_mutex());

            const auto iter = _impl.find(key);
            if (iter == _impl.end())
                return nullptr;
            return iter->second;
        }

        bool contains(const Key_& key) const override {
            gum::SignalLock l(get_mutex());
            return _impl.count(key);
        }

        bool is_empty() const override {
            gum::SignalLock l(get_mutex());
            return _impl.empty();
        }

        size_t get_count() const override {
            gum::SignalLock l(get_mutex());
            return _impl.size();
        }

        const SignalMutex& get_mutex() const override {
            return _changed.get_mutex();
        }

        SignalHandle<ChangedSignature> changed() const override {
            return _changed.get_handle();
        }

    private:
        void changedPopulator(const std::function<ChangedSignature>& slot) {
            for (const auto& kv : _impl)
                slot(MapOp::Added, kv.first, kv.second);
        }
    };

}
