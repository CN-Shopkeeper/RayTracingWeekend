#pragma once

#include "aabb.hpp"
#include "ray.hpp"
#include "rtweekend.hpp"

class Material;

class HitRecord {
   public:
    Point3 p;
    Vec3 normal;
    std::shared_ptr<Material> matPtr;
    double t;

    // 对于球体的一个点(θ,ϕ)
    // 其纹理坐标u = ϕ/2Π, v = θ/Π
    double u, v;

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

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const = 0;
};