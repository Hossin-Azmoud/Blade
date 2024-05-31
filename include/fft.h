#ifndef FFT_H
#define FFT_H
#include <assert.h>
#include <complex.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void fft(float *in, size_t stride, float complex *out, size_t n);
void apply_hamming_window(float *data, size_t length);
void get_spectrum(float complex *in, float *out, size_t length, size_t buckets, size_t SampleRate);
float expo(float x); 
#endif // FFT_H
