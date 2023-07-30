#pragma once

#include "hittable.hpp"
#include "rtweekend.hpp"

class XYRect : public Hittable {
   public:
    std::shared_ptr<Material> mp;
    double x0, x1, y0, y1, k;

    XYRect() {}

    XYRect(double _x0, double _x1, double _y0, double _y1, double _k,
           std::shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override {
        // The bounding box must have non-zero width in each dimension, so pad
        // the Z dimension a small amount.

        outputBox =
            AABB(Point3{x0, y0, k - 0.0001}, Point3{x1, y1, k + 0.0001});
        return true;
    }
};
class XZRect : public Hittable {
   public:
    std::shared_ptr<Material> mp;
    double x0, x1, z0, z1, k;

    XZRect() {}

    XZRect(double _x0, double _x1, double _z0, double _z1, double _k,
           std::shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override {
        // The bounding box must have non-zero width in each dimension, so pad
        // the Y dimension a small amount.

        outputBox =
            AABB(Point3{x0, k - 0.0001, z0}, Point3{x1, k + 0.0001, z1});
        return true;
    }
};
class YZRect : public Hittable {
   public:
    std::shared_ptr<Material> mp;
    double y0, y1, z0, z1, k;

    YZRect() {}

    YZRect(double _y0, double _y1, double _x0, double _x1, double _k,
           std::shared_ptr<Material> mat)
        : y0(_y0), y1(_y1), z0(_x0), z1(_x1), k(_k), mp(mat) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override {
        // The bounding box must have non-zero width in each dimension, so pad
        // the X dimension a small amount.

        outputBox =
            AABB(Point3{k - 0.0001, y0, z0}, Point3{k + 0.0001, y1, z1});
        return true;
    }
};

bool XYRect::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    auto t = (k - r.Origin().Z()) / r.Direction().Z();
    if (t < tMin || t > tMax) {
        return false;
    }
    auto x = r.Origin().X() + t * r.Direction().X();
    auto y = r.Origin().Y() + t * r.Direction().Y();
    if (x < x0 || x > x1 || y < y0 || y > y1) {
        return false;
    }
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (y - y0) / (y1 - y0);
    rec.t = t;

    auto outwardNormal = Vec3{0, 0, 1};
    rec.SetFaceNormal(r, outwardNormal);
    rec.matPtr = mp;
    rec.p = r.at(t);
    return true;
}

bool XZRect::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    auto t = (k - r.Origin().Y()) / r.Direction().Y();
    if (t < tMin || t > tMax) {
        return false;
    }
    auto x = r.Origin().X() + t * r.Direction().X();
    auto z = r.Origin().Z() + t * r.Direction().Z();
    if (x < x0 || x > x1 || z < z0 || z > z1) {
        return false;
    }
    rec.u = (x - x0) / (x1 - x0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;

    auto outwardNormal = Vec3{0, 1, 0};
    rec.SetFaceNormal(r, outwardNormal);
    rec.matPtr = mp;
    rec.p = r.at(t);
    return true;
}
bool YZRect::Hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const {
    auto t = (k - r.Origin().X()) / r.Direction().X();
    if (t < tMin || t > tMax) {
        return false;
    }
    auto z = r.Origin().Z() + t * r.Direction().Z();
    auto y = r.Origin().Y() + t * r.Direction().Y();
    if (z < z0 || z > z1 || y < y0 || y > y1) {
        return false;
    }
    rec.u = (y - y0) / (y1 - y0);
    rec.v = (z - z0) / (z1 - z0);
    rec.t = t;

    auto outwardNormal = Vec3{1, 0, 0};
    rec.SetFaceNormal(r, outwardNormal);
    rec.matPtr = mp;
    rec.p = r.at(t);
    return true;
}