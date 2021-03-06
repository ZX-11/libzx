#pragma once
#include <memory>
#include <string>
#include <cstring>
#include <stdexcept>
#include <initializer_list>
#include "slice.hpp"

namespace libzx {

template<typename T>
class unique_array : public std::unique_ptr<T[]> {
protected:
    using data = std::unique_ptr<T[]>;
    size_t len = 0;
public:
    unique_array() = default;
    unique_array(size_t size) : data(new T[size]()), len(size) { }
    unique_array(unique_array&& a) : data(a.release()), len(a.len) { a.len = 0; }
    unique_array(std::initializer_list<T> l) : data(new T[l.size()]()), len(l.size()) {
        std::move(l.begin(), l.end(), data::get());
    }
    unique_array(slice<T> s) : data(new T[s.size()]()), len(s.size()) {
        std::copy(s.begin(), s.end(), data::get());
    }

    auto& operator=(unique_array&& a) noexcept {
        if (this != &a) {
            data::reset(a.release());
            len = a.len;
            a.len = 0;
        }
        return *this;
    }

    auto clone() const {
        unique_array n(len);
        std::copy(data::get(), data::get()+len, n.get());
        return n;
    }
    
    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("unique_array: index (which is " + std::to_string(i) +
                ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return (*this)[i];
    }

    size_t size() const noexcept { return len; }
    T& front() noexcept { return (*this)[0]; }
    T& back() noexcept { return (*this)[len-1]; }
    T* begin() const noexcept { return data::get(); }
    T* end() const noexcept { return data::get() + len; }
};

template<typename T>
class shared_array : public std::shared_ptr<T[]> {
protected:
    using data = std::shared_ptr<T[]>;
    size_t len = 0;
public:
    shared_array() = default;
    shared_array(size_t size) : data(new T[size]()), len(size) { }
    shared_array(std::initializer_list<T> l) : data(new T[l.size()]()), len(l.size()) {
        for (auto&& i : l) (*this)[len++] = std::move(i);
    }
    shared_array(slice<T> s) : data(new T[s.size()]()), len(s.size()) {
        std::copy(s.begin(), s.end(), data::get());
    }

    auto clone() const {
        shared_array n(len);
        std::copy(data::get(), data::get()+len, n.get());
        return n;
    }

    T& at(size_t i) {
        if (i >= len)
            throw std::out_of_range("shared_array: index (which is " + std::to_string(i) +
                ") >= this->size() (which is " + std::to_string(len) + ")");
        else
            return (*this)[i];
    }

    size_t size() const noexcept { return len; }
    T& front() noexcept { return (*this)[0]; }
    T& back() noexcept { return (*this)[len-1]; }
    T* begin() const noexcept { return data::get(); }
    T* end() const noexcept { return data::get() + len; }
};

}
