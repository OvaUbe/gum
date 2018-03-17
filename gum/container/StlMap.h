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

#include <gum/container/IMap.h>

namespace gum {

    template < typename Key_, typename Value_, typename Impl_ >
    class StlMap : public virtual IMap<Key_,Value_> {
        Impl_ _impl;

    public:
        template < typename ...Args_ >
        StlMap(Args_&& ...args)
            :   _impl(std::forward<Args_>(args)...) { }

        void set(const Key_& key, const Value_& value) override {
            _impl[key] = value;
        }

        bool remove(const Key_& key) override {
            const auto iter = _impl.find(key);
            if (iter == _impl.end())
                return false;

            _impl.erase(iter);
            return true;
        }

        void clear() override {
            _impl.clear();
        }

        gum::Optional<Value_> get(const Key_& key) const override {
            const auto iter = _impl.find(key);
            if (iter == _impl.end())
                return nullptr;
            return iter->second;
        }

        bool contains(const Key_& key) const override {
            return _impl.count(key);
        }

        bool is_empty() const override {
            return _impl.empty();
        }

        size_t get_count() const override {
            return _impl.size();
        }
    };

}
