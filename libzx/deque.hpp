#pragma once
#include "slice.hpp"
#include "smart_array.hpp"

namespace libzx {

template<typename T>
class deque {
protected:
    unique_array<T> data;
    size_t len = 0, first = 0, last = 0;
    size_t cap() { return data.size(); }

    void grow(size_t size = 2) {
        auto new_cap = std::bit_ceil(data.size() + size);
        auto new_data = unique_array<T>(new_cap);
        std::move(data.begin(), data.begin() + last, new_data.begin());
        if (first > last) {
            std::move(data.begin() + first, data.end(), new_data.end() - data.size() + first);
            first = new_data.size() - data.size() + first;
        }
        data = std::move(new_data);
    }

public:
    deque(size_t len = 0, size_t min_cap = 16) : data(std::bit_ceil(std::max(len, min_cap))), len(len) { }
    deque(std::initializer_list<T> l) : data(std::bit_ceil(l.size())) {
        std::move(l.begin(), l.end(), data.get());
        first = 0, last = l.size();
    }
    deque(const slice<T>& s) : data(std::bit_ceil(s.size())), len(s.size()) {
        std::copy(s.begin(), s.end(), data.get());
        first = 0, last = s.size();
    }
    deque(const deque& v) : data(v.data.clone()), len(v.len) { }
    deque(deque&& v) : data(std::move(v.data)), len(v.len) { v.len = 0; }

    auto& operator=(const deque& v) {
        if (this != &v) {
            data = std::move(v.data.clone());
            len = v.len;
        }
        return *this;
    }

    auto& operator=(deque&& v) noexcept {
        if (this != &v) {
            data = std::move(v.data);
            len = v.len;
            v.len = 0;
        }
        return *this;
    }

    auto& push_back(convertible_to<T> auto&& t) {
        if (len >= cap() - 1 || cap() == 0) grow();
        data[last] = std::forward<decltype(t)>(t);
        last = (last + 1) % data.size();
        len++;
        return *this;
    }

    auto& emplace_back(auto&&... a) { return push_back(T(a...)); }

    auto& push_front(convertible_to<T> auto&& t) {
        if (len >= cap() - 1 || cap() == 0) grow();
        first = (first == 0) ? data.size()-1 : first-1;
        data[first] = std::forward<decltype(t)>(t);
        len++;
        return *this;
    }

    auto& emplace_front(auto&&... a) { return push_front(T(a...)); }

    T pop_back() {
        if (len == 0) return T();
        T& pop = back();
        last = (last == 0) ? data.size()-1 : last-1;
        len--;
        return std::move(pop);
    }

    T pop_front() {
        if (len == 0) return T();
        T& pop = front();
        first = (first + 1) % data.size();
        len--;
        return std::move(pop);
    }

    T& operator[](size_t i) noexcept { return data[(first + i) % data.size()]; }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("deque: index (which is " + std::to_string(i) +
                 ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return data[(first + i) % data.size()];
    }

    size_t size() const noexcept { return len; }
    T& front() { return data[first]; }
    T& back() { return data[last-1]; }

    friend class iter;
    struct iter {
        deque *const d;
        T* current;
        auto& operator++() {
            if (current == &d->data.back()) {
                current = &d->data.front();
            } else {
                current++;
            }
            return *this;
        }
        bool  operator==(iter& i) { return current == i.current; }
        auto& operator*() { return *current; }
    };

    auto begin() { return iter{ this, &data[first] }; }
    auto end() { return iter{ this, &data[last] }; }   
};

}
