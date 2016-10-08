#ifndef __WAVE_AUDIOTRACK_H
#define __WAVE_AUDIOTRACK_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef callback_t
typedef void (*callback_t)(void* thisz, unsigned char* buffer,int size,int* RealSize);
#endif

int AudioTrackCreate(int rate, int format, int channel,int MinWriteSize,callback_t cbf,void* user);

void AudioTrackStart();

void AudioTrackReset();

void AudioTrackWrite(const void* buffer, int length);

void AudioTrackPause();

void AudioTrackFlush();

void AudioTrackStop();

void AudioTrackDestroy();

#ifdef __cplusplus
}
#endif
#endif
