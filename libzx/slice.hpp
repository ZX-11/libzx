#pragma once
#include <stdexcept>
#include <string>

namespace libzx {

template<typename T>
concept iterable = requires(T t) { t.begin(); t.end(); t[0]; };

template<typename T>
concept iterator = requires(T a, T b) { a++; b - a; };

// slice is a reference to a continuous space;
// it is undefined to refer to discontinuous spaces;
// it is totally safe, except when vector grows;
template<typename T>
class slice {
    T *const data;
    const size_t len = 0;
public:
    template<size_t N>
    slice(T (&data)[N]) : data(data), len(N) {}

    slice(iterable auto&& data) : data(&(*data.begin())), len(data.end() - data.begin()) {}
    slice(iterator auto begin, iterator auto end) : data(&(*begin)), len(end - begin) {}

    T& operator[](size_t i) noexcept { return data[i]; }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("slice: index (which is " + std::to_string(i) +
                 ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[i];
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

    size_t size() const noexcept { return len; }
    T& front() noexcept { return data[0]; }
    T& back() noexcept { return data[len-1]; }
    T* begin() const noexcept { return data; }
    T* end() const noexcept { return data + len; }
};

template<typename T>
inline auto operator==(const slice<T>& s1, const slice<T>& s2) {
    return (s1 <=> s2) == 0;
}

}