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

#include <unordered_map>
#include <chrono>
#include <functional>
#include <cassert>

namespace cppgear {

/**
 * @brief The LRU class is a cache, which implements the LRU (least recently used)
 *     caching algorithm. If you are familiar with Rust containers, you will quickly
 *     get used to the interface.
 * @note The term "use" when applied to the cache item, means, that cache will most likely
 *     raise it's priority.
 */
template <typename K, typename V>
class LRU {
    /* Inner types */
public:
    using Timestamp = std::chrono::time_point<std::chrono::steady_clock>;

    /* Methods */
public:
    /**
     * @brief LRU - ctor.
     * @param capacity - cache maximim capacity (including).
     */
    explicit LRU(const size_t capacity);

    /**
     * @brief LRU - ctor.
     * @param capacity - cache maximum capacity (including).
     * @param size_mapper - a function of interface (const V&) -> size_t,
     *     which maps the provided value with it's logical size.
     */
    template <typename F>
    explicit LRU(const size_t capacity, F&& size_mapper);

    LRU(const LRU& copy) = delete;
    LRU(LRU&& move) = default;
    ~LRU() = default;

    LRU& operator=(const LRU& copy) = delete;
    LRU& operator=(LRU&& move) = default;

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
     */
    bool contains(const K& key) const;

    /**
     * @brief contains - use the value, associated with the key.
     * @param key - key.
     * @return true if found, false otherwise.
     */
    bool touch(const K& key);

private:
    void _remove_expired();
    size_t _remove_least_recent();
    Timestamp _least_recent_timestamp() const;

    /* Fields */
private:
    std::function<size_t(const V&)> m_size_mapper;
    std::unordered_map<K, std::pair<V, Timestamp>> m_pool;

    const size_t m_cap;

};

template <typename K, typename V>
LRU<K, V>::LRU(const size_t capacity)
    :
      m_size_mapper([](const V&) -> size_t { return 1; })
    , m_pool()
    , m_cap(capacity) { }

template <typename K, typename V>
template <typename F>
LRU<K, V>::LRU(const size_t capacity, F&& size_mapper)
    :
      m_size_mapper(std::forward<F>(size_mapper))
    , m_pool()
    , m_cap(capacity) { }

template <typename K, typename V>
size_t LRU<K, V>::size() const {
    return m_pool.size();
}

template <typename K, typename V>
bool LRU<K, V>::empty() const {
    return m_pool.empty();
}

template <typename K, typename V>
template <typename Key, typename Value>
std::experimental::optional<V> LRU<K, V>::insert(Key&& key, Value&& value) {
    std::experimental::optional<V> ret;
    if (!m_cap) {
        return ret;
    }

    auto iter = m_pool.find(key);
    if (iter != m_pool.end()) {
        ret = std::experimental::make_optional(std::move(iter->second.first));
        m_pool.erase(iter);
    }

    auto now = std::chrono::steady_clock::now();
    m_pool.emplace(std::forward<Key>(key),
                   std::pair<V, Timestamp>{ std::forward<Value>(value), std::move(now) });

    _remove_expired();
    return ret;
}

template <typename K, typename V>
std::experimental::optional<V> LRU<K, V>::invalidate(const K& key) {
    std::experimental::optional<V> ret;

    auto iter = m_pool.find(key);
    if (iter == m_pool.end()) {
        return ret;
    }

    ret = std::experimental::make_optional(std::move(iter->second.first));
    m_pool.erase(iter);

    return ret;
}

template <typename K, typename V>
const V* LRU<K, V>::get(const K& key) {
    auto iter = m_pool.find(key);
    if (iter == m_pool.end()) {
        return nullptr;
    }
    iter->second.second = std::chrono::steady_clock::now();
    return &(iter->second.first);
}

template <typename K, typename V>
V* LRU<K, V>::get_mut(const K& key) {
    return const_cast<V*>(get(key));
}

template <typename K, typename V>
const V* LRU<K, V>::peek(const K& key) const {
    const auto iter = m_pool.find(key);
    if (iter == m_pool.end()) {
        return nullptr;
    }
    return &(iter->second.first);
}

template <typename K, typename V>
V* LRU<K, V>::peek_mut(const K& key) {
    return const_cast<V*>(peek(key));
}

template <typename K, typename V>
bool LRU<K, V>::contains(const K& key) const {
    return m_pool.find(key) != m_pool.end();
}

template <typename K, typename V>
bool LRU<K, V>::touch(const K& key) {
    return bool(get(key));
}

template <typename K, typename V>
void LRU<K, V>::_remove_expired() {
    size_t cur_size(0);
    for (const auto& kv : m_pool) {
        cur_size += m_size_mapper(kv.second.first);
    }

    while (cur_size > m_cap) {
        size_t erased = _remove_least_recent();
        cur_size -= erased;
    }
}

template <typename K, typename V>
size_t LRU<K, V>::_remove_least_recent() {
    size_t erased(0);
    auto least_recent_ts = _least_recent_timestamp();

    for (auto iter = m_pool.begin(); iter != m_pool.end(); ++iter) {
        if (least_recent_ts == iter->second.second) {
            erased = m_size_mapper(iter->second.first);
            m_pool.erase(iter);
            return erased;
        }
    }

    assert(false);
    return erased;
}

template <typename K, typename V>
typename LRU<K, V>::Timestamp LRU<K, V>::_least_recent_timestamp() const {
    assert(!m_pool.empty());

    const Timestamp* p_min_ts = &(m_pool.begin()->second.second);
    for (const auto& kv : m_pool) {
        if (kv.second.second < *p_min_ts) {
            p_min_ts = &kv.second.second;
        }
    }

    return *p_min_ts;
}

} // cppgear
