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

#include <deque>

#include <gum/container/Lru.h>

namespace gum {

/**
 * @brief The TwoQ class is a cache, which implements the 2Q caching algorithm.
 *     If you are familiar with Rust containers, you will quickly
 *     get used to the interface.
 * @note The term "use" when applied to the cache item, means, that cache will most likely
 *     raise it's priority.
 */
template <typename K, typename V>
class TwoQ {
    /* Methods */
public:
    /**
     * @brief TwoQ - ctor.
     * @param capacity - cache maximim capacity (including).
     */
    explicit TwoQ(const size_t capacity);

    /**
     * @brief TwoQ - ctor.
     * @param capacity - cache maximum capacity (including).
     * @param size_mapper - a function of interface (const V&) -> size_t,
     *     which maps the provided value with it's logical size.
     */
    template <typename F>
    explicit TwoQ(const size_t capacity, F&& size_mapper);

    TwoQ(const TwoQ& copy) = delete;
    TwoQ(TwoQ&& move) = default;
    ~TwoQ() = default;

    TwoQ& operator=(const TwoQ& copy) = delete;
    TwoQ& operator=(TwoQ&& move) = default;

    /**
     * @brief size - current number of items in cache.
     * @note If you are using custom size mapper function,
     *     this will not be equal to the current logical size.
     * @return size.
     */
    size_t size() const;

    /**
     * @brief empty - check if cache is empty.
     * @return empty?
     */
    bool empty() const;

    /**
     * @brief insert - insert (or assign) an item to the cache.
     * @param key - key.
     * @param value - value.
     * @return old value, associated with this key, if any.
     */
    template <typename Key, typename Value>
    std::experimental::optional<V> insert(Key&& key, Value&& value);

    /**
     * @brief invalidate - remove value, associated with this key, from cache.
     * @param key - key.
     * @return valud, associated with this key, if any.
     */
    std::experimental::optional<V> invalidate(const K& key);

    /**
     * @brief get_mut - retrieve a pointer to const value,
     *     associated with the key and use the value.
     * @param key - key.
     * @return a pointer to the value, or nullptr if not found.
     * @note You should not store this pointer to yourself, as it may invalidate
     *     in the future.
     */
    const V* get(const K& key);

    /**
     * @brief get - retrieve a pointer to mutable value,
     *     associated with the key and use the value.
     * @param key - key.
     * @return a pointer to the value, or nullptr if not found.
     * @note You should not store this pointer to yourself, as it may invalidate
     *     in the future.
     */
    V* get_mut(const K& key);

    /**
     * @brief get - retrieve a pointer to const value, associated with the key.
     * @param key - key.
     * @return a pointer to the value, or nullptr if not found.
     * @note You should not store this pointer to yourself, as it may invalidate
     *     in the future.
     * @note This method does not use the value.
     */
    const V* peek(const K& key) const;

    /**
     * @brief get - retrieve a pointer to mutable value, associated with the key.
     * @param key - key.
     * @return a pointer to the value, or nullptr if not found.
     * @note You should not store this pointer to yourself, as it may invalidate
     *     in the future.
     * @note This method does not use the value.
     */
    V* peek_mut(const K& key);

    /**
     * @brief contains - is there a value associated with this key?
     * @param key - key.
     * @return true if found, false otherwise.
     * @note Due to the algorhitm nature, you cannot expect that
     *     second assert will not fire, because get() may invalidate the key:
     *
     *     assert(cache.contains(key));
     *     assert(cache.get(key) != nullptr);
     */
    bool contains(const K& key) const;

    /**
     * @brief contains - use the value, associated with the key.
     * @param key - key.
     * @return true if found, false otherwise.
     */
    bool touch(const K& key);

private:
    template <typename Key, typename Value>
    std::experimental::optional<V> _push(Key&& key, Value&& value);
    void _pipeline_items();

    /* Fields */
private:
    std::function<size_t(const V&)> m_size_mapper;
    std::deque<std::pair<K, V>> m_a_in;
    std::deque<std::pair<K, V>> m_a_out;
    LRU<K, V> m_a_m;

