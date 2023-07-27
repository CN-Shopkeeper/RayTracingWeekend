#pragma once

#include <iostream>

#include "vec3.hpp"

void WriteColor(std::ostream& out, Color pixelColor, int samplesPerPixel) {
    auto r = pixelColor.X();
    auto g = pixelColor.Y();
    auto b = pixelColor.Z();

    // Divide the color by the number of samples and gamma-correct for
    // gamma=2.0.
    auto scale = 1.0 / samplesPerPixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * Clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(255.999 * Clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(255.999 * Clamp(b, 0.0, 0.999)) << '\n';
}