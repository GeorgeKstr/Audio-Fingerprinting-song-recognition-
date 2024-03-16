///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#ifndef DECODE_H_INCLUDED
#define DECODE_H_INCLUDED
#include<string>
#include<sstream>
using namespace std;
extern "C"{
    #include<mpg123.h>
    #include<out123.h>
}

bool decodeMp3(const char *file, size_t rate, string &trackname, short* &out, size_t &out_n);

#endif // DECODE_H_INCLUDED
