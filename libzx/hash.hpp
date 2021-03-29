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
    auto t = static_cast<size_t>(i);
    return t * 2654435761;
}

}
