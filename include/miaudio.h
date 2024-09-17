#ifndef MIAUDIO
#define MIAUDIO
#include <miniaudio/miniaudio.h>
#include <fft.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <assert.h>
#include <sys/wait.h>
#include <complex.h>
#include <math.h>
// const double PI = acos(-1);
typedef struct {
  ma_decoder decoder;
  ma_uint64  position;
  ma_uint64  srate;
  float 	   *samples;
  ma_uint64  totalFrames;
  double     duration;
  uint32_t   framecount;
  TComplex *fft_; // Needs to be loaded each frame.
  size_t spec_sz;
  float *spectrum;     // Needs to be loaded each frame.
} MiAudio;

void   init_audio(MiAudio *audio);
double get_frames_as_seconds(ma_decoder *decoder, ma_uint64 frame);

#endif // MIAUDIO
