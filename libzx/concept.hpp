namespace libzx {

template<typename T>
concept iterable = requires(T t) { t.begin(); t.end(); t[0]; };

template<typename T>
concept iterator = requires(T a, T b) { a++; *a; b - a; };

template<typename T>
concept comparable = requires(T t) { t <=> t; };

}