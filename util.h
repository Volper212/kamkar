#pragma once

using uint = unsigned int;
using uchar = unsigned char;

enum Direction : unsigned char {
    Horizontal,
    Vertical
};

template <typename T>
union tvec2 {
    struct {
        T x, y;
    };
    T xy[2];
};

using vec2 = tvec2<float>;
using ivec2 = tvec2<int>;

#define BINARY_OPERATOR_VECTORS(op) template <typename T> constexpr tvec2<T> operator op(tvec2<T> left, tvec2<T> right) { return { left.x op right.x, left.y op right.y }; }

BINARY_OPERATOR_VECTORS(+)

#define BIT_OPERATOR_VECTOR_NUMBER(op) __forceinline constexpr ivec2 operator op(ivec2 left, int right) { return { left.x op right, left.y op right }; }

BIT_OPERATOR_VECTOR_NUMBER(>>)
BIT_OPERATOR_VECTOR_NUMBER(&)
BIT_OPERATOR_VECTOR_NUMBER(*)

template <typename T>
constexpr bool operator ==(tvec2<T> left, tvec2<T> right) {
    return left.x == right.x && left.y == right.y;
}