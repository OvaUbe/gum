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

#include <cppgear/options.h>

#include <cassert>
#include <iostream>

namespace cppgear {

Options::Options(int argc, char **argv)
    : m_unix_options()
    , m_gnu_options() {

    optional<char> unix_flag;
    optional<std::string> gnu_flag;
    optional<std::string> tail;

    int pos = 1;
    if (!_scroll(unix_flag, gnu_flag, tail, pos, argc, argv)) {
        return;
    }

    ArgList current_arg_list;
    if (!_handle_tail(unix_flag, gnu_flag, tail, pos, argc, argv, current_arg_list)) {
        return;
    }

    while (pos < argc) {
        char* arg = argv[pos];
        ++pos;

        bool is_gnu  = _is_gnu_flag(arg);
        bool is_unix = _is_unix_flag(arg);

        if (is_gnu || is_unix) {
            _stash(unix_flag, gnu_flag, current_arg_list);

            if (is_unix) {
                unix_flag = _to_unix_flag(arg, tail);
            } else if (is_gnu) {
                gnu_flag = _to_gnu_flag(arg, tail);
            } else {
                assert(false);
            }

            if (!_handle_tail(unix_flag, gnu_flag, tail, pos, argc, argv, current_arg_list)) {
                return;
            }
        } else {
            current_arg_list.push_back(std::string(arg));
        }
    }

    _stash(unix_flag, gnu_flag, current_arg_list);
}

bool Options::contains(char unix_flag) const {
    return (m_unix_options.end() != m_unix_options.find(unix_flag));
}

bool Options::contains(std::string const& gnu_flag) const {
    return (m_gnu_options.end() != m_gnu_options.find(gnu_flag));
}

Options::ArgList const* Options::get(char unix_flag) const {
    auto const iter = m_unix_options.find(unix_flag);
    return (m_unix_options.end() != iter) ? &(iter->second) : nullptr;
}

Options::ArgList const* Options::get(std::string const& gnu_flag) const {
    auto const iter = m_gnu_options.find(gnu_flag);
    return (m_gnu_options.end() != iter) ? &(iter->second) : nullptr;
}

optional<Options::ArgList> Options::remove(char unix_flag) {
    auto const iter = m_unix_options.find(unix_flag);
    if (m_unix_options.end() == iter) {
        return std::experimental::nullopt;
    }
    ArgList result = std::move(iter->second);
    m_unix_options.erase(iter);

    return std::move(result);
}

optional<Options::ArgList> Options::remove(std::string const& gnu_flag) {
    auto const iter = m_gnu_options.find(gnu_flag);
    if (m_gnu_options.end() == iter) {
        return std::experimental::nullopt;
    }
    ArgList result = std::move(iter->second);
    m_gnu_options.erase(iter);

    return std::move(result);
}

bool Options::_scroll(optional<char>& unix_flag,
                      optional<std::string>& gnu_flag,
                      optional<std::string>& tail,
                      int& pos, int argc, char** argv) const {

    while (pos < argc) {
        char* arg = argv[pos];
        ++pos;

        if (_is_unix_flag(arg)) {
            unix_flag = _to_unix_flag(arg, tail);
            return true;
        }
        if (_is_gnu_flag(arg)) {
            gnu_flag = _to_gnu_flag(arg, tail);
            return true;
        }
    }
    return false;
}

bool Options::_handle_tail(optional<char>& unix_flag,
                  optional<std::string>& gnu_flag,
                  optional<std::string>& tail,
                  int& pos, int argc, char** argv,
                  ArgList& current_arg_list) {

    if (!tail) {
        return true;
    }

    current_arg_list.push_back(std::move(tail.value()));
    tail = std::experimental::nullopt;

    _stash(unix_flag, gnu_flag, current_arg_list);

    if (!_scroll(unix_flag, gnu_flag, tail, pos, argc, argv)) {
        return false;
    }

    return _handle_tail(unix_flag, gnu_flag, tail, pos, argc, argv, current_arg_list);
}

void Options::_stash(optional<char>& unix_flag,
                     optional<std::string>& gnu_flag,
                     ArgList& current_arg_list) {
    if (unix_flag) {
        assert(!gnu_flag);
        _merge_emplace(unix_flag.value(), std::move(current_arg_list));
        unix_flag = std::experimental::nullopt;
    } else if (gnu_flag) {
        assert(!unix_flag);
        _merge_emplace(std::move(gnu_flag.value()), std::move(current_arg_list));
        gnu_flag = std::experimental::nullopt;
    } else {
        assert(false);
    }

    current_arg_list = ArgList();
}

void Options::_merge_emplace(char flag, ArgList arg_list) {
    auto iter = m_unix_options.find(flag);

    if (m_unix_options.end() != iter) {
        auto& old_args = iter->second;
        arg_list.reserve(arg_list.size() + old_args.size());

        for (auto& arg : old_args) {
            arg_list.push_back(std::move(arg));
        }
        m_unix_options.erase(iter);
    }

    m_unix_options.emplace(flag, std::move(arg_list));
}

void Options::_merge_emplace(std::string flag, ArgList arg_list) {
    auto iter = m_gnu_options.find(flag);

    if (m_gnu_options.end() != iter) {
        auto& old_args = iter->second;
        arg_list.reserve(arg_list.size() + old_args.size());

        for (auto& arg : old_args) {
            arg_list.push_back(std::move(arg));
        }
        m_gnu_options.erase(iter);
    }

    m_gnu_options.emplace(flag, std::move(arg_list));
}

bool Options::_is_unix_flag(char const* arg) const {
    if ('-' != arg[0]) {
        return false;
    }
    if ('-' == arg[1]) {
        return false;
    }
    if ('\0' == arg[1]) {
        return false;
    }
    return true;
}

bool Options::_is_gnu_flag(char const* arg) const {
    if ('-' != arg[0]) {
        return false;
    }
    if ('-' != arg[1]) {
        return false;
    }
    if ('\0' == arg[2]) {
        return false;
    }
    return true;
}

char Options::_to_unix_flag(char const* arg, optional<std::string>& tail) const {
    char flag = arg[1];
    if ('\0' != arg[2]) {
        tail = std::string(&arg[2]);
    } else {
        tail = std::experimental::nullopt;
    }
    return flag;
}

std::string Options::_to_gnu_flag(char const* arg, optional<std::string>& tail) const {
    std::string flag;

    char const delimiter = '=';
    size_t delimiter_pos = 0;

    /* Ignore first symbol */
    size_t i = 3;
    char c;
    do {
        c = arg[i];
        if (delimiter == c) {
            delimiter_pos = i;
            break;
        }
        ++i;
    } while ('\0' != c);

    bool no_tail = !delimiter_pos || '\0' == arg[delimiter_pos + 1];
    if (no_tail) {
        flag = std::string(&arg[2]);
        tail = std::experimental::nullopt;
    } else {
        std::copy(&arg[2], &arg[delimiter_pos], std::back_inserter(flag));
        tail = std::string(&arg[delimiter_pos + 1]);
    }

    return flag;
}

} // cppgear
