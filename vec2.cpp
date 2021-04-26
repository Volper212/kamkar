export module vec2;

export template <typename T>
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
