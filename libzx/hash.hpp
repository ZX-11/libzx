#pragma once
#include "concepts.hpp"
#include "string.hpp"

namespace libzx {

inline size_t hash(const char* s) {
    size_t seed = 1313131, hash = 0;
    while (*s) hash = hash * seed + (*s++);
    return hash;
}

inline size_t hash(const string& s) {
    return hash(s.c_str());
}

inline size_t hash(integral auto i) {
    return static_cast<size_t>(i) * 2654435761;
}

inline size_t hash(float i) {
    return hash(*reinterpret_cast<uint32_t*>(&i));
}

inline size_t hash(double i) {
    return hash(*reinterpret_cast<uint64_t*>(&i));
}

template<typename T>
concept hashable = requires(T t) { { hash(t) } -> convertible_to<size_t>; t == t; };

static size_t bit_floor(size_t n) {
    n -= 1;
    n |= n >> 1; 
    n |= n >> 2; 
    n |= n >> 4; 
    n |= n >> 8; 
    n |= n >> 16;
    n |= n >> 32;
    return n+1;
}

}
