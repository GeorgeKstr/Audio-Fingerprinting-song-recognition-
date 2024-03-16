///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#ifndef FINGERPRINT_H_INCLUDED
#define FINGERPRINT_H_INCLUDED
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <cstdlib>
using namespace std;
unsigned int getFrequencyIndex(unsigned int freq);
void produceFingerprint(double **data, size_t slices, size_t fuz_factor, unsigned long* &out);
#endif // FINGERPRINT_H_INCLUDED
