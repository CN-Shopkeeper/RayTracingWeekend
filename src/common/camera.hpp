#pragma once

#include "rtweekend.hpp"

class Camera {
   private:
    Point3 origin_;
    Point3 lowerLeftCorner_;
    Vec3 horizontal_;
    Vec3 vertical_;
    Vec3 u_, v_, w_;
    double lensRadius_;
    // shutter open/close times
    double time0_, time1_;

   public:
    Camera(Point3 lookfrom, Point3 lookat, Vec3 vup,
           double vfov,  // vertical field-of-view in degrees
           double aspectRatio, double aperture, double focusDist,
           double time0 = 0, double time1 = 0) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewportHeight = 2.0 * h;
        auto viewportWidth = aspectRatio * viewportHeight;

        w_ = UnitVector(lookfrom - lookat);
        u_ = UnitVector(Cross(vup, w_));
        v_ = Cross(w_, u_);

        // * 这些操作保证了落在focus plane上的物体一定是精确的，
        // * 反之，落在非focus plane上则越远越模糊
        origin_ = lookfrom;
        horizontal_ = focusDist * viewportWidth * u_;
        vertical_ = focusDist * viewportHeight * v_;
        lowerLeftCorner_ =
            origin_ - horizontal_ / 2 - vertical_ / 2 - focusDist * w_;

        lensRadius_ = aperture / 2;
        time0_ = time0;
        time1_ = time1;
    }

    Ray GetRay(double s, double t) const {
        Vec3 rd = lensRadius_ * RandomInUnitDisk();
        Vec3 offset = u_ * rd.X() + v_ * rd.Y();
        return Ray(origin_ + offset,
                   lowerLeftCorner_ + s * horizontal_ + t * vertical_ -
                       origin_ - offset,
                   RandomDouble(time0_, time1_));
    }
};