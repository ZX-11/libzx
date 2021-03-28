#pragma once
#include <cstdint>
#include "concepts.hpp"

namespace libzx {

class range {
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
    range(iterable auto&& i) :
        start(0), stop(i.end() - i.begin()), step(1) {}
    template<typename T, size_t N>
    range(T (&a)[N]) : start(0), stop(N), step(1) {}
    auto begin() { return iter{ start, step }; }
    auto end() { return iter{ stop, step }; }
};

}