    const size_t m_in_cap;
    const size_t m_out_cap;

};

template <typename K, typename V>
TwoQ<K, V>::TwoQ(const size_t capacity)
    :
      m_size_mapper([](const V&) -> size_t { return 1; })
    , m_a_in()
    , m_a_out()
    , m_a_m(capacity - size_t(0.75f * float(capacity)), [](const V&) -> size_t { return 1; })
    , m_in_cap(size_t(0.25f * float(capacity)))
    , m_out_cap(size_t(0.5f * float(capacity))) { }

template <typename K, typename V>
template <typename F>
TwoQ<K, V>::TwoQ(const size_t capacity, F&& size_mapper)
    :
      m_size_mapper(size_mapper)
    , m_a_in()
    , m_a_out()
    , m_a_m(capacity - size_t(0.75f * float(capacity)), std::forward<F>(size_mapper))
    , m_in_cap(size_t(0.25f * float(capacity)))
    , m_out_cap(size_t(0.5f * float(capacity))) { }

template <typename K, typename V>
size_t TwoQ<K, V>::size() const {
    return m_a_in.size() + m_a_out.size() + m_a_m.size();
}

template <typename K, typename V>
bool TwoQ<K, V>::empty() const {
    return m_a_in.empty() && m_a_out.empty() && m_a_m.empty();
}

template <typename K, typename V>
template <typename Key, typename Value>
std::experimental::optional<V> TwoQ<K, V>::insert(Key&& key, Value&& value) {
    std::experimental::optional<V> ret;

    if (!m_in_cap || !m_out_cap) {
        return ret;
    }

    ret = _push(std::forward<Key>(key), std::forward<Value>(value));

    _pipeline_items();
    return ret;
}

template <typename K, typename V>
std::experimental::optional<V> TwoQ<K, V>::invalidate(const K& key) {
    if (m_a_m.contains(key)) {
        return m_a_m.invalidate(key);
    }

    for (auto iter = m_a_in.begin(); iter != m_a_in.end(); ++iter) {
        if (iter->first == key) {
            auto ret = std::experimental::make_optional(std::move(iter->second));
            m_a_in.erase(iter);
            return ret;
        }
    }
    for (auto iter = m_a_out.begin(); iter != m_a_out.end(); ++iter) {
        if (iter->first == key) {
            auto ret = std::experimental::make_optional(std::move(iter->second));
            m_a_out.erase(iter);
            return ret;
        }
    }

    return std::experimental::nullopt;
}

template <typename K, typename V>
const V* TwoQ<K, V>::get(const K& key) {
    if (m_a_m.contains(key)) {
        return m_a_m.get_mut(key);
    }

    for (auto& kv : m_a_in) {
        if (key == kv.first) {
            return &(kv.second);
        }
    }

    for (auto iter = m_a_out.begin(); iter != m_a_out.end(); ++iter) {
        if (key == iter->first) {
            /* Transfer from Aout to Am */
            m_a_m.insert(std::move(iter->first), std::move(iter->second));
            m_a_out.erase(iter);
            return m_a_m.peek_mut(key);
        }
    }

    return nullptr;
}

template <typename K, typename V>
V* TwoQ<K, V>::get_mut(const K& key) {
    return const_cast<V*>(get(key));
}

template <typename K, typename V>
const V* TwoQ<K, V>::peek(const K& key) const {
    if (m_a_m.contains(key)) {
        return m_a_m.peek(key);
    }

    for (const auto& kv : m_a_in) {
        if (key == kv.first) {
            return &(kv.second);
        }
    }

    for (const auto& kv : m_a_out) {
        if (key == kv.first) {
            return &(kv.second);
        }
    }

    return nullptr;
}

template <typename K, typename V>
V* TwoQ<K, V>::peek_mut(const K& key) {
    return const_cast<V*>(peek(key));
}

template <typename K, typename V>
bool TwoQ<K, V>::contains(const K& key) const {
    if (m_a_m.contains(key)) {
        return true;
    }

    for (const auto& kv : m_a_in) {
        if (key == kv.first) {
            return true;
        }
    }

    for (const auto& kv : m_a_out) {
        if (key == kv.first) {
            return true;
        }
    }

    return false;
}

template <typename K, typename V>
bool TwoQ<K, V>::touch(const K& key) {
    return bool(get(key));
}

template <typename K, typename V>
template <typename Key, typename Value>
std::experimental::optional<V> TwoQ<K, V>::_push(Key&& key, Value&& value) {
    if (m_a_m.contains(key)) {
        return m_a_m.insert(std::forward<Key>(key), std::forward<Value>(value));
    }

    for (auto& kv : m_a_in) {
        if (kv.first == key) {
            auto ret = std::experimental::make_optional(std::move(kv.second));
            kv.second = std::forward<Value>(value);
            return ret;
        }
    }

    for (auto& kv : m_a_out) {
        if (kv.first == key) {
            auto ret = std::experimental::make_optional(std::move(kv.second));
            kv.second = std::forward<Value>(value);
            return ret;
        }
    }

    m_a_in.emplace_back(std::forward<Key>(key), std::forward<Value>(value));
    return std::experimental::nullopt;
}

template <typename K, typename V>
void TwoQ<K, V>::_pipeline_items() {
    size_t a_in_size(0);
    for (const auto& item : m_a_in) {
        a_in_size += m_size_mapper(item.second);
    }

    while (a_in_size > m_in_cap) {
        size_t erased = m_size_mapper(m_a_in.front().second);
        a_in_size -= erased;
        m_a_out.push_back(std::move(m_a_in.front()));
        m_a_in.pop_front();
    }

    size_t a_out_size(0);
    for (const auto& item : m_a_out) {
        a_out_size += m_size_mapper(item.second);
    }

    while (a_out_size > m_out_cap) {
        size_t erased = m_size_mapper(m_a_out.front().second);
        a_out_size -= erased;
        m_a_out.pop_front();
    }
}


}
