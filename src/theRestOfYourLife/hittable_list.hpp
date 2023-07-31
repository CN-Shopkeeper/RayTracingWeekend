#pragma once

#include <memory>
#include <vector>

#include "hittable.hpp"

class HittableList : public Hittable {
   public:
    std::vector<std::shared_ptr<Hittable>> objects;

    HittableList() {}
    HittableList(std::shared_ptr<Hittable> object) { add(object); }

    void clear() { objects.clear(); }
    void add(std::shared_ptr<Hittable> object) { objects.push_back(object); }

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override;
};

bool HittableList::Hit(const Ray& r, double t_min, double t_max,
                       HitRecord& rec) const {
    HitRecord tempRec;
    bool hitAnything = false;
    auto closestSoFar = t_max;

    // 根据closestSoFar来决定最近的物体
    for (const auto& object : objects) {
        if (object->Hit(r, t_min, closestSoFar, tempRec)) {
            hitAnything = true;
            closestSoFar = tempRec.t;
            rec = tempRec;
        }
    }

    return hitAnything;
}

bool HittableList::BoundingBox(double time0, double time1,
                               AABB& outputBox) const {
    if (objects.empty()) {
        return false;
    }

    AABB tempBox;
    bool firstBox = true;

    for (const auto& object : objects) {
        if (!object->BoundingBox(time0, time1, tempBox)) {
            return false;
        }
        outputBox = firstBox ? tempBox : SurroundingBox(outputBox, tempBox);
        firstBox = false;
    }

    return true;
}
