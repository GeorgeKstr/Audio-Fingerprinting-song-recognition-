///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#ifndef FOURIER_H_INCLUDED
#define FOURIER_H_INCLUDED
#include<math.h>
#include<cmath>
#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
extern "C" {
    #include <fftw3.h>
}

void fourierTransform(short *data, size_t n, size_t slicesize, float step, double** &out, size_t &slices, size_t &outslicesize);

#endif // FOURIER_H_INCLUDED
