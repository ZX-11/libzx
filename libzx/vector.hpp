#pragma once
#include <bit>
#include <optional>
#include <stdexcept>
#include <initializer_list>
#include "smart_array.hpp"
#include "slice.hpp"

namespace libzx {

template<typename T>
class vector {
protected:
    unique_array<T> data;
    size_t len = 0;
    size_t cap() { return data.size(); }

    void grow(size_t size = 2) {
        auto new_cap = std::bit_ceil(data.size() + size);
        auto new_data = unique_array<T>(new_cap);
        std::move(data.begin(), data.end(), new_data.begin());
        data = std::move(new_data);
    }
public:
    vector(size_t len = 0, size_t min_cap = 16) : data(std::max(std::bit_ceil(len+1), min_cap)), len(len) {}
    vector(std::initializer_list<T> l) : data(std::max(std::bit_ceil(l.size()+1), (size_t)16)), len(l.size()) {
        std::move(l.begin(), l.end(), data.get());
    }
    vector(const slice<T>& s) : data(std::max(std::bit_ceil(s.size()+1), (size_t)16)), len(s.size()) {
        std::copy(s.begin(), s.end(), data.get());
    }
    vector(const vector& v) : data(v.data.clone()), len(v.len) { }
    vector(vector&& v) : data(std::move(v.data)), len(v.len) { v.len = 0; }

    auto& operator=(const vector& v) {
        if (this != &v) {
            data = v.data.clone();
            len = v.len;
        }
        return *this;
    }

    auto& operator=(vector&& v) noexcept {
        if (this != &v) {
            data = std::move(v.data);
            len = v.len;
            v.len = 0;
        }
        return *this;
    }

    auto& push_back(convertible_to<T> auto&& t) {
        if (len == cap()) grow();
        data[len++] = std::forward<decltype(t)>(t);
        return *this;
    }

    auto& emplace_back(auto&&... a) { return push_back(T(a...)); }

    T pop_back() { return len > 0 ? std::move(data[--len]) : at(-1); }

    T& operator[](size_t i) noexcept { return data[i]; }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("vector: index (which is " + std::to_string(i) +
                 ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[i];
    }

    size_t size() const noexcept { return len; }
    T& front() { return data[0]; }
    T& back() { return data[len-1]; }
    T* begin() const noexcept { return data.get(); }
    T* end() const noexcept { return data.get() + len; }
};

}
