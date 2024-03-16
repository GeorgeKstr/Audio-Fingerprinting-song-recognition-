///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#include "record.h"

bool recordAudio(size_t seconds, size_t samplerate, short* &pcm, size_t &n){
    WAVEFORMATEX wfx={};
    wfx.wFormatTag=WAVE_FORMAT_PCM;
    wfx.nChannels=1;
    wfx.nSamplesPerSec=samplerate;
    wfx.wBitsPerSample=16;
    wfx.nBlockAlign=wfx.wBitsPerSample*wfx.nChannels/8;
    wfx.nAvgBytesPerSec=wfx.nBlockAlign*wfx.nSamplesPerSec;
    HWAVEIN wi;
    waveInOpen( &wi, WAVE_MAPPER, &wfx, 0L, 0L, CALLBACK_NULL | WAVE_FORMAT_DIRECT );
    char buffers[2][samplerate*wfx.nChannels*2/2];
    WAVEHDR headers[2]={{},{}};
    char *pcm8 = new char[samplerate*seconds*wfx.nChannels*2];
    for(int i=0; i<2; ++i)
    {
        headers[i].lpData = buffers[i];
        headers[i].dwBufferLength=samplerate*wfx.nChannels*2/2;
        waveInPrepareHeader(wi, &headers[i], sizeof(headers[i]));
        waveInAddBuffer(wi, &headers[i], sizeof(headers[i]));
    }
    n=0;
    size_t i;
    waveInStart(wi);
    while(n<seconds*samplerate){
        for(auto& h : headers){
            if(h.dwFlags & WHDR_DONE){
                for(i=0; i<h.dwBufferLength; i++)
                    pcm8[n*2+i]=h.lpData[i];
                h.dwFlags=0;
                n+=h.dwBytesRecorded/2;
                h.dwBytesRecorded=0;
                waveInPrepareHeader(wi, &h, sizeof(h));
                waveInAddBuffer(wi, &h, sizeof(h));
            }
        }
    }
    waveInStop(wi);
    for(auto& h : headers)waveInUnprepareHeader(wi, &h, sizeof(h));
    waveInClose(wi);

    pcm = reinterpret_cast<short*>(pcm8);
    return true;
}
