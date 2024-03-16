///author George Kiriakidis
///libraries used: mpg123, fftw3, libpng, png++
#include "fourier.h"

float window(int n, int N){
    //return 0.54-0.46*cos((2*M_PI*n)/(N-1));
    return 0.5*(1-cos(2*M_PI*n)/(N-1));
}
void fourierTransform(short *data, size_t n, size_t slicesize, float step, double** &out, size_t &slices, size_t &outslicesize){
    outslicesize = slicesize/2 + 1;
    double *fft_in = (double*) fftw_malloc(slicesize*sizeof(double));
    fftw_complex *fft_out = (fftw_complex*) fftw_malloc(outslicesize*sizeof(fftw_complex));
    fftw_plan plan = fftw_plan_dft_r2c_1d(slicesize, fft_in, fft_out, FFTW_ESTIMATE);
    size_t i;
    slices=0;
    for(i=0;i+slicesize-1<n; i+=size_t(slicesize*step))slices++;
    out = new double*[slices];
    float real, imag;
    size_t slice=0;
    for(size_t start=0; start+slicesize-1<n; start+=size_t(step*slicesize)){
        out[slice]=new double[outslicesize];
        for(i=0; i<slicesize; i++)
            fft_in[i]=(double)data[start+i];//window(i, outslicesize);
        fftw_execute(plan);

        for(i=0; i<outslicesize; i++){
            real=fft_out[i][0];
            imag=fft_out[i][1];
            out[slice][i]=10*log10(real*real+imag*imag);
        }
        slice++;
    }

    fftw_destroy_plan(plan);
    fftw_free(fft_in);
    fftw_free(fft_out);
    fftw_cleanup();
}
