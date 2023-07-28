#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

class MovingSphere : public Hittable {
   public:
    Point3 center0, center1;
    double time0, time1;
    double radius;
    std::shared_ptr<Material> matPtr;

    MovingSphere() {}
    MovingSphere(Point3 cen0, Point3 cen1, double tm0, double tm1, double r,
                 std::shared_ptr<Material> m)
        : center0(cen0),
          center1(cen1),
          time0(tm0),
          time1(tm1),
          radius(r),
          matPtr(m) {}

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;

    Point3 Center(double time) const;
};

bool MovingSphere::Hit(const Ray& r, double t_min, double t_max,
                       HitRecord& rec) const {
    // 相对于Shpere，只是改变了center的获取方式
    // 由于只是进行学习，不需要考虑进一步抽象和封装

    Vec3 oc = r.Origin() - Center(r.Time());
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
    Vec3 outwardNormal = (rec.p - Center(r.Time())) / radius;
    rec.SetFaceNormal(r, outwardNormal);
    rec.matPtr = matPtr;

    return true;
}

Point3 MovingSphere::Center(double time) const {
    return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}