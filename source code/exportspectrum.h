#ifndef EXPORTSPECTRUM_H_INCLUDED
#define EXPORTSPECTRUM_H_INCLUDED
#include<cmath>
#include <png.hpp>
using namespace png;
void setImagePalette(image<index_pixel> &img);
void exportspectrum(double **data, size_t x, size_t y, float percent, const char *file);

#endif // EXPORTSPECTRUM_H_INCLUDED
