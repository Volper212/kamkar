#pragma once

using uint = unsigned int;
using uchar = unsigned char;

template <typename T, int N>
constexpr int lengthof(T(&array)[N]) {
    return N;
}

constexpr int max(int left, int right) {
    if (left < right) {
        return right;
    }
    return left;
}

constexpr int min(int left, int right) {
    if (left < right) {
        return left;
    }
    return right;
}

constexpr int iceil(float input) {
    const int trunced = int(input);
    return trunced + (trunced < input);
}

constexpr int iround(float input) {
    return iceil(input - 0.5f);
}

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
BINARY_OPERATOR_VECTORS(-)

constexpr vec2 operator /(vec2 left, float right) { return { left.x / right, left.y / right }; }

#define BIT_OPERATOR_VECTOR_NUMBER(op) constexpr ivec2 operator op(ivec2 left, int right) { return { left.x op right, left.y op right }; }

BIT_OPERATOR_VECTOR_NUMBER(>>)
BIT_OPERATOR_VECTOR_NUMBER(&)
BIT_OPERATOR_VECTOR_NUMBER(*)

constexpr bool operator ==(ivec2 left, ivec2 right) {
    return left.x == right.x && left.y == right.y;
}
