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

#include <gum/smartpointer/SharedPtr.h>
#include <gum/smartpointer/SharedReference.h>
#include <gum/exception/Exception.h>

namespace gum {

    template < typename Char_ >
    class BasicString {
        using Self = BasicString<Char_>;

        using Impl_ = std::basic_string<Char_>;

    public:
        using value_type = typename Impl_::value_type;

        using pointer = typename Impl_::pointer;
        using const_pointer = typename Impl_::const_pointer;
        using reference = typename Impl_::reference;
        using const_reference = typename Impl_::const_reference;

        using iterator = typename Impl_::iterator;
        using const_iterator = typename Impl_::const_iterator;
        using reverse_iterator = typename Impl_::reverse_iterator;
        using const_reverse_iterator = typename Impl_::const_reverse_iterator;

    public:
        Impl_ _impl;

    public:
        BasicString() = default;
        BasicString(Self const&) = default;
        BasicString(Self&&) = default;

        BasicString(Impl_ const& impl)
            :   _impl(impl)
        { }

        BasicString(Impl_&& impl)
            :   _impl(std::move(impl))
        { }

        BasicString(const_pointer cstring)
            :   _impl(cstring)
        { }

        template < typename InputIterator_ >
        BasicString(InputIterator_ first, InputIterator_ last)
            :   _impl(first, last)
        { }

        Self& operator=(Self const&) = default;
        Self& operator=(Self&&) = default;

        Self& operator=(Impl_ const& impl) {
            _impl = impl;
            return self;
        }

        Self& operator=(Impl_&& impl) {
            _impl = std::move(impl);
            return self;
        }

        Self& operator=(const_pointer cstring) {
            _impl = cstring;
            return self;
        }

        reference at(size_t index){
            GUM_CHECK_INDEX(index, size());
            return _impl[index];
        }

        const_reference at(size_t index) const {
            GUM_CHECK_INDEX(index, size());
            return _impl[index];
        }

        reference operator[](size_t index) const {
            return at(index);
        }

        const_reference operator[](size_t index) {
            return at(index);
        }

        reference front() {
            return at(0);
        }

        const_reference front() const {
            return at(0);
        }

        reference back() {
            GUM_CHECK_INDEX(0, size());
            return _impl.back();
        }

        const_reference back() const {
            GUM_CHECK_INDEX(0, size());
            return _impl.back();
        }

        pointer data() {
            return _impl.data();
        }

        const_pointer data() const {
            return _impl.data();
        }

        const_pointer c_str() const {
            return _impl.c_str();
        }

        size_t size() const {
            return _impl.size();
        }

        size_t length() const {
            return _impl.length();
        }

        bool empty() const {
            return _impl.empty();
        }

        iterator begin() {
            return _impl.begin();
        }

        const_iterator begin() const {
            return _impl.begin();
        }

        const_iterator cbegin() const {
            return _impl.cbegin();
        }

        iterator end() {
            return _impl.end();
        }

        const_iterator end() const {
            return _impl.cend();
        }

        const_iterator cend() const {
            return _impl.cend();
        }

        reverse_iterator rbegin() {
            return _impl.rbegin();
        }

        const_reverse_iterator rbegin() const {
            return _impl.cbegin();
        }

        const_reverse_iterator crbegin() const {
            return _impl.cbegin();
        }

        reverse_iterator rend() {
            return _impl.rend();
        }

        const_reverse_iterator rend() const {
            return _impl.crend();
        }

        const_reverse_iterator crend() const {
            return _impl.crend();
        }

        explicit operator Impl_ () const& {
            return _impl;
        }

        explicit operator Impl_ () && {
            return std::move(_impl);
        }

        Self& operator<<(Self const& string) {
            _impl += string._impl;
            return self;
        }

        Self& operator<<(Self&& string) {
            _impl += std::move(string._impl);
            return self;
        }

        Self& operator<<(const_pointer cstring) {
            _impl += cstring;
            return self;
        }

        Self& operator<<(value_type ch) {
            _impl += ch;
            return self;
        }
    };


    using String = BasicString<char>;
    GUM_DECLARE_PTR(String);
    GUM_DECLARE_REF(String);

}
