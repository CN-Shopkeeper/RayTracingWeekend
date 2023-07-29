#pragma once

#include "hittable.hpp"
#include "hittable_list.hpp"
#include "rtweekend.hpp"

class BVHNode : public Hittable {
   public:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;

    AABB box;

    BVHNode() {}
    BVHNode(const HittableList& list, double time0, double time1)
        : BVHNode(list.objects, 0, list.objects.size(), time0, time1) {}
    BVHNode(const std::vector<std::shared_ptr<Hittable>>& src_objects,
            size_t start, size_t end, double time0, double time1);

    virtual bool Hit(const Ray& r, double tMin, double tMax,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override;
};

BVHNode::BVHNode(const std::vector<std::shared_ptr<Hittable>>& src_objects,
                 size_t start, size_t end, double time0, double time1) {
    // Create a modifiable array of the source scene objects
    auto objects = src_objects;
    // 随机选择一个轴
    int axis = RandomInt(0, 2);
    auto comparator = (axis == 0)   ? BoxCompareX
                      : (axis == 1) ? BoxCompareY
                                    : BoxCompareZ;

    size_t objectSpan = end - start;

    if (objectSpan == 1) {
        left = right = objects[start];
    } else if (objectSpan == 2) {
        if (comparator(objects[start], objects[start + 1])) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            left = objects[start + 1];
            right = objects[start];
        }
    } else {
        std::sort(objects.begin() + start, objects.begin() + end, comparator);

        auto mid = start + objectSpan / 2;

        left = std::make_shared<BVHNode>(objects, start, mid, time0, time1);
        right = std::make_shared<BVHNode>(objects, mid, end, time0, time1);
    }

    AABB boxLeft, boxRight;

    if (!left->BoundingBox(time0, time1, boxLeft) ||
        !right->BoundingBox(time0, time1, boxRight)) {
        std::cerr << "No bounding box in bvh_node constructor.\n";
    }
    box = SurroundingBox(boxLeft, boxRight);
}

bool BVHNode::Hit(const Ray& r, double tMin, double tMax,
                  HitRecord& rec) const {
    if (!box.Hit(r, tMin, tMax)) {
        return false;
    }

    bool hitLeft = left->Hit(r, tMin, tMax, rec);
    bool hitRight = right->Hit(r, tMin, hitLeft ? rec.t : tMax, rec);

    return hitLeft || hitRight;
}

bool BVHNode::BoundingBox(double time0, double time1, AABB& outputBox) const {
    outputBox = box;
    return true;
}

inline bool BoxCompare(const std::shared_ptr<Hittable> a,
                       const std::shared_ptr<Hittable> b, int axis) {
    AABB boxA;
    AABB boxB;

    if (!a->BoundingBox(0, 0, boxA) || !b->BoundingBox(0, 0, boxB)) {
        std::cerr << "No bounding box in bvh_node constructor.\n";
    }

    return boxA.Min().e[axis] < boxB.Min().e[axis];
}

bool BoxCompareX(const std::shared_ptr<Hittable> a,
                 const std::shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 0);
}

bool BoxCompareY(const std::shared_ptr<Hittable> a,
                 const std::shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 1);
}

bool BoxCompareZ(const std::shared_ptr<Hittable> a,
                 const std::shared_ptr<Hittable> b) {
    return BoxCompare(a, b, 2);
}
