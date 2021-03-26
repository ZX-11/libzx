#pragma once
#include <stdexcept>
#include <initializer_list>
#include "slice.hpp"

namespace libzx {

template<typename T, size_t N>
class array {
    T data[N];
    const size_t len = N;
public:
    array() = default;
    array(std::initializer_list<T> l) {
        std::move(l.begin(), l.begin() + std::min(l.size(), len), data);
    }
    array(slice<T> s) {
        std::copy(s.begin(), s.begin() + std::min(s.size(), len), data);
    }

    auto operator<=>(const array& a) const {
        for (size_t i = 0; i < N-1; i++) {
            if (data[i] != a.data[i]) {
                return data[i] <=> a.data[i];
            }
        }
        return data[N-1] <=> a.data[N-1];
    }

    auto operator==(const array& a) const {
        return (*this <=> a) == 0;
    }

    T &at(size_t i) {
        if (i >= len)
            throw std::out_of_range("array: index (which is " + std::to_string(i) +
                ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[i];
    }

    T& operator[](size_t i) { return data[i]; }

    size_t size() const noexcept { return len; }
    T& front() noexcept { return data[0]; }
    T& back() noexcept { return data[len-1]; }
    T* begin() const noexcept { return &data[0]; }
    T* end() const noexcept { return &data[len]; }
};

}
