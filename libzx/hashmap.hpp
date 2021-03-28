#pragma once
#include <optional>
#include <stdexcept>
#include "hash.hpp"
#include "range.hpp"
#include "string.hpp"
#include "concepts.hpp"
#include "smart_array.hpp"

namespace libzx {

template<hashable K, equalable V>
struct hashpair { K key; V value; };

template<hashable K, equalable V>
class hashmap {
    using record = hashpair<K,V>;
    unique_array<record> data;
    unique_array<bool> occupied;
    size_t len = 0;
    size_t cap() { return data.size(); }
    double payload() { return (double)len / (double)data.size(); }

    void grow() {
        auto new_cap = std::max(data.size() * 2, data.size() + 1);
        auto new_data = unique_array<record>(new_cap);
        auto new_occupied = unique_array<bool>(new_cap);
        for (auto i : range(data)) {
            if (!occupied[i]) continue;
            for (size_t j = hash(data[i].key) % new_cap, k = 0; k < new_cap; j = (j+1) % new_cap, k++) {
                if (new_occupied[j]) continue;
                new_data[j] = record{
                    std::move(data[i].key),
                    std::move(data[i].value)
                };
                new_occupied[j] = true;
                break;
            }
        }
        data = std::move(new_data);
        occupied = std::move(new_occupied);
    }

    auto find(const K& key) {
        for (size_t i = hash(key) % cap(), j = 0; j < cap(); i = (i+1)%cap(), j++) {
            if (occupied[i] && data[i].key == key) {
                return &data[i];
            } else if (!occupied[i]) {
                break;
            }
        }
        return (record*)nullptr;
    }

    size_t first() {
        for (auto i : range(data)) {
            if (occupied[i]) return i;
        }
        return data.size();
    }
public:
    hashmap(size_t min_cap = 16) : data(min_cap), occupied(min_cap) {}

    auto& set(convertible_to<K> auto&& key, convertible_to<V> auto&& value) {
        if (cap() == 0 || payload() > 0.6) grow();
        for (size_t i = hash(key) % cap(), j = 0; j < cap(); i = (i+1) % cap(), j++) {
            if (!occupied[i]) {
                data[i] = record{
                    std::forward<decltype(key)>(key),
                    std::forward<decltype(value)>(value)
                };
                occupied[i] = true;
                len++;
                break;
            } else if (data[i].key == key) {
                data[i].value = std::forward<decltype(value)>(value);
                break;
            }
        }
        return *this;
    }

    auto contains(const K& key) {
        return find(key) != nullptr;
    }

    std::optional<std::reference_wrapper<V>> get(const K& key) {
        if (auto t = find(key); t != nullptr) return std::optional<std::reference_wrapper<V>>(t->value);
        return std::nullopt;
    }

    auto remove(const K& key) {
        if (auto t = find(key); t != nullptr) {
            occupied[t - data.begin()] = false;
            len--;
            return true;
        }
        return false;
    }

    struct iter {
        hashmap* map;
        size_t index;
        auto& operator++() {
            do {
                index++;
            } while (index < map->data.size() && !map->occupied[index]);
            return *this;
        }
        auto operator!=(iter& i) { return index != i.index; }
        auto& operator*() { return map->data[index]; }
    };

    auto begin() { return iter{ this, first() }; }
    auto end() { return iter{ this, data.size() }; }
};

}
