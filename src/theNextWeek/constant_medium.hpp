#pragma once

#include "hittable.hpp"
#include "material.hpp"
#include "rtweekend.hpp"
#include "texture.hpp"

class ConstantMedium : public Hittable {
   public:
    std::shared_ptr<Hittable> boundary;
    std::shared_ptr<Material> phaseFunction;
    double negInvDensity;

    ConstantMedium(std::shared_ptr<Hittable> b, double d,
                   std::shared_ptr<Texture> a)
        : boundary(b),
          negInvDensity(-1 / d),
          phaseFunction(std::make_shared<Isotropic>(a)) {}

    ConstantMedium(std::shared_ptr<Hittable> b, double d, Color c)
        : boundary(b),
          negInvDensity(-1 / d),
          phaseFunction(std::make_shared<Isotropic>(c)) {}

    virtual bool Hit(const Ray& r, double tMin, double tMax,
                     HitRecord& rec) const override;

    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override {
        return boundary->BoundingBox(time0, time1, outputBox);
    }
};

bool ConstantMedium::Hit(const Ray& r, double tMin, double tMax,
                         HitRecord& rec) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && RandomDouble() < 0.00001;

    HitRecord rec1, rec2;

    if (!boundary->Hit(r, -infinity, infinity, rec1)) {
        return false;
    }

    if (!boundary->Hit(r, rec1.t + 0.0001, infinity, rec2)) {
        return false;
    }

    if (debugging)
        std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

    if (rec1.t < tMin) rec1.t = tMin;
    if (rec2.t > tMax) rec2.t = tMax;

    if (rec1.t >= rec2.t) {
        return false;
    }

    if (rec1.t < 0) rec1.t = 0;

    const auto rayLength = r.Direction().Length();
    const auto distanceInsideBoundary = (rec2.t - rec1.t) * rayLength;
    const auto hitDistance = negInvDensity * log(RandomDouble());

    if (hitDistance > distanceInsideBoundary) return false;

    rec.t = rec1.t + hitDistance / rayLength;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance = " << hitDistance << '\n'
                  << "rec.t = " << rec.t << '\n'
                  << "rec.p = " << rec.p << '\n';
    }

    rec.normal = Vec3(1, 0, 0);  // arbitrary
    rec.front_face = true;       // also arbitrary
    rec.matPtr = phaseFunction;

    return true;
}
