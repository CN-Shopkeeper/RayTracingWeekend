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

class Translate : public Hittable {
   public:
    Translate(std::shared_ptr<Hittable> p, const Vec3& displacement)
        : ptr(p), offset(displacement) {}

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& output_box) const override;

   public:
    std::shared_ptr<Hittable> ptr;
    Vec3 offset;
};

bool Translate::Hit(const Ray& r, double t_min, double t_max,
                    HitRecord& rec) const {
    Ray moved_r(r.Origin() - offset, r.Direction(), r.Time());
    if (!ptr->Hit(moved_r, t_min, t_max, rec)) return false;

    rec.p += offset;
    rec.SetFaceNormal(moved_r, rec.normal);

    return true;
}

bool Translate::BoundingBox(double time0, double time1,
                            AABB& output_box) const {
    if (!ptr->BoundingBox(time0, time1, output_box)) return false;

    output_box = AABB(output_box.Min() + offset, output_box.Max() + offset);

    return true;
}

class RotateY : public Hittable {
   public:
    RotateY(std::shared_ptr<Hittable> p, double angle);

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& output_box) const override {
        output_box = bbox;
        return hasbox;
    }

   public:
    std::shared_ptr<Hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    AABB bbox;
};

RotateY::RotateY(std::shared_ptr<Hittable> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    hasbox = ptr->BoundingBox(0, 1, bbox);

    Point3 min(infinity, infinity, infinity);
    Point3 max(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i * bbox.Max().X() + (1 - i) * bbox.Min().X();
                auto y = j * bbox.Max().Y() + (1 - j) * bbox.Min().Y();
                auto z = k * bbox.Max().Z() + (1 - k) * bbox.Min().Z();

                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                Vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = fmin(min[c], tester[c]);
                    max[c] = fmax(max[c], tester[c]);
                }
            }
        }
    }

    bbox = AABB(min, max);
}

bool RotateY::Hit(const Ray& r, double t_min, double t_max,
                  HitRecord& rec) const {
    auto origin = r.Origin();
    auto direction = r.Direction();

    origin[0] = cos_theta * r.Origin()[0] - sin_theta * r.Origin()[2];
    origin[2] = sin_theta * r.Origin()[0] + cos_theta * r.Origin()[2];

    direction[0] = cos_theta * r.Direction()[0] - sin_theta * r.Direction()[2];
    direction[2] = sin_theta * r.Direction()[0] + cos_theta * r.Direction()[2];

    Ray rotated_r(origin, direction, r.Time());

    if (!ptr->Hit(rotated_r, t_min, t_max, rec)) return false;

    auto p = rec.p;
    auto normal = rec.normal;

    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    rec.p = p;
    rec.SetFaceNormal(rotated_r, normal);

    return true;
}
