#pragma once

#include "rtweekend.hpp"

class Camera {
   private:
    Point3 origin_;
    Point3 lowerLeftCorner_;
    Vec3 horizontal_;
    Vec3 vertical_;

   public:
    Camera() {
        auto aspect_ratio = 16.0 / 9.0;
        // [-1, 1] [-1, 1]
        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        // near plane
        auto focal_length = 1.0;

        origin_ = Point3(0, 0, 0);
        horizontal_ = Vec3(viewport_width, 0.0, 0.0);
        vertical_ = Vec3(0.0, viewport_height, 0.0);
        lowerLeftCorner_ = origin_ - horizontal_ / 2 - vertical_ / 2 -
                           Vec3(0, 0, focal_length);
    }

    Ray GetRay(double u, double v) const {
        return Ray(origin_, lowerLeftCorner_ + u * horizontal_ + v * vertical_ -
                                origin_);
    }
};