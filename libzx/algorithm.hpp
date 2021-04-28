#pragma once
#include <functional>
#include "slice.hpp"

namespace libzx {

template<comparable T>
void sort(slice<T> s) {
    if (s.size() == 0) return;
    auto p = &s.front(), q = &s.back();
    auto&& key = s[s.size() / 2];
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

template<typename T>
void sort(slice<T> s, std::function<bool(T&,T&)> less) {
    if (s.size() == 0) return;
    auto p = &s.front(), q = &s.back();
    auto&& key = s[s.size() / 2];
    while (p <= q) {
        while (less(*p, key)) p++;
        while (less(key, *q)) q--;
        if (p <= q) {
            std::swap(*p, *q);
            p++, q--;
        }
    }
    if (q > &s.front()) sort(slice<T>(s.begin(), q+1), less);
    if (p < &s.back()) sort(slice<T>(p, s.end()), less);
}

}
