#pragma once
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

    void grow(size_t size = 1) {
        auto new_cap = std::max(data.size() * 2, data.size() + size);
        auto new_data = unique_array<T>(new_cap);
        for (size_t i = 0; i < data.size(); i++) {
            new_data[i] = std::move(data[i]);
        }
        data = std::move(new_data);
    }
public:
    vector() : data(16) {}
    vector(size_t len, size_t min_cap = 16) : data(std::max(len + len / 3, min_cap)), len(len) {}
    vector(std::initializer_list<T> l) : data(l.size() + l.size() / 3) {
        for (auto&& i : l) data[len++] = std::move(i);
    }
    vector(const slice<T>& s) : data(s.size() + s.size() / 3), len(s.size()) {
        std::copy(s.begin(), s.end(), data.get());
    }
    vector(const vector& v) : data(v.data.clone()), len(v.len) { }
    vector(vector&& v) : data(v.data), len(v.len) { }

    auto& operator=(const vector& v) {
        data = std::move(v.data.clone());
        len = v.len;
        return *this;
    }

    auto& operator=(vector&& v) noexcept {
        data = std::move(v.data);
        len = v.len;
        return *this;
    }

    auto& push_back(const T& t) {
        if (len == cap()) grow();
        data[len++] = t;
        return *this;
    }

    auto& push_back(T&& t) {
        if (len == cap()) grow();
        data[len++] = std::move(t);
        return *this;
    }

    T& pop_back() noexcept { return len > 0 ? data[--len] : data[0]; }

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