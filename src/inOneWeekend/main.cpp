#include <iostream>

#include "color.hpp"
#include "vec3.hpp"

int main() {
    //  Image
    const int imageWidth = 256;
    const int imageHeight = 256;

    // Render
    std::cout << "P3\n" << imageWidth << ' ' << imageHeight << "\n255\n";

    // ! 直接重定向会导致输出的文件是带有BOM的UTF-16的文件
    // ! .\inOneWeekend.exe | set-content image.ppm -encoding String
    for (int j = imageHeight - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < imageWidth; ++i) {
            Color pixelColor(double(i) / (imageWidth - 1),
                             double(j) / (imageHeight - 1), 0.25);
            WriteColor(std::cout, pixelColor);
        }
        std::cout << "\n";
    }
    std::cerr << "\nDone.\n";
    return 0;
}