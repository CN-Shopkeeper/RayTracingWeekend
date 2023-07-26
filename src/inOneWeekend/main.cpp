#include <iostream>

#include "color.hpp"
#include "ray.hpp"
#include "vec3.hpp"

Color RayColor(const Ray& r);

int main() {
    //  Image

    const auto aspectRatio = 16.0 / 9.0;
    const int imageWidth = 400;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);

    // Camera

    // [-1, 1] [-1, 1]
    auto viewportHeight = 2.0;
    auto viewportWidth = aspectRatio * viewportHeight;
    // near plane
    auto focalLength = 1.0;

    auto origin = Point3{0, 0, 0};
    auto horizontal = Vec3{viewportWidth, 0, 0};
    auto vertical = Vec3{0, viewportHeight, 0};
    auto lowerLeftCorner =
        origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focalLength);

    // Render

    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    // ! 直接重定向会导致输出的文件是带有BOM的UTF-16的文件
    // ! .\inOneWeekend.exe | set-content image.ppm -encoding String
    for (int j = imageHeight - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            auto u = double(i) / (imageWidth - 1);
            auto v = double(j) / (imageHeight - 1);
            Ray r{origin,
                  lowerLeftCorner + u * horizontal + v * vertical - origin};
            Color pixelColor = RayColor(r);
            WriteColor(std::cout, pixelColor);
        }
        std::cout << "\n";
    }
    std::cerr << "\nDone.\n";
    return 0;
}

Color RayColor(const Ray& r) {
    Vec3 unitDir = UnitVector(r.Direction());
    auto t = 0.5 * (unitDir.Y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}