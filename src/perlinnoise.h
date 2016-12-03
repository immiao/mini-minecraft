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
    double GetValue(double i, double j, int period);
    double GetValue(int x, int y, int z, int period);
    double GetPerlinNoise(double i, double j);
    double GetPerlinNoise(int i, int j, int k);
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
