#include "PerlinNoise.h"
#include <cstdio>
#include <cmath>

PerlinNoise::PerlinNoise()
{
    persistence = 0;
    frequency = 0;
    amplitude  = 0;
    octaves = 0;
}

PerlinNoise::PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _scale)
{
    persistence = _persistence;
    frequency = _frequency;
    amplitude  = _amplitude;
    octaves = _octaves;
    scale = _scale;
}

int PerlinNoise::GetHeight(int i, int j) const
{
    double t = 0.0f;
    double _amplitude = 1;
    double freq = frequency;

    for(int k = 0; k < octaves; k++)
    {
        t += GetValue(i, j, 1 << (octaves - 1 - k)) * _amplitude;
        _amplitude *= persistence;
        freq *= 2;
    }
    //printf("%lf\n", t);
    t /= octaves;

    //printf("%lf\n", t);
    t = (t + 1) * 0.5;
    t=pow(t,3.3);
    t *= scale;
    return t;
}
double PerlinNoise::Test(int i, int j)
{
    double t = 0.0f;
    double _amplitude = 1;
    int freq = frequency;

    for(int k = 0; k < octaves; k++)
    {
        t += GetValue(i, j, 1 << (octaves - 1 - k)) * _amplitude;
        _amplitude *= persistence;
        freq *= 2;
    }
    t /= octaves;

    return (t + 1) * 0.5f;
}

double PerlinNoise::GetValue(int i, int j, int period) const
{
    int i0 = i / period * period;
    int i1 = i0 + period;
    double horizontal_blend = (i - i0) / (double)period;

    int j0 = j / period * period;
    int j1 = j0 + period;
    double vertical_blend = (j - j0) / (double)period;

    double p00 = Noise(i0, j0);
    double p10 = Noise(i1, j0);
    double p01 = Noise(i0, j1);
    double p11 = Noise(i1, j1);

    double p0 = p00 * (1 - horizontal_blend) + p10 * horizontal_blend;
    double p1 = p01 * (1 - horizontal_blend) + p11 * horizontal_blend;

    return p0 * (1 - vertical_blend) + p1 * vertical_blend;
}

double PerlinNoise::Noise(int x, int y) const
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
}
