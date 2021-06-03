#pragma once
#include <initializer_list>
#include "hash.hpp"
#include "range.hpp"
#include "concepts.hpp"
#include "smart_array.hpp"

namespace libzx {

template<hashable T>
class hashset {
protected:
    struct status { uint8_t occupied : 1, conflict : 1; };

    unique_array<T> data;
    unique_array<status> state;
    size_t len = 0;
    size_t cap() { return data.size(); }
    double payload() { return (double)len / (double)data.size(); }

    void grow() {
        auto new_cap = data.size() * 2 + 1;
        auto new_data = unique_array<T>(new_cap);
        auto new_state = unique_array<status>(new_cap);
        for (auto i : urange(data)) {
            if (!state[i].occupied) continue;
            for (size_t j = hash(data[i]) % new_cap, k = 0; k < new_cap; j = (j+1) % new_cap, k++) {
                if (new_state[j].occupied) {
                    new_state[j].conflict = true;
                    continue;
                }
                new_data[j] = std::move(data[i]);
                new_state[j].occupied = true;
                break;
            }
        }
        data = std::move(new_data);
        state = std::move(new_state);
    }

    auto find(const T& key) -> T* {
        for (size_t i = hash(key) % cap(), j = 0; j < cap(); i = (i+1) % cap(), j++) {
            if (state[i].occupied && data[i] == key) {
                return &data[i];
            }
            if (!state[i].conflict) {
                break;
            }
        }
        return nullptr;
    }

    size_t first() {
        for (auto i : urange(data)) {
            if (state[i].occupied) return i;
        }
        return data.size();
    }
public:
    hashset(size_t min_cap = 16) : data(min_cap), state(min_cap) {}
    hashset(std::initializer_list<T> l) :
        data(l.size() + l.size() / 2) , state(l.size() + l.size() / 2) {
        for (auto&& i : l) put(std::move(i));
    }

    auto& put(convertible_to<T> auto&& key) {
        if (cap() == 0 || payload() > 0.6) grow();
        for (size_t i = hash(key) % cap(), j = 0; j < cap(); i = (i+1) % cap(), j++) {
            if (!state[i].occupied) {
                data[i] = std::forward<decltype(key)>(key);
                state[i].occupied = true;
                len++;
                break;
            } else if (data[i] == key) {
                break;
            } else {
                state[i].conflict = true;
            }
        }
        return *this;
    }

    bool contains(const T& key) {
        return find(key) != nullptr;
    }

    auto remove(const T& key) {
        if (auto t = find(key); t != nullptr) {
            state[t - data.begin()].occupied = false;
            len--;
            return true;
        }
        return false;
    }

    struct iter {
        hashset* const set;
        size_t index;
        auto& operator++() {
            do {
                index++;
            } while (index < set->data.size() && !set->state[index].occupied);
            return *this;
        }
        auto operator!=(iter& i) { return index != i.index; }
        auto& operator*() { return set->data[index]; }
    };

    auto begin() { return iter{ this, first() }; }
    auto end() { return iter{ this, data.size() }; }
    auto size() { return len; }
};

}