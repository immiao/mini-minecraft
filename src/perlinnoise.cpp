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

int PerlinNoise::GetHeight(int i, int j)
{
    int centerx = -50;
    int centery = -50;

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
    t = (t + 1) * 0.5;
//    if (t > 1)
    //printf("%lf\n", t);
    if (i >= centerx && j >= centery)
    {
        t=pow(t,3.3);
        t *= scale;
    }
    else if (i >= centerx && j < centery)
    {
        t=pow(t, 6);
        t *= scale;
    }
    else if (i < centerx && j >= centery)
    {
        t=pow(t,3.3);
        t *= 50;
        if (t < 5)
            t = 0;
    }
    else
    {
        t=pow(t,3.3);
        t *= 300;
    }
    return t;
}


double PerlinNoise::SCurve(double x)
{
    return 6 * x * x * x * x * x - 15 * x * x * x * x + 10 * x * x * x;
}

double PerlinNoise::Lerp(double p0, double p1, double x)
{
    return p0 * (1 - x) + p1 * x;
}

double PerlinNoise::GetValue(int i, int j, int period)
{
    int i0 = i / period * period;
    if (i < 0) i0 -= period;
    int i1 = i0 + period;
    double horizontal_blend = (i - i0) / (double)period;

    int j0 = j / period * period;
    if (j < 0) j0 -= period;
    int j1 = j0 + period;
    double vertical_blend = (j - j0) / (double)period;

    double p00 = Noise(i0, j0);
    double p10 = Noise(i1, j0);
    double p01 = Noise(i0, j1);
    double p11 = Noise(i1, j1);

    double p0 = Lerp(p00, p10, SCurve(horizontal_blend));
    double p1 = Lerp(p01, p11, SCurve(horizontal_blend));
//if (p0 > 1)
//    printf("LERP:%d %d %f %f %f %f\n", i, i0, period, p0, SCurve(horizontal_blend), horizontal_blend);
    return Lerp(p0, p1, SCurve(vertical_blend));
}

double PerlinNoise::GetValue(double i, double j, int period)
{
    int i0 = (int)i / period * period;
    if (i < 0) i0 -= period;
    int i1 = i0 + period;
    double horizontal_blend = (i - i0) / (double)period;

    int j0 = (int)j / period * period;
    if (j < 0) j0 -= period;
    int j1 = j0 + period;
    double vertical_blend = (j - j0) / (double)period;

    double p00 = Noise(i0, j0);
    double p10 = Noise(i1, j0);
    double p01 = Noise(i0, j1);
    double p11 = Noise(i1, j1);

    double p0 = Lerp(p00, p10, SCurve(horizontal_blend));
    double p1 = Lerp(p01, p11, SCurve(horizontal_blend));
//if (p0 > 1)
//    printf("LERP:%d %d %f %f %f %f\n", i, i0, period, p0, SCurve(horizontal_blend), horizontal_blend);
    return Lerp(p0, p1, SCurve(vertical_blend));
}

double PerlinNoise::GetPerlinNoise(double i, double j)
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
    t /= octaves;
    return t;
}
double PerlinNoise::GetPerlinNoise(int x, int y, int z)
{
    double t = 0.0f;
    double _amplitude = 1;
    double freq = frequency;

    for(int k = 0; k < octaves; k++)
    {
        t += GetValue(x, y, z, 1 << (octaves - 1 - k)) * _amplitude;
        _amplitude *= persistence;
        freq *= 2;
    }

    t /= octaves;
    //printf("%f\n", t);
    return t;
}

double PerlinNoise::GetValue(int i, int j, int k, int period)
{
    int i0 = i / period * period;
    if (i < 0) i0 -= period;
    int i1 = i0 + period;
    double i_blend = (i - i0) / (double)period;

    int j0 = j / period * period;
    if (j < 0) j0 -= period;
    int j1 = j0 + period;
    double j_blend = (j - j0) / (double)period;

    int k0 = k / period * period;
    if (k < 0) k0 -= period;
    int k1 = k0 + period;
    double k_blend = (k - k0) / (double)period;

    double p000 = Noise(i0, j0, k0);
    double p100 = Noise(i1, j0, k0);
    double p010 = Noise(i0, j1, k0);
    double p110 = Noise(i1, j1, k0);
    double p001 = Noise(i0, j0, k1);
    double p101 = Noise(i1, j0, k1);
    double p011 = Noise(i0, j1, k1);
    double p111 = Noise(i1, j1, k1);

//    double p00 = p000 * (1 - i_blend) + p100 * i_blend;
//    double p10 = p010 * (1 - i_blend) + p110 * i_blend;
//    double p01 = p001 * (1 - i_blend) + p101 * i_blend;
//    double p11 = p011 * (1 - i_blend) + p111 * i_blend;

//    double p0 = p00 * (1 - j_blend) + p10 * j_blend;
//    double p1 = p01 * (1 - j_blend) + p11 * j_blend;

    double p00 = Lerp(p000, p100, SCurve(i_blend));
    double p10 = Lerp(p010, p110, SCurve(i_blend));
    double p01 = Lerp(p001, p101, SCurve(i_blend));
    double p11 = Lerp(p011, p111, SCurve(i_blend));

    double p0 = Lerp(p00, p10, SCurve(j_blend));
    double p1 = Lerp(p01, p11, SCurve(j_blend));
    return Lerp(p0, p1, SCurve(k_blend));
}

double PerlinNoise::Noise(int x, int y)
{
    int n = x + y * 57;
    n = (n << 13) ^ n;
    int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
}

double PerlinNoise::Noise(int x, int y, int z)
{
    int n = x + y * 57 + z * 197;
    n = (n << 13) ^ n;
    int t = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0 - double(t) * 0.931322574615478515625e-9;/// 1073741824.0);
}
