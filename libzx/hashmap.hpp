#pragma once
#include <optional>
#include <initializer_list>
#include "hash.hpp"
#include "range.hpp"
#include "concepts.hpp"
#include "smart_array.hpp"

namespace libzx {

template<hashable K, typename V>
class hashmap {
public:
    struct pair { K key; V value; };
protected:
    struct status { uint8_t occupied : 1, conflict : 1; };

    unique_array<pair> data;
    unique_array<status> state;
    size_t len = 0;
    size_t cap() { return data.size(); }
    double payload() { return (double)len / (double)data.size(); }

    void grow() {
        auto new_cap = data.size() * 2 + 1;
        auto new_data = unique_array<pair>(new_cap);
        auto new_state = unique_array<status>(new_cap);
        for (auto i : urange(data)) {
            if (!state[i].occupied) continue;
            for (size_t j = hash(data[i].key) % new_cap, k = 0; k < new_cap; j = (j+1) % new_cap, k++) {
                if (new_state[j].occupied) {
                    new_state[j].conflict = true;
                    continue;
                }
                new_data[j] = pair{
                    std::move(data[i].key),
                    std::move(data[i].value)
                };
                new_state[j].occupied = true;
                break;
            }
        }
        data = std::move(new_data);
        state = std::move(new_state);
    }

    auto find(const K& key) -> pair* {
        for (size_t i = hash(key) % cap(), j = 0; j < cap(); i = (i+1) % cap(), j++) {
            if (state[i].occupied && data[i].key == key) {
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
    hashmap(size_t min_cap = 16) : data(min_cap), state(min_cap) {}
    hashmap(std::initializer_list<pair> l) :
        data(l.size() + l.size() / 2) , state(l.size() + l.size() / 2) {
        for (auto&& [k, v] : l) set(std::move(k), std::move(v));
    }

    auto& set(convertible_to<K> auto&& key, convertible_to<V> auto&& value) {
        if (cap() == 0 || payload() > 0.6) grow();
        for (size_t i = hash(key) % cap(), j = 0; j < cap(); i = (i+1) % cap(), j++) {
            if (!state[i].occupied) {
                data[i] = pair{
                    std::forward<decltype(key)>(key),
                    std::forward<decltype(value)>(value)
                };
                state[i].occupied = true;
                len++;
                break;
            } else if (data[i].key == key) {
                data[i].value = std::forward<decltype(value)>(value);
                break;
            } else {
                state[i].conflict = true;
            }
        }
        return *this;
    }

    auto contains(const K& key) {
        return find(key) != nullptr;
    }

    auto get(const K& key) -> std::optional<std::reference_wrapper<V>> {
        if (auto t = find(key); t != nullptr) return t->value;
        return std::nullopt;
    }

    auto remove(const K& key) {
        if (auto t = find(key); t != nullptr) {
            state[t - data.begin()].occupied = false;
            len--;
            return true;
        }
        return false;
    }

    struct iter {
        hashmap* const map;
        size_t index;
        auto& operator++() {
            do {
                index++;
            } while (index < map->data.size() && !map->state[index].occupied);
            return *this;
        }
        auto operator!=(iter& i) { return index != i.index; }
        auto& operator*() { return map->data[index]; }
    };

    auto begin() { return iter{ this, first() }; }
    auto end() { return iter{ this, data.size() }; }
    auto size() { return len; }
};

}