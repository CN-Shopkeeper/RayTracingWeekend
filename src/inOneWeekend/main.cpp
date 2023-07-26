#include <iostream>

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"

Color RayColor(const Ray& r, const Hittable& world, int depth);

int main() {
    //  Image

    const auto aspectRatio = 16.0 / 9.0;
    const int imageWidth = 400;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    const int samplePerPixels = 100;
    const int maxDepth = 50;

    // World
    HittableList world;
    world.add(std::make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.add(std::make_shared<Sphere>(Point3(0, -100.5, -1), 100));

    // Camera

    Camera camera;

    // Render

    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    // ! 直接重定向会导致输出的文件是带有BOM的UTF-16的文件
    // ! .\inOneWeekend.exe | set-content image.ppm -encoding String
    for (int j = imageHeight - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            Color pixelColor = Color{0, 0, 0};
            for (int s = 0; s < samplePerPixels; ++s) {
                // 一个像素取100条打在这个像素内的光线
                auto u = (i + RandomDouble()) / (imageWidth - 1);
                auto v = (j + RandomDouble()) / (imageHeight - 1);
                Ray r = camera.GetRay(u, v);
                pixelColor += RayColor(r, world, maxDepth);
            }
            WriteColor(std::cout, pixelColor, samplePerPixels);
        }
        std::cout << "\n";
    }
    std::cerr << "\nDone.\n";
    return 0;
}

Color RayColor(const Ray& r, const Hittable& world, int depth) {
    HitRecord rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) return Color(0, 0, 0);
    if (world.Hit(r, 0, infinity, rec)) {
        // 随机漫反射
        Point3 target = rec.p + rec.normal + RandomInUnitSphere();
        // 递归，多次反射
        return 0.5 * RayColor(Ray(rec.p, target - rec.p), world, depth - 1);
    }
    Vec3 unitDir = UnitVector(r.Direction());
    auto t = 0.5 * (unitDir.Y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}
