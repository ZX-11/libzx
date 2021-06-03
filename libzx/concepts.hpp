#pragma once
#include <type_traits>

namespace libzx {

using std::convertible_to;

template<typename T>
concept iterable = requires(T t) { t.begin(); t.end(); };

template<typename T>
concept iterator = requires(T t) { t++; *t; t != t; };

template<typename T>
concept continuous_iterator = requires(T t) { t++; *t; t - t; t != t; };

template<typename T>
concept indexable = requires(T t) { t.size(); t[0]; };

template<typename T>
concept sliceable = requires(T t) { t.end() - t.begin(); };

template<typename T>
concept streamable = requires(T t) { t.begin(); t.end(); t.size(); };

template<typename T>
concept container = requires(T t, size_t s) { T(s); t[s]; };

template<typename T>
concept comparable = requires(T t) { t <=> t; };

template<typename T, typename W>
concept comparable_with = requires(T t, W w) { t <=> w; };

template<typename T>
concept equality_comparable = requires(T t) { t == t; };

template<typename T, typename J>
concept joinable_to = requires(T t, J j) { j += t; };

template<typename T>
concept integral = std::is_integral_v<T> || std::is_pointer_v<T>;

}