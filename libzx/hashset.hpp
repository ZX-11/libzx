#pragma once

#include "hash.hpp"
#include "range.hpp"
#include "concepts.hpp"
#include "smart_array.hpp"

namespace libzx {

template<hashable T>
class hashset {
    unique_array<T> data;
    unique_array<bool> occupied;
    size_t len = 0;
    size_t cap() { return data.size(); }
    double payload() { return (double)len / (double)data.size(); }

    void grow() {
        auto new_cap = data.size() * 2 + 1;
        auto new_data = unique_array<T>(new_cap);
        auto new_occupied = unique_array<bool>(new_cap);
        for (auto i : range(data)) {
            if (!occupied[i]) continue;
            for (size_t j = hash(data[i]) % new_cap, k = 0; k < new_cap; j = (j+1) % new_cap, k++) {
                if (new_occupied[j]) continue;
                new_data[j] = std::move(data[i]);
                new_occupied[j] = true;
                break;
            }
        }
        data = std::move(new_data);
        occupied = std::move(new_occupied);
    }

    auto find(const T& element) {
        for (size_t i = hash(element) % cap(), j = 0; j < cap(); i = (i+1)%cap(), j++) {
            if (occupied[i] && data[i] == element) {
                return &data[i];
            } else if (!occupied[i]) {
                break;
            }
        }
        return (T*)nullptr;
    }

    size_t first() {
        for (auto i : range(data)) {
            if (occupied[i]) return i;
        }
        return data.size();
    }
public:
    hashset(size_t min_cap = 16) : data(min_cap), occupied(min_cap) {}
    hashset(std::initializer_list<T> l) :
        data(l.size() + l.size() / 2) , occupied(l.size() + l.size() / 2) {
        for (auto&& i : l) put(std::move(i));
    }

    auto& put(convertible_to<T> auto&& element) {
        if (cap() == 0 || payload() > 0.7) grow();
        for (size_t i = hash(element) % cap(), j = 0; j < cap(); i = (i+1) % cap(), j++) {
            if (!occupied[i]) {
                data[i] = std::forward<decltype(element)>(element);
                occupied[i] = true;
                len++;
                break;
            } else if (data[i] == element) {
                break;
            }
        }
        return *this;
    }

    bool contains(const T& element) {
        return find(element) != nullptr;
    }

    auto remove(const T& element) {
        if (auto t = find(element); t != nullptr) {
            occupied[t - data.begin()] = false;
            len--;
            return true;
        }
        return false;
    }

    struct iter {
        hashset* set;
        size_t index;
        auto& operator++() {
            do {
                index++;
            } while (index < set->data.size() && !set->occupied[index]);
            return *this;
        }
        auto operator!=(iter& i) { return index != i.index; }
        auto& operator*() { return set->data[index]; }
    };

    auto begin() { return iter{ this, first() }; }
    auto end() { return iter{ this, data.size() }; }
};

}
