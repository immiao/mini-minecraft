#ifndef PERLINNOISE_H
#define PERLINNOISE_H


class PerlinNoise
{
public:

  // Constructor
    PerlinNoise();
    PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves);

    // Get
    double Persistence() const { return persistence; }
    double Frequency()   const { return frequency;   }
    double Amplitude()   const { return amplitude;   }
    int    Octaves()     const { return octaves;     }

    // Set
    void Set(double _persistence, double _frequency, double _amplitude, int _octaves);

    void SetPersistence(double _persistence) { persistence = _persistence; }
    void SetFrequency(  double _frequency)   { frequency = _frequency;     }
    void SetAmplitude(  double _amplitude)   { amplitude = _amplitude;     }
    void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
    int GetHeight(int i, int j) const;

private:


    double Noise(int x, int y) const;

    double persistence, frequency, amplitude;
    int octaves;
};

#endif // PERLINNOISE_H
