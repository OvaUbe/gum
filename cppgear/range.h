/*
 * Copyright (c) 2016 Vladimir Golubev
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

#include <cppgear/container/iterator.h>

namespace cppgear {

    template < typename Iterable_ >
    class Range {
        class Counter  : public IteratorBase<Counter,
                                              std::bidirectional_iterator_tag,
                                              Iterable_
                                             > {
            using Base = IteratorBase<Counter,
                                      std::bidirectional_iterator_tag,
                                      Iterable_
                                     >;

        public:
            using value_type = typename Base::value_type;
            using difference_type = typename Base::difference_type;
            using pointer = typename Base::value_type*;
            using reference = typename Base::value_type&;
            using iterator_category = typename Base::iterator_category;

        public:
            explicit Counter(Iterable_ iterable, Iterable_ step) :
                m_iterable(iterable),
                m_step(step) { }

            bool equals(Counter const& other) const {
                return m_iterable == other.m_iterable;
            }

            reference dereference() {
                return m_iterable;
            }

            void increment() {
                m_iterable += m_step;
            }

            void decrement() {
                m_iterable -= m_step;
            }

        private:
            Iterable_ m_iterable;
            Iterable_ m_step;
        };

    public:
        Range(Iterable_ first, Iterable_ last, Iterable_ step) :
            m_begin(first, step),
            m_end(last, step) { }

        Counter begin() {
            return m_begin;
        }

        Counter begin() const {
            return m_begin;
        }

        Counter cbegin() {
            return m_begin;
        }

        Counter end() {
            return m_end;
        }

        Counter end() const {
            return m_end;
        }

        Counter cend() {
            return m_end;
        }

    private:
        Counter    m_begin;
        Counter    m_end;
    };

    template < typename Iterable_ >
    Range<Iterable_> range(Iterable_ begin, Iterable_ end, Iterable_ step = Iterable_(1)) {
        return Range<Iterable_>(begin, end, step);
    }

}
