#pragma once
#include <algorithm>
#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double RandomDouble() {
    // Returns a random real in [0,1).
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double RandomDouble(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * RandomDouble();
}

inline int RandomInt(int min, int max) {
    // Returns a random integer in [min,max].
    return static_cast<int>(RandomDouble(min, max + 1));
}

inline double Clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

// Common Headers
#include "ray.hpp"
#include "vec3.hpp"