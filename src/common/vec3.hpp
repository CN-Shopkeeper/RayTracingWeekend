#pragma once

#include <cmath>
#include <iostream>

class Vec3 {
   public:
    double e[3];

    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double X() const { return e[0]; }
    double Y() const { return e[1]; }
    double Z() const { return e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    Vec3 &operator+=(const Vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3 &operator*=(const double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3 &operator/=(const double t) { return *this *= 1 / t; }

    double Length() const { return sqrt(LengthSquared()); }

    double LengthSquared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    inline static Vec3 Random() {
        return Vec3(RandomDouble(), RandomDouble(), RandomDouble());
    }

    inline static Vec3 Random(double min, double max) {
        return Vec3(RandomDouble(min, max), RandomDouble(min, max),
                    RandomDouble(min, max));
    }

    bool NearZero() const {
        // Return true if the vector is close to zero in all dimensions.
        const auto s = 1e-8;
        return (fabs(e[0]) < s) && (fabs(e[1]) < s) && (fabs(e[2]) < s);
    }
};

// Type aliases for vec3
using Point3 = Vec3;  // 3D point
using Color = Vec3;   // RGB color

// vec3 Utility Functions

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, double t) { return t * v; }

inline Vec3 operator/(Vec3 v, double t) { return (1 / t) * v; }

inline double Dot(const Vec3 &u, const Vec3 &v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 Cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 UnitVector(Vec3 v) { return v / v.Length(); }

Vec3 RandomInUnitSphere() {
    // 在单位半径的球体内随机选择一个点
    // todo 优化效率防止死循环
    while (true) {
        auto p = Vec3::Random(-1, 1);
        if (p.LengthSquared() >= 1) continue;
        return p;
    }
}

Vec3 RandomUnitVector() { return UnitVector(RandomInUnitSphere()); }

Vec3 RandomInHemisphere(const Vec3 &normal) {
    Vec3 inUnitSphere = RandomInUnitSphere();
    if (Dot(inUnitSphere, normal) >
        0.0)  // In the same hemisphere as the normal
        return inUnitSphere;
    else
        return -inUnitSphere;
}

Vec3 Reflect(const Vec3 &v, const Vec3 &n) { return v - 2 * Dot(v, n) * n; }

// 将折射光线拆分成axis-align的两条光线
Vec3 Refract(const Vec3 &uv, const Vec3 &n, double etaiOverEtat) {
    auto cosTheta = fmin(Dot(-uv, n), 1.0);
    Vec3 rOutPerp = etaiOverEtat * (uv + cosTheta * n);
    Vec3 rOutParallel = -sqrt(fabs(1.0 - rOutPerp.LengthSquared())) * n;
    return rOutPerp + rOutParallel;
}

Vec3 RandomInUnitDisk() {
    while (true) {
        auto p = Vec3(RandomDouble(-1, 1), RandomDouble(-1, 1), 0);
        if (p.LengthSquared() >= 1) continue;
        return p;
    }
}
