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

#include <cppgear/types.h>

#include <vector>
#include <unordered_map>

namespace cppgear {

class Options {
    /* Inner types */
public:
    using ArgList = std::vector<std::string>;

private:
    template <typename T>
    using optional = std::experimental::optional<T>;

    /* Methods */
public:
    Options(int argc, char** argv);

    Options(Options const& copy) = default;
    Options(Options&& move) = default;
    ~Options() = default;

    Options& operator=(Options const& copy) = default;
    Options& operator=(Options&& move) = default;

    bool contains(char unix_flag) const;
    bool contains(std::string const& gnu_flag) const;

    ArgList const* get(char unix_flag) const;
    ArgList const* get(std::string const& gnu_flag) const;

    std::experimental::optional<ArgList> remove(char unix_flag);
    std::experimental::optional<ArgList> remove(std::string const& gnu_flag);

private:
    bool _scroll(optional<char>& unix_flag,
                 optional<std::string>& gnu_flag,
                 optional<std::string>& tail,
                 int& pos, int argc, char** argv) const;

    bool _handle_tail(optional<char>& unix_flag,
                      optional<std::string>& gnu_flag,
                      optional<std::string>& tail,
                      int& pos, int argc, char** argv,
                      ArgList& current_arg_list);

    void _stash(optional<char>& unix_flag,
                optional<std::string>& gnu_flag,
                ArgList& current_arg_list);

    void _merge_emplace(char flag, ArgList arg_list);
    void _merge_emplace(std::string flag, ArgList arg_list);

    bool _is_unix_flag(char const* arg) const;
    bool _is_gnu_flag(char const* arg) const;

    char _to_unix_flag(char const* arg, optional<std::string>& tail) const;
    std::string _to_gnu_flag(char const* arg, optional<std::string>& tail) const;

    /* Fields */
private:
    std::unordered_map<char, ArgList> m_unix_options;
    std::unordered_map<std::string, ArgList> m_gnu_options;

};

} // cppgear
