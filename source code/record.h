///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#ifndef RECORD_H_INCLUDED
#define RECORD_H_INCLUDED
#include<windows.h>
#include<mmsystem.h>

    bool recordAudio(size_t seconds, size_t samplerate, short* &pcm, size_t &n);

#endif // RECORD_H_INCLUDED
