#include "PerlinNoise.h"
#include <cstdio>

PerlinNoise::PerlinNoise()
{
    persistence = 0;
    frequency = 0;
    amplitude  = 0;
    octaves = 0;
}

PerlinNoise::PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves)
{
    persistence = _persistence;
    frequency = _frequency;
    amplitude  = _amplitude;
    octaves = _octaves;
}

int PerlinNoise::GetHeight(int i, int j) const
{
    double t = 0.0f;
    double _amplitude = 1;
    double freq = frequency;

    for(int k = 0; k < octaves; k++)
    {
        t += Noise(j * freq, i * freq) * _amplitude;
        _amplitude *= persistence;
        freq *= 2;
    }
    //printf("%lf\n", t);
    int result = (t + 1.0) > 0 ? (int)(t + 1.5) : 0;
    return result;
}

double PerlinNoise::Noise(int x, int y) const
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
}
