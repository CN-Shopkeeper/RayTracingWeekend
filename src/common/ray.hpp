#pragma once

#include "vec3.hpp"

class Ray {
   public:
    Point3 origin;
    Vec3 dir;

    Ray() {}
    Ray(const Point3& origin, const Vec3& dir) : origin(origin), dir(dir) {}

    Point3 Origin() const { return origin; }
    Vec3 Direction() const { return dir; }

    Point3 at(double t) const { return origin + t * dir; }
};