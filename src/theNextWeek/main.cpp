#include <iostream>

#include "aarec.hpp"
#include "bvh.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "moving_sphere.hpp"
#include "perlin.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"
#include "texture.hpp"

Color RayColor(const Ray& r, const Color& background, const Hittable& world,
               int depth);
HittableList RandomScene();
HittableList TwoSpheres();
HittableList TwoPerlinSpheres();
HittableList Earth();
HittableList SimpleLight();
HittableList CornellBox();

int main() {
    //  Image

    auto aspectRatio = 16.0 / 9.0;
    int imageWidth = 400;
    int samplePerPixels = 100;
    const int maxDepth = 50;

    // World

    HittableList world;
    Point3 lookFrom;
    Point3 lookAt;
    auto vfov = 40.0;
    auto aperture = 0.0;
    Color background{0, 0, 0};

    switch (0) {
        case 1:
            world = RandomScene();
            background = Color{0.7, 0.8, 1.0};
            lookFrom = Point3(13, 2, 3);
            lookAt = Point3(0, 0, 0);
            vfov = 20.0;
            aperture = 0.1;
            break;

        case 2:
            world = TwoSpheres();
            background = Color{0.7, 0.8, 1.0};
            lookFrom = Point3(13, 2, 3);
            lookAt = Point3(0, 0, 0);
            vfov = 20.0;

            break;

        case 3:
            world = TwoPerlinSpheres();
            background = Color{0.7, 0.8, 1.0};
            lookFrom = Point3(13, 2, 3);
            lookAt = Point3(0, 0, 0);
            vfov = 20.0;
            break;

        case 4:
            world = Earth();
            background = Color{0.7, 0.8, 1.0};
            lookFrom = Point3(13, 2, 3);
            lookAt = Point3(0, 0, 0);
            vfov = 20.0;
            break;

        case 5:
            world = SimpleLight();
            samplePerPixels = 400;
            background = Color{0.0, 0.0, 0.0};
            lookFrom = Point3(26, 3, 6);
            lookAt = Point3(0, 2, 0);
            vfov = 20.0;
            break;

        default:
        case 6:
            world = CornellBox();
            aspectRatio = 1.0;
            imageWidth = 600;
            samplePerPixels = 200;
            background = Color(0, 0, 0);
            lookFrom = Point3(278, 278, -800);
            lookAt = Point3(278, 278, 0);
            vfov = 40.0;
            break;
    }

    // Camera

    Vec3 vup = Vec3(0, 1, 0);
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    auto distToFocus = 10.0;
    Camera camera(lookFrom, lookAt, vup, vfov, aspectRatio, aperture,
                  distToFocus, 0.0, 1.0);

    // Render

    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    // ! 直接重定向会导致输出的文件是带有BOM的UTF-16的文件
    // ! .\theNextWeek.exe | set-content imageTheNextWeek.ppm -encoding String
    for (int j = imageHeight - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            Color pixelColor = Color{0, 0, 0};
            for (int s = 0; s < samplePerPixels; ++s) {
                // 一个像素取samplePerPixels条打在这个像素内的光线
                auto u = (i + RandomDouble()) / (imageWidth - 1);
                auto v = (j + RandomDouble()) / (imageHeight - 1);
                Ray r = camera.GetRay(u, v);
                pixelColor += RayColor(r, background, world, maxDepth);
            }
            WriteColor(std::cout, pixelColor, samplePerPixels);
        }
        std::cout << "\n";
    }
    std::cerr << "\nDone.\n";
    return 0;
}

Color RayColor(const Ray& r, const Color& background, const Hittable& world,
               int depth) {
    HitRecord rec;
    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) return Color(0, 0, 0);

    // If the ray hits nothing, return the background color.
    // 防止浮点数近似为0
    if (!world.Hit(r, 0.001, infinity, rec)) {
        return background;
    }

    Ray scattered;
    Color attenuation;
    Color emitted = rec.matPtr->Emitted(rec.u, rec.v, rec.p);

    if (!rec.matPtr->Scatter(r, rec, attenuation, scattered)) {
        return emitted;
    }

    // 递归，多次反射
    return emitted +
           attenuation * RayColor(scattered, background, world, depth - 1);
}

HittableList RandomScene() {
    HittableList world;

    auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1),
                                                    Color(0.9, 0.9, 0.9));
    world.add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000,
                                       std::make_shared<Lambertian>(checker)));
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
                    auto center2 = center + Vec3(0, RandomDouble(0, 0.5), 0);
                    world.add(std::make_shared<MovingSphere>(
                        center, center2, 0.0, 1.0, 0.2, sphereMaterial));
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

HittableList TwoSpheres() {
    HittableList objects;

    auto checker = std::make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1),
                                                    Color(0.9, 0.9, 0.9));
    objects.add(std::make_shared<Sphere>(
        Point3(0, -10, 0), 10, std::make_shared<Lambertian>(checker)));
    objects.add(std::make_shared<Sphere>(
        Point3(0, 10, 0), 10, std::make_shared<Lambertian>(checker)));
    return objects;
}

HittableList TwoPerlinSpheres() {
    HittableList objects;

    auto pertext = std::make_shared<NoiseTexture>(4);
    objects.add(std::make_shared<Sphere>(
        Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(
        Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));
    return objects;
}

HittableList Earth() {
    auto earthTexture =
        std::make_shared<ImageTexture>("resources/earthmap.jpg");
    auto earthSurface = std::make_shared<Lambertian>(earthTexture);
    auto globe = std::make_shared<Sphere>(Point3{0, 0, 0}, 2, earthSurface);

    return HittableList(globe);
}

HittableList SimpleLight() {
    HittableList objects;

    auto pertext = std::make_shared<NoiseTexture>(4);
    objects.add(std::make_shared<Sphere>(
        Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(
        Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    // ? 颜色值超出范围
    auto diffLight = std::make_shared<DiffuseLight>(Color{4, 4, 4});
    objects.add(std::make_shared<XYRect>(3, 5, 1, 3, -2, diffLight));
    return objects;
}

HittableList CornellBox() {
    HittableList objects;
    auto red = std::make_shared<Lambertian>(Color{0.65, 0.05, 0.05});
    auto white = std::make_shared<Lambertian>(Color{0.73, 0.73, 0.73});
    auto green = std::make_shared<Lambertian>(Color{.12, .45, .15});
    auto light = std::make_shared<DiffuseLight>(Color{15, 15, 15});

    objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 555, green));
    objects.add(std::make_shared<YZRect>(0, 555, 0, 555, 0, red));
    objects.add(std::make_shared<XZRect>(213, 343, 227, 332, 554, light));
    objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 0, white));
    objects.add(std::make_shared<XZRect>(0, 555, 0, 555, 555, white));
    objects.add(std::make_shared<XYRect>(0, 555, 0, 555, 555, white));

    return objects;
}
