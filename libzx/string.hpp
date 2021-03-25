#pragma once
#include "vector.hpp"
#include <cstring>
#include <iostream>

namespace libzx {

class string : public vector<char> {
public:
    string() :       vector()  { data[0] = 0; }
    string(char c) : vector{c} { data[1] = 0; }
    string(const char* s) :   vector(strlen(s)) { strcpy(begin(), s); }
    string(const string& s) : vector(s.size())  { strcpy(begin(), s.begin()); puts("copy"); }
    string(string&& s) :      vector(s.size())  { data = std::move(s.data); puts("move"); }
    string(const slice<char>& s) : vector(s.size()) { strncpy(begin(), s.begin(), s.size()); }

    auto& operator=(const string& s) {
        return vector::operator=(s);
    }
    auto& operator=(string&& s) noexcept {
        return vector::operator=(std::move(s));
    }

    auto& push_back(char c) {
        vector::push_back(c);
        *end() = 0;
        return *this;
    }

    auto& operator+=(const string& s) { 
        if (cap() - len <= s.size()) grow(s.size());
        strcpy(end(), s.begin());
        len += s.size();
        return *this;
    }

    auto& operator+=(const char* s) {
        size_t s_len = strlen(s);
        if (cap() - len <= s_len) grow(s_len);
        strcpy(end(), s);
        len += s_len;
        return *this;
    }

    auto& operator+=(const slice<char>& s) {
        if (cap() - len <= s.size()) grow(s.size());
        strncpy(end(), s.begin(), s.size());
        len += s.size();
        return *this;
    }

    auto& operator+=(char c) { return push_back(c); }

    auto operator<=>(const string& s) const noexcept {
        if (len != s.len) return len <=> s.len;
        return strcmp(begin(), s.begin()) <=> 0;
    }

    auto operator<=>(const char* s) const noexcept {
        return strcmp(begin(), s) <=> 0;
    }

    const char* c_str() const noexcept { return data.get(); }

    auto std_str() const noexcept { return std::string(data.get()); }

    static auto getline(std::istream& in = std::cin) {
        string s;
        for (; in.peek() != '\n' && in.peek() != EOF; in.get()) {
            s.push_back(in.peek());
        }
        if (in.peek() == '\n') in.get();
        return s;
    }

    friend auto& getline(std::istream& in, string& s);
};

template<typename T>
concept stringable = requires(T t) { string(t); };

template<typename T>
concept string_joinable = requires(string s, T t) { s += t; };

inline auto operator+(stringable auto&& s1, string_joinable auto&& s2) {
    string s(std::forward<decltype(s1)>(s1));
    return std::move(s += s2);
}

template<typename T>
concept string_comparable = requires(string s, T t) { s <=> t; };

inline auto operator==(string_comparable auto&& s1, string_comparable auto&& s2) noexcept {
    return (s1 <=> s2) == 0;
}

inline auto& operator<<(std::ostream& out, const string& s) {
    return out << s.c_str();
}

inline auto& operator>>(std::istream& in, string& s) {
    while (isspace(in.peek())) in.get();
    for (; !isspace(in.peek()) && in.peek() != EOF; in.get()) {
        s.push_back(in.peek());
    }
    return in;
}

inline auto& getline(std::istream& in, string& s) {
    s[0] = 0; s.len = 0;
    for (; in.peek() != '\n' && in.peek() != EOF; in.get()) {
        s.push_back(in.peek());
    }
    if (in.peek() == '\n') in.get();
    return s;
}

}