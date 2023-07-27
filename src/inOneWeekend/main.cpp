#include <iostream>

#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"

Color RayColor(const Ray& r, const Hittable& world, int depth);
HittableList RandomScene();

int main() {
    //  Image

    const auto aspectRatio = 16.0 / 9.0;
    const int imageWidth = 800;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    const int samplePerPixels = 500;
    const int maxDepth = 50;

    // World

    auto world = RandomScene();

    // Camera

    Point3 lookFrom = Point3(13, 2, 3);
    Point3 lookAt = Point3(0, 0, 0);
    Vec3 vup = Vec3(0, 1, 0);
    auto distToFocus = 10.0;
    auto aperture = 0.1;
    Camera camera(lookFrom, lookAt, vup, 20.0, aspectRatio, aperture,
                  distToFocus);

    // Render

    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    // ! 直接重定向会导致输出的文件是带有BOM的UTF-16的文件
    // ! .\inOneWeekend.exe | set-content image.ppm -encoding String
    for (int j = imageHeight - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            Color pixelColor = Color{0, 0, 0};
            for (int s = 0; s < samplePerPixels; ++s) {
                // 一个像素取samplePerPixels条打在这个像素内的光线
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
    // 防止浮点数近似为0
    if (world.Hit(r, 0.001, infinity, rec)) {
        Ray scattered;
        Color attenuation;
        if (rec.matPtr->Scatter(r, rec, attenuation, scattered)) {
            // 递归，多次反射
            return attenuation * RayColor(scattered, world, depth - 1);
        }
        return Color(0, 0, 0);
    }
    Vec3 unitDir = UnitVector(r.Direction());
    auto t = 0.5 * (unitDir.Y() + 1.0);
    return (1.0 - t) * Color(1.0, 1.0, 1.0) + t * Color(0.5, 0.7, 1.0);
}

HittableList RandomScene() {
    HittableList world;

    auto groundMaterial = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.add(
        std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, groundMaterial));
    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto chooseMat = RandomDouble();
            Point3 center(a + 0.9 * RandomDouble(), 0.2,
                          b + 0.9 * RandomDouble());

            if ((center - Point3(4, 0.2, 0)).Length() > 0.9) {
                std::shared_ptr<Material> sphereMaterial;

                if (chooseMat < 0.8) {
                    // diffuse
                    auto albedo = Color::Random() * Color::Random();
                    sphereMaterial = std::make_shared<Lambertian>(albedo);
                    world.add(
                        std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                } else if (chooseMat < 0.95) {
                    // metal
                    auto albedo = Color::Random(0.5, 1);
                    auto fuzz = RandomDouble(0, 0.5);
                    sphereMaterial = std::make_shared<Metal>(albedo, fuzz);
                    world.add(
                        std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                } else {
                    // glass
                    sphereMaterial = std::make_shared<Dielectric>(1.5);
                    world.add(
                        std::make_shared<Sphere>(center, 0.2, sphereMaterial));
                }
            }
        }
    }

    auto material1 = std::make_shared<Dielectric>(1.5);
    world.add(std::make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = std::make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(std::make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = std::make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(std::make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}
