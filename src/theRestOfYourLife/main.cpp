#include <iostream>

#include "aarec.hpp"
#include "box.hpp"
#include "bvh.hpp"
#include "camera.hpp"
#include "color.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "perlin.hpp"
#include "rtweekend.hpp"
#include "sphere.hpp"
#include "texture.hpp"

Color RayColor(const Ray& r, const Color& background, const Hittable& world,
               int depth);
HittableList CornellBox();

int main() {
    //  Image

    auto aspectRatio = 1.0 / 1.0;
    int imageWidth = 600;
    const int imageHeight = static_cast<int>(imageWidth / aspectRatio);
    int samplePerPixels = 100;
    const int maxDepth = 50;

    // World

    HittableList world = CornellBox();
    Color background{0, 0, 0};

    // Camera

    Point3 lookFrom{278, 278, -800};
    Point3 lookAt{278, 278, 0};
    Vec3 vup = Vec3(0, 1, 0);
    auto distToFocus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;
    auto time0 = 0.0;
    auto time1 = 1.0;
    Camera camera(lookFrom, lookAt, vup, vfov, aspectRatio, aperture,
                  distToFocus, time0, time1);

    // Render

    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    // ! 直接重定向会导致输出的文件是带有BOM的UTF-16的文件
    // ! .\theRestOfYourLife.exe | set-content image.ppm -encoding String
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

    std::shared_ptr<Hittable> box1 =
        std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<RotateY>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3{256, 0, 295});
    objects.add(box1);

    std::shared_ptr<Hittable> box2 =
        std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<RotateY>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3{130, 0, 65});
    objects.add(box2);
    return objects;
}
