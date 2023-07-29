#pragma once

#include "rtweekend.hpp"

class AABB {
   public:
    Point3 maximum;
    Point3 minimum;

    AABB() {}
    AABB(const Point3& a, const Point3& b) : minimum(a), maximum(b) {}

    Point3 Min() const { return minimum; }
    Point3 Max() const { return maximum; }

    bool Hit(const Ray& r, double tMin, double tMax) const {
        for (int a = 0; a < 3; a++) {
            auto invD = 1.0f / r.Direction()[a];
            auto t0 = (minimum[a] - r.Origin()[a]) * invD;

            auto t1 = (maximum[a] - r.Origin()[a]) * invD;
            if (invD < 0.0f) {
                std::swap(t0, t1);
            }
            tMin = t0 > tMin ? t0 : tMin;
            tMax = t1 < tMax ? t1 : tMax;
            if (tMax <= tMin) {
                return false;
            }
        }
        return true;
    }
};

AABB SurroundingBox(const AABB& box0, const AABB& box1) {
    Point3 small{fmin(box0.Min().X(), box1.Min().X()),
                 fmin(box0.Min().Y(), box1.Min().Y()),
                 fmin(box0.Min().Z(), box1.Min().Z())};
    Point3 big{fmax(box0.Max().X(), box1.Max().X()),
               fmax(box0.Max().Y(), box1.Max().Y()),
               fmax(box0.Max().Z(), box1.Max().Z())};
    return AABB{small, big};
}