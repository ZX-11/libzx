#pragma once
#include <string>
#include <stdexcept>
#include "concepts.hpp"

namespace libzx {

// slice is a reference to a continuous space;
// it is undefined to refer to discontinuous spaces;
// it is totally safe, except when vector grows;
template<typename T>
class slice {
protected:
    T* data = nullptr;
    size_t len = 0;
    slice(T* data, size_t len) : data(data), len(len) {}
public:
    slice() = default;
    template<size_t N>
    slice(T (&data)[N], size_t begin = 0, size_t end = SIZE_MAX) :
        data(&data[begin]),
        len(std::min(end, N) - begin) {
            if constexpr (std::is_same_v<T, const char>) len -= 1;
        }
    slice(sliceable auto&& data, size_t begin = 0, size_t end = SIZE_MAX) :
        data(&(*data.begin()) + begin),
        len(std::min(end, static_cast<size_t>(data.end() - data.begin())) - begin) {}
    slice(continuous_iterator auto begin, continuous_iterator auto end) :
        data(&(*begin)), len(end - begin) {}
    T& operator[](size_t i) noexcept { return data[i]; }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("slice: index (which is " + std::to_string(i) +
                 ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[i];
    }

    auto sub(size_t begin, size_t end = SIZE_MAX) {
        if (begin > len) at(begin);
        return slice<T>(data + begin, std::min(end, len) - begin);
    }

    auto operator<=>(const slice& s) const {
        if (len != s.len) return len <=> s.len;
        for (size_t i = 0; i < len-1; i++) {
            if (data[i] != s.data[i]) {
                return data[i] <=> s.data[i];
            }
        }
        return data[len-1] <=> s.data[len-1];
    }

    auto operator==(const slice& s) const {
        return (*this <=> s) == 0;
    }

    size_t size() const noexcept { return len; }
    T& front() noexcept { return data[0]; }
    T& back() noexcept { return data[len-1]; }
    T* begin() const noexcept { return data; }
    T* end() const noexcept { return data + len; }
};

template<size_t N>
inline auto operator<=>(slice<char>& s1, const char (&s2)[N]) {
    if (s1.size() != N-1) return s1.size() <=> N-1;
    for (size_t i = 0; i < N-2; i++) {
        if (s1[i] != s2[i]) {
            return s1[i] <=> s2[i];
        }
    }
    return s1.back() <=> s2[N-2];
}

template<size_t N>
inline auto operator==(slice<char>& s1, const char (&s2)[N]) {
    return (s1 <=> s2) == 0;
}

}
