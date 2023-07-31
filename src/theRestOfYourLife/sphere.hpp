#pragma once

#include "hittable.hpp"
#include "vec3.hpp"

class Sphere : public Hittable {
   private:
    static void getSphereUV(const Point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.Y());
        auto phi = atan2(-p.Z(), p.X()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

   public:
    Point3 center;
    double radius;
    std::shared_ptr<Material> matPtr;

    Sphere() {}
    Sphere(Point3 cen, double r, std::shared_ptr<Material> m)
        : center(cen), radius(r), matPtr(m) {}

    virtual bool Hit(const Ray& r, double t_min, double t_max,
                     HitRecord& rec) const override;
    virtual bool BoundingBox(double time0, double time1,
                             AABB& outputBox) const override;
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
    getSphereUV(outwardNormal, rec.u, rec.v);
    rec.matPtr = matPtr;

    return true;
}

bool Sphere::BoundingBox(double time0, double time1, AABB& outputBox) const {
    outputBox = AABB{center - Vec3{radius, radius, radius},
                     center + Vec3{radius, radius, radius}};
    return true;
}