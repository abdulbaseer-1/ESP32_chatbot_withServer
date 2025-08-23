#ifndef MIC_I2S_H
#define MIC_I2S_H 
 
#include <stdint.h>

#define FRAME_SAMPLES 480
#define OUT_SAMPLES   160

extern int32_t raw_buf[FRAME_SAMPLES];
extern int16_t in_buf[FRAME_SAMPLES];
extern int16_t out_buf[OUT_SAMPLES];


void i2s_mic_init(void); 
int i2s_mic_read(void* buffer, int len);
// void convert_32bit_to_16bit(int32_t *src, int16_t *dst, int sample_count); 
// void fir_filter(float input, float *output); 
// void downsample_48_to_16(int16_t *in_samples, int in_len, int16_t *out_samples, int *out_len); 
// void downsample_48_to_16_no_filter(int16_t *in_samples, int in_len, int16_t *out_samples, int *out_len);
#endif
