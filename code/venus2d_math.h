/* date = February 25th 2021 3:43 pm */

#ifndef VENUS2D_MATH_H
#define VENUS2D_MATH_H

#include <math.h>

struct v2
{
    float x;
    float y;
};

inline
v2 new_v2(float x, float y)
{
    return {x, y};
}

inline
v2 operator+(v2 v0, v2 v1)
{
    return {v0.x + v1.x, v0.y + v1.y};
}

inline
v2 operator-(v2 v0, v2 v1)
{
    return {v0.x - v1.x, v0.y - v1.y};
}

inline
void operator+=(v2& v0, v2 v1)
{
    v0 = v0 + v1;
}

inline
void operator-=(v2& v0, v2 v1)
{
    v0 = v0 - v1;
}

inline
v2 operator*(v2 v0, float s)
{
    return {v0.x * s, v0.y * s};
}

inline
v2 operator*(v2 v0, v2 v1)
{
    return {v0.x * v1.x, v0.y * v1.y};
}

inline
v2 operator*(float s, v2 v0)
{
    return {v0.x * s, v0.y * s};
}

inline
void operator*=(v2& v0, float s)
{
    v0 = v0 * s;
}

inline
v2 operator/(v2 v0, float s)
{
    return {v0.x / s, v0.y / s};
}

inline
v2 operator/(v2 v0, v2 v1)
{
    return {v0.x / v1.x, v0.y / v1.y};
}

v2 operator-(v2 v)
{
    return {-v.x, -v.y};
}

inline
float v2_dot(v2 v0, v2 v1)
{
    return v0.x * v1.x + v0.y * v1.y;
}

inline
float v2_leght(v2 v)
{
    return sqrtf(v2_dot(v, v));
}

inline
v2 v2_normalize(v2 v)
{
    float len = v2_leght(v);
    return {v.x / len, v.y / len};
}

inline
v2 v2_perp(v2 v)
{
    return {-v.y, v.x};
}

inline
v2 v2_lerp(v2 v0, v2 v1, float t)
{
    v2 resutl = (1-t)*v0 + t*v1;
}

inline
uint32_t lerp(uint32_t u0, uint32_t u1, float t)
{
    return (1-t)*(float)u0 + t*(float)u1;
}

inline
float lerp(float f0, float f1, float t)
{
    return (1-t)*f0 + t*f1;
}

#endif //VENUS2D_MATH_H
