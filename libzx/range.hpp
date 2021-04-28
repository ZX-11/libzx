#pragma once
#include <cstdint>
#include "concepts.hpp"

namespace libzx {

enum class direction { forward, backward };

class range {
protected:
    int64_t start, stop, step;
public:
    struct iter {
        int64_t value, step;
        auto& operator++() { value += step; return *this; }
        auto operator==(iter& i) { return value == i.value; }
        auto operator*() { return value; }
    };
    range(int64_t start, int64_t stop, int64_t step = 1) :
        start(start), stop(stop), step(step) {}
    range(indexable auto&& i, direction d = direction::forward) {
        if (d == direction::forward) {
            start = 0, stop = i.size(), step = 1;
        } else {
            start = i.size(), stop = -1, step = -1;
        }
    }
    template<typename T, size_t N>
    range(T (&a)[N], direction d = direction::forward) {
        if (d == direction::forward) {
            start = 0, stop = N, step = 1;
        } else {
            start = N, stop = -1, step = -1;
        }
    }
    auto begin() { return iter{ start, step }; }
    auto end() { return iter{ stop, step }; }
    auto size() { return (stop - start) / step; }
};

}