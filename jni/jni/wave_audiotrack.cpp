#include "../common/common.h"
#include <media/AudioSystem.h>
#include <media/AudioTrack.h>
#if PLATFORM >= 14
#include <system/audio.h>
#endif
using namespace android;
static 		AudioTrack* Audio = 0;
static int 	iMinWriteSize = 16384;
#include "wave_audiotrack.h"

static callback_t iCallBack;

static void AudioTrackCbf(int event, void* user, void *info)
{
	if (event == AudioTrack::EVENT_MORE_DATA)
	{
		// set size to 0 to signal we're not using the callback to write more data
		AudioTrack::Buffer* Buffer = (AudioTrack::Buffer*)info;

		unsigned char *rawBuffer = (unsigned char *)Buffer->raw;

		if(!Buffer->size)
		{
			DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::wave_wrap::AudioTrackCbf liblossless callback: audiotrack requested zero bytes"));
		}
		else
		{
			int RealSize = 0;

			iCallBack(user,rawBuffer,Buffer->size,&RealSize);

			if(RealSize>=0)
			{
				Buffer->size = RealSize;
			}
			else
			{
				Buffer->size = 0;
			}
			DEBUG_MSG1(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackCbf Buffer->size %d"),Buffer->size);
		}
	}
}
int AudioTrackCreate(int rate, int format, int channel,int MinWriteSize,callback_t cbf,void* user)
{
	if(MinWriteSize>0)
		iMinWriteSize=MinWriteSize;

	switch(format)
    {
        case 0: // SAMPLE_FMT_U8
        {
#if PLATFORM < 14
        	format = AudioSystem::PCM_8_BIT;
#elif PLATFORM >= 14
		format = AUDIO_FORMAT_PCM_8_BIT;
#endif
        	DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::AudioTrackCreate PCM_8_BIT"));
            break;
        }
        case 1: // SAMPLE_FMT_S16
        {
#if PLATFORM < 14
        	format = AudioSystem::PCM_16_BIT;
#elif PLATFORM >= 14
			format = AUDIO_FORMAT_PCM_16_BIT;
#endif
        	DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::AudioTrackCreate PCM_16_BIT"));
            break;
        }
        default:
        {
        	DEBUG_MSG1(DEBUG_AUDIO,T("audio track format %d is not supported"), format);
            if (Audio)
            {
                delete Audio;
                Audio = 0;
            }
            return -1;
        }
    }
    if (Audio)
    {
        if (rate != Audio->getSampleRate() || format != Audio->format() || channel != Audio->channelCount())
        {
        	DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackCreate delete Audio;"));
            delete Audio;
            Audio = 0;
        }
    }
    if (!Audio)
    {
        Audio = new AudioTrack();
        // try cupcake
        status_t status;
#if  PLATFORM  < 14
        status = Audio->set(
            AudioSystem::MUSIC,
            rate,
            format,
            channel,
            iMinWriteSize,
            0,
            AudioTrackCbf,
            user);

        DEBUG_MSG5(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackCreate ret %d,rate %d format %d channel %d iMinWriteSize %d"),\
        		status,rate,format,channel,iMinWriteSize);
        // try eclair or higher
        if (status == BAD_VALUE)
        {
        	int chl = (channel== 2) ? 12 : 4;
            status = Audio->set(
                AudioSystem::MUSIC,
                rate,
                format,
                chl, // CHANNEL_OUT_STEREO
                iMinWriteSize,
                0,
				AudioTrackCbf,
				user);
            DEBUG_MSG4(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackCreate try eclair ret %d,rate %d format %d channel %d"),\
            		status,rate,format,channel);
        }
#else
		 Audio = new AudioTrack(
            AUDIO_STREAM_MUSIC,
            rate,
            format,
            channel>=2 
                ?  AUDIO_CHANNEL_OUT_STEREO
                : AUDIO_CHANNEL_OUT_MONO, 
            iMinWriteSize,
            0,
            AudioTrackCbf,
            user);
#endif
        if (status == NO_ERROR)
        {
        	DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackCreate"));
        	iCallBack = cbf;
        }
        else
        {
        	DEBUG_MSG1(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackCreate delete Audio %d;"),status);
            delete Audio;
            Audio = 0;
        }
    }
    return Audio ? 0 : -1;
}
void AudioTrackStart()
{
	if(Audio)
	{
		DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackStart"));

		Audio->start();

		DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackStart end"));
	}
}
void AudioTrackReset()
{
	if(Audio)
	{
		DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::AudioTrackReset,Audio->flush();,Audio->Stop();"));
		Audio->flush();
		Audio->stop();
	}
}
void AudioTrackWrite(const void* buffer, int length)
{
    int l = 0;
    int WriteSize = iMinWriteSize;

    while (l < length)
    {
    	if(length-l<iMinWriteSize)
    		WriteSize = length -l;

    	DEBUG_MSG2(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackWrite Write size %d,length %d"),l,length);
        l += Audio->write((char*)(buffer) + l, WriteSize);
        DEBUG_MSG2(DEBUG_AUDIO,T("android_waveout::AudioTrackWrite Write size %d,length %d"),l,length);
    }
}
void AudioTrackPause()
{
	if(Audio)
	{
		DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackPause"));
		Audio->pause();
	}
}
void AudioTrackFlush()
{
	if(Audio)
	{
		DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackFlush"));
		Audio->flush();
	}
}
void AudioTrackStop()
{
	if(Audio)
	{
		DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackPause"));
		Audio->stop();
		Audio->flush();
	}
}
void AudioTrackDestroy()
{
    if (Audio)
    {
    	DEBUG_MSG(DEBUG_AUDIO,T("android_waveout::wave_wrap::AudioTrackDestroy"));
        delete Audio;
        Audio = 0;
    }
}
