#include "fft.h"
#include "miaudio.h"
#include <miplayer.h>

double get_frames_as_seconds(ma_decoder *decoder, ma_uint64 frame) {
  return (double)((double)frame / decoder->outputSampleRate);
}

void init_audio(MiAudio *audio) {

  // Preparing the decoder for the audio.
  // Identify how much frames are in the audio file so we can load them all at
  // once.
  ma_decoder_get_length_in_pcm_frames(&audio->decoder, &audio->totalFrames);

  // Load the frames to the audio->samples array.
  size_t N = (audio->totalFrames * audio->decoder.outputChannels);
  audio->samples = (float *)malloc(sizeof(float) * N);

  // Zero the samples
  ma_uint64 read = 0;

  // Load the samples, put (audio->totalFrames) of samples into audio->samples
  // then store how much was read in the read variable.
  ma_decoder_read_pcm_frames(&audio->decoder, audio->samples,
                             audio->totalFrames, &read);
  audio->Input    = malloc(sizeof(TComplexF) * FFT_SIZE);
  audio->fft_     = malloc(sizeof(TComplexF) * FFT_SIZE);
  audio->fft_abs  = malloc(sizeof(float) * FFT_SIZE);
  audio->spectrum = malloc(sizeof(float) *
                           FFT_SIZE); // Will be populated and processed using fft.

  audio->spec_sz  = 0;
  // sanity check haha
  assert(read == audio->totalFrames);
  audio->duration = get_frames_as_seconds(
      &audio->decoder, audio->totalFrames * audio->decoder.outputChannels);
}
