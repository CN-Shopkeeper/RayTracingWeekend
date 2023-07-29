#pragma once

#include "rtweekend.hpp"

class Texture {
   public:
    virtual Color Value(double u, double v, const Point3& p) const = 0;
};

class SolidColor : public Texture {
   private:
    Color colorValue;

   public:
    SolidColor() {}
    SolidColor(Color c) : colorValue(c) {}
    SolidColor(double red, double green, double blue)
        : colorValue(Color{red, green, blue}) {}

    virtual Color Value(double u, double v, const Point3& p) const override {
        return colorValue;
    };
};

class CheckerTexture : public Texture {
   public:
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;

    CheckerTexture() {}
    CheckerTexture(std::shared_ptr<Texture> _odd,
                   std::shared_ptr<Texture> _even)
        : odd(_odd), even(_even) {}
    CheckerTexture(Color c1, Color c2)
        : even(std::make_shared<SolidColor>(c1)),
          odd(std::make_shared<SolidColor>(c2)) {}

    virtual Color Value(double u, double v, const Point3& p) const override {
        auto sines = sin(10 * p.X()) * sin(10 * p.Y()) * sin(10 * p.Z());
        if (sines < 0) {
            return odd->Value(u, v, p);
        } else {
            return even->Value(u, v, p);
        }
    }
};
