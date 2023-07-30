#pragma once

#include "aarec.hpp"
#include "hittable_list.hpp"
#include "rtweekend.hpp"

class Box : public Hittable {
   public:
    Point3 boxMin;
    Point3 boxMax;
    HittableList sides;

    Box() {}
    Box(const Point3& p0, const Point3& p1, std::shared_ptr<Material> ptr);

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override {
        outputBox = AABB{boxMin, boxMax};
        return true;
    };
};

Box::Box(const Point3& p0, const Point3& p1, std::shared_ptr<Material> ptr) {
    boxMin = p0, boxMax = p1;

    sides.add(
        std::make_shared<XYRect>(p0.X(), p1.X(), p0.Y(), p1.Y(), p1.Z(), ptr));
    sides.add(
        std::make_shared<XYRect>(p0.X(), p1.X(), p0.Y(), p1.Y(), p0.Z(), ptr));

    sides.add(
        std::make_shared<XZRect>(p0.X(), p1.X(), p0.Z(), p1.Z(), p1.Y(), ptr));
    sides.add(
        std::make_shared<XZRect>(p0.X(), p1.X(), p0.Z(), p1.Z(), p0.Y(), ptr));

    sides.add(
        std::make_shared<YZRect>(p0.Y(), p1.Y(), p0.Z(), p1.Z(), p1.X(), ptr));
    sides.add(
        std::make_shared<YZRect>(p0.Y(), p1.Y(), p0.Z(), p1.Z(), p0.X(), ptr));
}

bool Box::Hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const {
    return sides.Hit(r, t_min, t_max, rec);
}
