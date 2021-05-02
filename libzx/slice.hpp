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
    T *data;
    size_t len = 0;
    slice(T* data, size_t len) : data(data), len(len) {}
public:
    template<size_t N>
    slice(T (&data)[N]) : data(data), len(N) {}

    slice(sliceable auto&& data) : data(&(*data.begin())), len(data.end() - data.begin()) {}
    slice(continuous_iterator auto begin, continuous_iterator auto end) : data(&(*begin)), len(end - begin) {}

    T& operator[](size_t i) noexcept { return data[i]; }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("slice: index (which is " + std::to_string(i) +
                 ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[i];
    }

    auto sub(size_t begin, size_t end) {
        if (begin > len) at(begin);
        return slice<T>(data + begin, std::min(end,len)-begin);
    }

    auto sub(size_t begin) {
         if (begin > len) at(begin);
        return slice<T>(data + begin, len-begin);
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

}
