#pragma once

#include "rtweekend.hpp"

class HitRecord;

class Material {
   public:
    virtual bool scatter(const Ray& rayIn, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
   public:
    Color albedo;

    Lambertian(const Color& a) : albedo(a) {}

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const override {
        // 随机漫反射
        auto scatterDirection = rec.normal + RandomUnitVector();

        // Lambertian Reflection
        // auto scatterDirection = rec.p + rec.normal + RandomUnitVector();

        // Catch degenerate scatter direction
        if (scatterDirection.NearZero()) scatterDirection = rec.normal;

        scattered = Ray(rec.p, scatterDirection);
        attenuation = albedo;
        return true;
    }
};

class Metal : public Material {
   public:
    Color albedo;
    double fuzz;

    Metal(const Color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

    virtual bool scatter(const Ray& rayIn, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const override {
        Vec3 reflected = Reflect(UnitVector(rayIn.Direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * RandomInUnitSphere());
        attenuation = albedo;
        return (Dot(scattered.Direction(), rec.normal) > 0);
    }
};
