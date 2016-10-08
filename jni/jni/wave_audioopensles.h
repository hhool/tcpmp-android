#ifndef __WAVE_AUDIOOPENSLES_H
#define __WAVE_AUDIOOPENSLES_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef callback_t
typedef void (*callback_t)(void* thisz, unsigned char* buffer,int size,int* RealSize);
#endif

int AudioOpenSLESCreate(int rate, int format, int channel,int MinWriteSize,callback_t cbf,void* user);

void AudioOpenSLESStart();

void AudioOpenSLESReset();

void AudioOpenSLESWrite(const void* buffer, int length);

void AudioOpenSLESPause();

void AudioOpenSLESFlush();

void AudioOpenSLESStop();

void AudioOpenSLESDestroy();

#ifdef __cplusplus
}
#endif
#endif
