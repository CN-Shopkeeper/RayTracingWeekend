#pragma once

#include "rtweekend.hpp"

class HitRecord;

class Material {
   public:
    virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const = 0;
};

class Lambertian : public Material {
   public:
    Color albedo;

    Lambertian(const Color& a) : albedo(a) {}

    virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
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

    virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const override {
        Vec3 reflected = Reflect(UnitVector(rayIn.Direction()), rec.normal);
        scattered = Ray(rec.p, reflected + fuzz * RandomInUnitSphere());
        attenuation = albedo;
        return (Dot(scattered.Direction(), rec.normal) > 0);
    }
};

class Dielectric : public Material {
   public:
    // index of Refraction
    double ir;

    Dielectric(double indexOfRefraction) : ir(indexOfRefraction) {}

    virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
                         Color& attenuation, Ray& scattered) const override {
        // always refracts
        attenuation = Color(1.0, 1.0, 1.0);
        double refractionRatio = rec.front_face ? (1.0 / ir) : ir;

        Vec3 unitDirection = UnitVector(rayIn.Direction());
        double cosTheta = fmin(Dot(-unitDirection, rec.normal), 1.0);
        double sinTheta = sqrt(1.0 - cosTheta * cosTheta);

        bool cannotRefract = refractionRatio * sinTheta > 1.0;
        Vec3 direction;

        if (cannotRefract) {
            direction = Reflect(unitDirection, rec.normal);
        } else {
            direction = Refract(unitDirection, rec.normal, refractionRatio);
        }

        scattered = Ray(rec.p, direction);
        return true;
    }
};
