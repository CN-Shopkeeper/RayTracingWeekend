#pragma once

#include <iostream>

#include "perlin.hpp"
#include "rtw_std_image.hpp"
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

class NoiseTexture : public Texture {
   private:
    Perlin noise_;
    double scale_;

   public:
    NoiseTexture() {}
    NoiseTexture(double sc) : scale_(sc) {}

    virtual Color Value(double u, double v, const Point3& p) const override {
        return Color{1, 1, 1} * 0.5 *
               (1 + sin(scale_ * p.Z() + 10 * noise_.Turb(p)));
    }
};

class ImageTexture : public Texture {
   private:
    unsigned char* data_;
    int width_, height_;
    int bytesPerScaline_;

   public:
    const static int bytesPerPixel = 3;

    ImageTexture()
        : data_(nullptr), width_(0), height_(0), bytesPerScaline_(0) {}

    ImageTexture(const char* filename) {
        auto componentsPerPixel = bytesPerPixel;

        data_ = stbi_load(filename, &width_, &height_, &componentsPerPixel,
                          componentsPerPixel);

        if (!data_) {
            std::cerr << "ERROR: Could not load texture image file '"
                      << filename << "'.\n";
            width_ = height_ = 0;
        }

        bytesPerScaline_ = bytesPerPixel * width_;
    }

    ~ImageTexture() { delete data_; }

    virtual Color Value(double u, double v, const Point3& p) const override {
        // If we have no texture data, then return solid cyan as a debugging aid
        if (data_ == nullptr) {
            return Color{0, 1, 1};
        }

        // Clamp input texture coordinates to [0,1] x [1,0]
        u = Clamp(u, 0.0, 1.0);
        v = 1.0 - Clamp(v, 0.0, 1.0);  // Flip V to image coordinates

        auto i = static_cast<int>(u * width_);
        auto j = static_cast<int>(v * height_);

        // Clamp integer mapping, since actual coordinates should be less
        // than 1.0
        if (i >= width_) i = width_ - 1;
        if (j >= height_) j = height_ - 1;

        const auto colorScale = 1.0 / 255.0;
        auto pixel = data_ + j * bytesPerScaline_ + i * bytesPerPixel;
        return Color{colorScale * pixel[0], colorScale * pixel[1],
                     colorScale * pixel[2]};
    }
};
