export module vec2;

#define DEFINE_OPERATOR(op)                                                                        \
    template <typename U>                                                                          \
    constexpr vec2<decltype(T() op U())> operator op(vec2<U> other) const {                        \
        return { x op other.x, y op other.y };                                                     \
    }                                                                                              \
    template <typename U>                                                                          \
    constexpr vec2<decltype(T() op U())> operator op(U number) const {                             \
        return { x op number, y op number };                                                       \
    }

export enum Dimension : bool { X, Y };

export template <typename T>
struct vec2 {
    T x;
    T y;

    constexpr vec2() {}
    constexpr vec2(T x, T y) : x(x), y(y) {}

    constexpr T& operator[](Dimension dimension) {
        return dimension == X ? x : y;
    }

    DEFINE_OPERATOR(+)
    DEFINE_OPERATOR(-)
    DEFINE_OPERATOR(*)
    DEFINE_OPERATOR(/)
    DEFINE_OPERATOR(>>)
    DEFINE_OPERATOR(&)

    constexpr bool operator==(vec2 other) const {
        return x == other.x && y == other.y;
    }

    template <typename U>
    constexpr explicit operator vec2<U>() const {
        return { U(x), U(y) };
    }
};
