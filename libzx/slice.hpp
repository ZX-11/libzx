#pragma once
#include <stdexcept>
#include <string>

namespace libzx {

template<typename T>
concept iterable = requires(T t) { t.begin(); t.end(); t[0]; };

template<typename T>
concept iterator = requires(T a, T b) { a++; *a; b - a; };

// slice is a reference to a continuous space;
// it is undefined to refer to discontinuous spaces;
// it is totally safe, except when vector grows;
template<typename T>
class slice {
    T *const data;
    const size_t len = 0;
    slice(T* data, size_t len) : data(data), len(len) {}
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

    auto sub(size_t begin, size_t end) {
        return slice<T>(&at(begin), std::min(end,len)-begin);
    }

    auto sub(size_t begin) {
        return slice<T>(&at(begin), len-begin);
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

template<typename T>
concept comparable = requires(T t) { t <=> t; };

template<comparable T>
void sort(slice<T> s) {
    if (s.size() == 0) return;
    auto p = &s.front(), q = &s.back();
    auto&& key = s.front();
    while (p <= q) {
        while (*p < key) p++;
        while (*q > key) q--;
        if (p <= q) {
            std::swap(*p, *q);
            p++, q--;
        }
    }
    if (q > &s.front()) sort(slice<T>(s.begin(), q+1));
    if (p < &s.back()) sort(slice<T>(p, s.end()));
}

}
