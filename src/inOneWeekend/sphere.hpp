#pragma once

#include "hittable.hpp"
#include "vec3.hpp"

class Sphere : public Hittable {
   public:
    Point3 center;
    double radius;

    Sphere() {}
    Sphere(Point3 cen, double r) : center(cen), radius(r) {}

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;
};

bool Sphere::Hit(const Ray& r, double t_min, double t_max,
                 HitRecord& rec) const {
    Vec3 oc = r.Origin() - center;
    auto a = r.Direction().LengthSquared();
    auto half_b = Dot(oc, r.Direction());
    auto c = oc.LengthSquared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (-half_b - sqrtd) / a;
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root) return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    Vec3 outwardNormal = (rec.p - center) / radius;
    rec.SetFaceNormal(r, outwardNormal);

    return true;
}