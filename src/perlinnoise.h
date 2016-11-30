#ifndef PERLINNOISE_H
#define PERLINNOISE_H


class PerlinNoise
{
public:

  // Constructor
    PerlinNoise();
    PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _scale);

    int GetHeight(int i, int j) const;
    double GetValue(int i, int j, int period) const;

private:

    double Noise(int x, int y) const;

    double persistence, frequency, amplitude;
    int octaves;
    int scale;
};

#endif // PERLINNOISE_H
