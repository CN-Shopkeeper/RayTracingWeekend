#pragma once

#include "ray.hpp"

class HitRecord {
   public:
    Point3 p;
    Vec3 normal;
    double t;

    bool front_face;

    inline void SetFaceNormal(const Ray& r, const Vec3& outward_normal) {
        front_face = Dot(r.Direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
   public:
    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const = 0;
};