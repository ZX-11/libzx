#pragma once
#include "smart_array.hpp"

namespace libzx {

template<typename T>
class deque {
protected:
    unique_array<T> data;
    size_t len = 0, begin = 0, end = 0;
    size_t cap() { return data.size(); }

    void grow(size_t size = 2) {
        auto new_cap = std::max(data.size() * 2, data.size() + size);
        auto new_data = unique_array<T>(new_cap);
        std::move(data.begin(), data.begin() + end, new_data.begin());
        if (begin > end) {
            std::move(data.begin() + begin, data.end(), new_data.end() - data.size() + begin);
            begin = new_data.size() - data.size() + begin;
        }
        data = std::move(new_data);
    }

public:
    deque(size_t min_cap = 16) : data(min_cap) { }
    deque(std::initializer_list<T> l) : data(l.size() + l.size()/3) {
        std::move(l.begin(), l.end(), data.get());
        begin = 0, end = l.size();
    }
    deque(const slice<T>& s) : data(s.size() + s.size()/3), len(s.size()) {
        std::copy(s.begin(), s.end(), data.get());
        begin = 0, end = s.size();
    }
    deque(const deque& v) : data(v.data.clone()), len(v.len) { }
    deque(deque&& v) : data(v.data), len(v.len) { v.len = 0; }

    auto& operator=(const deque& v) {
        data = std::move(v.data.clone());
        len = v.len;
        return *this;
    }

    auto& operator=(deque&& v) noexcept {
        data = std::move(v.data);
        len = v.len;
        v.len = 0;
        return *this;
    }

    auto& push_back(convertible_to<T> auto&& t) {
        if (len >= cap() - 1 || cap() == 0) grow();
        data[end] = std::forward<decltype(t)>(t);
        end = (end + 1) % data.size();
        len++;
        return *this;
    }

    auto& push_front(convertible_to<T> auto&& t) {
        if (len >= cap() - 1 || cap() == 0) grow();
        begin = (begin == 0) ? data.size()-1 : begin-1;
        data[begin] = std::forward<decltype(t)>(t);
        len++;
        return *this;
    }

    T pop_back() {
        if (len == 0) return T();
        T& pop = back();
        end = (end == 0) ? data.size()-1 : end-1;
        len--;
        return std::move(pop);
    }

    T pop_front() {
        if (len == 0) return T();
        T& pop = front();
        begin = (begin + 1) % data.size();
        len--;
        return std::move(pop);
    }

    T& operator[](size_t i) noexcept { return data[(begin + i) % data.size()]; }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("deque: index (which is " + std::to_string(i) +
                 ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[(begin + i) % data.size()];
    }

    size_t size() const noexcept { return len; }
    T& front() { return data[begin]; }
    T& back() { return data[end-1]; }
};

}
