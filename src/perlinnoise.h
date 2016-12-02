#ifndef PERLINNOISE_H
#define PERLINNOISE_H


class PerlinNoise
{
public:

  // Constructor
    PerlinNoise();
    PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _scale);

    int GetHeight(int i, int j);
    double GetValue(int i, int j, int period);
    double GetDirection(int x, int y, int z);
    double GetValue(int x, int y, int z, int period);
    double SCurve(double x);
    double Lerp(double p0, double p1, double x);

private:

    double Noise(int x, int y);
    double Noise(int x, int y, int z);
    double persistence, frequency, amplitude;
    int octaves;
    int scale;
};

#endif // PERLINNOISE_H
