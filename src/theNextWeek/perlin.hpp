#pragma once

#include "rtweekend.hpp"

class Perlin {
   private:
    static const int pointCount = 256;
    Vec3* ranvec;
    int* permX;
    int* permY;
    int* permZ;

    static int* perlinGeneratePerm() {
        auto p = new int[pointCount];
        for (int i = 0; i < Perlin::pointCount; i++) {
            p[i] = i;
        }
        permute(p, pointCount);
        return p;
    }

    static void permute(int* p, int n) {
        for (int i = n - 1; i > 0; i--) {
            int target = RandomInt(0, i);
            std::swap(p[i], p[target]);
        }
    }

    static double perlinInterp(Vec3 c[2][2][2], double u, double v, double w) {
        auto uu = u * u * (3 - 2 * u);
        auto vv = v * v * (3 - 2 * v);
        auto ww = w * w * (3 - 2 * w);
        auto accum = 0.0;
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++) {
                    Vec3 weight_v{u - i, v - j, w - k};
                    accum += (i * uu + (1 - i) * (1 - uu)) *
                             (j * vv + (1 - j) * (1 - vv)) *
                             (k * ww + (1 - k) * (1 - ww)) *
                             Dot(c[i][j][k], weight_v);
                }
        return accum;
    }

   public:
    Perlin() {
        ranvec = new Vec3[pointCount];
        for (int i = 0; i < pointCount; ++i) {
            ranvec[i] = UnitVector(Vec3::Random(-1, 1));
        }

        permX = perlinGeneratePerm();
        permY = perlinGeneratePerm();
        permZ = perlinGeneratePerm();
    }

    ~Perlin() {
        delete[] ranvec;
        delete[] permX;
        delete[] permY;
        delete[] permZ;
    }

    double Noise(const Point3& p) const {
        auto u = p.X() - floor(p.X());
        auto v = p.Y() - floor(p.Y());
        auto w = p.Z() - floor(p.Z());

        auto i = static_cast<int>(floor(p.X()));
        auto j = static_cast<int>(floor(p.Y()));
        auto k = static_cast<int>(floor(p.Z()));
        Vec3 c[2][2][2];
        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] =
                        ranvec[permX[(i + di) & 255] ^ permY[(j + dj) & 255] ^
                               permZ[(k + dk) & 255]];
        return perlinInterp(c, u, v, w);
    }

    double Turb(const Point3& p, int depth = 7) const {
        auto accum = 0.0;
        auto tempP = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {
            accum += weight * Noise(tempP);
            weight *= 0.5;
            tempP *= 2;
        }
        return fabs(accum);
    }
};