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
        size_t c = 0;
        for (auto&& i : l) {
            data[c++] = std::move(i);
            if (c == len) break;
        }
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

    T& at(size_t i) {
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

template<typename T, size_t N>
inline auto operator==(const array<T,N>& a1, const array<T,N>& a2) {
    return (a1 <=> a2) == 0;
}

}
