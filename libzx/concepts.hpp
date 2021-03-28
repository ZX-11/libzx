#pragma once
#include <type_traits>
namespace libzx {

template<typename T>
concept iterable = requires(T t) { t.begin(); t.end(); t[0]; };

template<typename T>
concept iterator = requires(T a, T b) { a++; *a; b - a; };

template<typename T>
concept comparable = requires(T t) { t <=> t; };

template<typename T, typename W>
concept comparable_with = requires(T t, W w) { t <=> w; };

template<typename T>
concept equalable = requires(T t) { t == t; };

template<typename T, typename J>
concept joinable_to = requires(T t, J j) { j += t; };

template<typename F, typename T>
concept convertible_to = std::is_convertible_v<F, T>;

template<typename T>
concept integral = std::is_integral_v<T> || std::is_pointer_v<T>;

template<typename T>
concept hashable = true;    // TODO

}