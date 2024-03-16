///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#include "fingerprint.h"
#define upper_limit 300
#define lower_limit 20

unsigned int getFrequencyIndex(unsigned int freq){
    unsigned int ranges[]={60, 100, 140, 180, upper_limit+1};
    unsigned int i=0;
    while(ranges[i]<freq)i++;
    return i;
}

void produceFingerprint(double **fft_data, size_t slices, size_t fuz_factor, unsigned long* &out){
    out = new unsigned long[slices];
    double highscores[5];
    unsigned int freqs[5];
    size_t freq;
    unsigned int index;
    for(size_t slice=0; slice<slices; slice++){
        highscores[0]=highscores[1]=highscores[2]=highscores[3]=highscores[4]=0;
        freqs[0]=freqs[1]=freqs[2]=freqs[3]=freqs[4]=0;
        for(freq=lower_limit;freq<=upper_limit;freq++){
            index = getFrequencyIndex(freq);
            if(fft_data[slice][freq]>highscores[index]){
                highscores[index]=fft_data[slice][freq];
                freqs[index]=freq;
            }
        }
        ostringstream oss;
        for(int i=0; i<5; i++){
            freqs[i]-=freqs[i]%fuz_factor;
            oss<<freqs[i];
        }
        istringstream iss(oss.str());
        out[slice]=atol(oss.str().c_str());
    }
}
