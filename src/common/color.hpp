#pragma once

#include <iostream>

#include "vec3.hpp"

void WriteColor(std::ostream& out, Color pixelColor, int samplesPerPixel) {
    auto r = pixelColor.X();
    auto g = pixelColor.Y();
    auto b = pixelColor.Z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samplesPerPixel;
    r *= scale;
    g *= scale;
    b *= scale;
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * Clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(255.999 * Clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(255.999 * Clamp(b, 0.0, 0.999)) << '\n';
}