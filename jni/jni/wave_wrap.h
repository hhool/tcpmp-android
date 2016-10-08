#ifndef __WAVE_WRAP_H
#define __WAVE_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef callback_t
typedef void (*callback_t)(void* thisz, unsigned char* buffer,int size,int* RealSize);
#endif

typedef int 	(*AUDIOCREATE)	(int rate, int format, int channel,int MinWriteSize,callback_t cbf,void* user);
typedef void	(*AUDIOSTART) 	();
typedef void	(*AUDIORESET) 	();
typedef void	(*AUDIOWRITE) 	(const void* buffer, int length);
typedef void	(*AUDIOPAUSE) 	();
typedef void	(*AUDIOFLUSH) 	();
typedef void	(*AUDIOSTOP) 	();
typedef void	(*AUDIODESTROY) ();

typedef struct audio_output_device
{
	AUDIOCREATE 	AudioFunCreate;
	AUDIOSTART		AudioFuncStart;
	AUDIORESET  	AudioFuncReset;
	AUDIOWRITE  	AudioFuncWrite;
	AUDIOPAUSE  	AudioFuncPause;
	AUDIOFLUSH		AudioFuncFlush;
	AUDIOSTOP		AudioFuncStop;
	AUDIODESTROY	AudioFuncDestroy;
}audio_output_device;


#ifdef __cplusplus
}
#endif
#endif
