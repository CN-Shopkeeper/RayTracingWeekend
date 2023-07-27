#pragma once

#include "rtweekend.hpp"

class Camera {
   private:
    Point3 origin_;
    Point3 lowerLeftCorner_;
    Vec3 horizontal_;
    Vec3 vertical_;

   public:
    Camera(Point3 lookfrom, Point3 lookat, Vec3 vup,
           double vfov,  // vertical field-of-view in degrees
           double aspectRatio) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewportHeight = 2.0 * h;
        auto viewportWidth = aspectRatio * viewportHeight;

        auto w = UnitVector(lookfrom - lookat);
        auto u = UnitVector(Cross(vup, w));
        auto v = Cross(w, u);

        origin_ = lookfrom;
        horizontal_ = viewportWidth * u;
        vertical_ = viewportHeight * v;
        lowerLeftCorner_ = origin_ - horizontal_ / 2 - vertical_ / 2 - w;
    }

    Ray GetRay(double s, double t) const {
        return Ray(origin_, lowerLeftCorner_ + s * horizontal_ + t * vertical_ -
                                origin_);
    }
};