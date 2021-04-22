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

enum Direction : uchar {
    Horizontal,
    Vertical
};

template <typename T>
union vec2 {
    struct {
        T x, y;
    };
    T xy[2];

    constexpr vec2 operator +(vec2 other) const {
        return { x + other.x, y + other.y };
    }

    constexpr vec2 operator -(vec2 other) const {
        return { x - other.x, y - other.y };
    }

    constexpr vec2 operator *(T number) const {
        return { x * number, y * number };
    }

    constexpr vec2 operator >>(T number) const {
        return { x >> number, y >> number };
    }

    constexpr vec2 operator &(T number) const {
        return { x & number, y & number };
    }

    constexpr bool operator ==(vec2 other) const {
        return x == other.x && y == other.y;
    }
};

