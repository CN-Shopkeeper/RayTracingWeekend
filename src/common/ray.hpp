#pragma once

#include "vec3.hpp"

class Ray {
   public:
    Point3 origin;
    Vec3 dir;
    double time;

    Ray() {}
    Ray(const Point3& origin, const Vec3& dir, double tm = 0.0)
        : origin(origin), dir(dir), time(tm) {}

    Point3 Origin() const { return origin; }
    Vec3 Direction() const { return dir; }
    double Time() const { return time; }

    Point3 at(double t) const { return origin + t * dir; }
};