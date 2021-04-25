#pragma once
#include <memory>
#include <functional>
#include "deque.hpp"

#define lambda(x) [](auto&& x)

namespace libzx {

template<typename T>
class stream {
public:
    struct pair { T value; bool exist; };
    deque<pair> queue;
    size_t exist = 0;
    stream() = default;
    stream(streamable auto&& s) : queue(s.size()), exist(s.size()) {
        for (auto&& i : s) {
            queue.push_back(pair{i, true});
        }
    }

    auto& for_each(std::function<void(T&)> fn) {
        for (auto&& [v, e] : queue) {
            if (e) fn(v);
        }
        return *this;
    }

    auto& filter(std::function<bool(T&)> fn) {
        for (auto&& [v, e] : queue) {
            if (e) {
                if (!(e = fn(v))) exist--;
            }
        }
        return *this;
    }

    auto& map(std::function<T(T&)> fn) {
        for (auto&& [v, e] : queue) {
            if (e) v = fn(v);
        }
        return *this;
    }

    template<typename R>
    auto map(std::function<R(T&)> fn) {
        stream<R> new_stream;
        for (auto&& [v, e] : queue) {
            if (e) new_stream.queue.push_back(typename stream<R>::pair{fn(v), true}), new_stream.exist++;
        }
        return new_stream;
    }

    template<container C>
    auto collect() {
        C c(exist);
        size_t i = 0;
        for (auto&& [v, e] : queue) {
            if (e) c[i++] = std::move(v);
        }
        return c;
    }
};

}