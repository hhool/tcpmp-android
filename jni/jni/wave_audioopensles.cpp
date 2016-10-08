#include "../common/common.h"
#include <pthread.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "wave_audioopensles.h"


/* audio */
// engine interfaces
static SLObjectItf iEngineObject = NULL;
static SLEngineItf iEngineEngine;

// output mix interfaces
static SLObjectItf iOutputMixObject = NULL;
static SLEnvironmentalReverbItf iOutputMixEnvironmentalReverb = NULL;

// buffer queue player interfaces
static SLObjectItf iPlayerObject = NULL;
static SLPlayItf iPlayerPlay;
static SLAndroidSimpleBufferQueueItf iPlayerBufferQueue;
static SLMuteSoloItf iPlayerMuteSolo;
static SLVolumeItf iPlayerVolume;


static bool_t		ibFlushed = 1;
static callback_t 	iCallBack = NULL;
static void* 		iUser = NULL;
static unsigned char iBuffer[16384];
static int iBufferSize = 16384;

// for OpenSLES
/*
*/
static void wavout_cbf_sles(SLAndroidSimpleBufferQueueItf bq, void *user)
{
    SLresult result;
    
	int RealSize = 0;

	memset(iBuffer,0,iBufferSize);
	
	iCallBack(user,iBuffer,iBufferSize,&RealSize);
			
	if(RealSize >0 )	
	{
       result = (*bq)->Enqueue(bq, (uint8_t*)iBuffer, RealSize);
				
       if (result != SL_RESULT_SUCCESS)
	   {
           DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles:: (*bq)->Enqueue(bq, (short *)(is->audio_buf),  %d", result);
       }
	   else
	   {
          
       }  
	}
	else
	{
		DEBUG_MSG(DEBUG_AUDIO,T("wave_opensles::wavout_cbf_sles:no data"));
		result = (*bq)->Enqueue(bq, (uint8_t*)iBuffer, iBufferSize);
				
		if (result != SL_RESULT_SUCCESS)
		{
		   DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles:: (*bq)->Enqueue(bq, (short *)(is->audio_buf),  %d", result);
		}
		else
		{
		  
		}  
	}
}


int AudioOpenSLESCreate(int rate, int format, int channel,int MinWriteSize,callback_t cbf,void* user)
{
    SLresult result;
    SLint32 flags;
    SLDataFormat_PCM format_pcm;
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq;

    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESCreate()");

    if(iEngineObject && iEngineEngine)
	{
        return 0;
    }

    AudioOpenSLESDestroy();


    switch(format)
	{
        case 0: // SAMPLE_FMT_U8
            format = SL_PCMSAMPLEFORMAT_FIXED_8;
            DEBUG_MSG(DEBUG_AUDIO,T("wave_opensles::AudioOpenSLESCreate SL_PCMSAMPLEFORMAT_FIXED_8"));
            break;
        case 1: // SAMPLE_FMT_S16
            format = SL_PCMSAMPLEFORMAT_FIXED_16;
            DEBUG_MSG(DEBUG_AUDIO,T("wave_opensles::AudioOpenSLESCreate SL_PCMSAMPLEFORMAT_FIXED_16"));
            break;
        default:
            DEBUG_MSG1(DEBUG_AUDIO,T("wave_opensles::audio track format %d is not supported"), format);
            return -1;
    }

    // create engine
    result = slCreateEngine(&iEngineObject, 0, NULL, 0, NULL, NULL);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::slCreateEngine %d",result);
        return -1;
    }

    // realize the engine
    result = (*iEngineObject)->Realize(iEngineObject, SL_BOOLEAN_FALSE);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::Realize %d",result);
        return -1;
    }
    

    // get the engine interface, which is needed in order to create other objects
    result = (*iEngineObject)->GetInterface(iEngineObject, SL_IID_ENGINE, &iEngineEngine);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::GetInterface %d",result);
        return -1;
    }

    // create output mix, with environmental reverb specified as a non-required interface
    const SLInterfaceID ids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean req[1] = {SL_BOOLEAN_FALSE};
    result = (*iEngineEngine)->CreateOutputMix(iEngineEngine, &iOutputMixObject, 1, ids, req);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::CreateOutputMix %d",result);
        return -1;
    }


    // realize the output mix
    result = (*iOutputMixObject)->Realize(iOutputMixObject, SL_BOOLEAN_FALSE);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::iOutputMixObject Realize %d",result);
        return -1;
    }

    // configure audio source
    loc_bufq.locatorType = SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE;
    loc_bufq.numBuffers = 2;
    format_pcm.formatType        = SL_DATAFORMAT_PCM;
    format_pcm.numChannels     = channel >=2 ? 2 : 1;
    format_pcm.samplesPerSec   = rate*1000;
    format_pcm.bitsPerSample    = format;
    format_pcm.containerSize     = format;
    format_pcm.channelMask      = channel >=2 ? SL_SPEAKER_FRONT_LEFT|SL_SPEAKER_FRONT_RIGHT : SL_SPEAKER_FRONT_CENTER;
    format_pcm.endianness        = SL_BYTEORDER_LITTLEENDIAN;       
    SLDataSource audioSrc = {&loc_bufq, &format_pcm};
    
    // configure audio sink
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, iOutputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    // create audio player
    const SLInterfaceID ids2[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND,
            /*SL_IID_MUTESOLO,*/ SL_IID_VOLUME};
    const SLboolean req2[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE,
            /*SL_BOOLEAN_TRUE,*/ SL_BOOLEAN_TRUE};
    DEBUG_MSG7(DEBUG_AUDIO,"wave_opensles::CreateAudioPlayer \tformatType[%d]\tnumChannels[%d]\tsamplesPerSec[%d]\tbitsPerSample[%d]\tcontainerSize[%d]\tchannelMask[%d]\tendianness[%d]",
        format_pcm.formatType,format_pcm.numChannels,format_pcm.samplesPerSec,
        format_pcm.bitsPerSample,format_pcm.containerSize,format_pcm.channelMask,
        format_pcm.endianness); 


    result = (*iEngineEngine)->CreateAudioPlayer(iEngineEngine, &iPlayerObject, &audioSrc, &audioSnk,
            3, ids2, req2);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::CreateAudioPlayer %d",result);
        return -1;
    }
    // realize the player
    result = (*iPlayerObject)->Realize(iPlayerObject, SL_BOOLEAN_FALSE);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::(*iPlayerObject)->Realize(iPlayerObject, SL_BOOLEAN_FALSE) %d",result);
        return -1;
    }
    // get the play interface
    result = (*iPlayerObject)->GetInterface(iPlayerObject, SL_IID_PLAY, &iPlayerPlay);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::(*iPlayerObject)->GetInterface(iPlayerObject, SL_IID_PLAY, &iPlayerPlay); %d",result);
        return -1;
    }
    // get the buffer queue interface
    result = (*iPlayerObject)->GetInterface(iPlayerObject, SL_IID_BUFFERQUEUE,&iPlayerBufferQueue);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::(*iPlayerObject)->GetInterface(iPlayerObject, SL_IID_BUFFERQUEUE ,&iPlayerBufferQueue) %d",result);
        return -1;
    }
    // register callback on the buffer queue
    result = (*iPlayerBufferQueue)->RegisterCallback(iPlayerBufferQueue, wavout_cbf_sles, user);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::(*iPlayerBufferQueue)->RegisterCallback(iPlayerBufferQueue, egl_audio_callback, is) %d",result);
        return -1;
    }

    // get the volume interface
    result = (*iPlayerObject)->GetInterface(iPlayerObject, SL_IID_VOLUME, &iPlayerVolume);
    if(result)
	{
        DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles::(*iPlayerObject)->GetInterface(iPlayerObject, SL_IID_VOLUME, &iPlayerVolume) %d",result);
        return -1;
    }

	iCallBack = cbf;
	iUser	  = user;

    return 0;
}



void AudioOpenSLESStart()
{
    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESStart()");
	
	SLresult result;
    
	int RealSize = 0;

	if(ibFlushed)
	{
		memset(iBuffer,0,iBufferSize);
		
		iCallBack(iUser,iBuffer,iBufferSize,&RealSize);
			
		if(RealSize >0 )	
		{
			result = (*iPlayerBufferQueue)->Enqueue(iPlayerBufferQueue, (uint8_t*)iBuffer, RealSize);
					
			if (result != SL_RESULT_SUCCESS)
			{
			   	DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles:: (*bq)->Enqueue(bq, (short *)(is->audio_buf),  %d", result);
			}
			else
			{
				result = (*iPlayerBufferQueue)->Enqueue(iPlayerBufferQueue, (uint8_t*)iBuffer, iBufferSize);

				if (result != SL_RESULT_SUCCESS)
				{
				   DEBUG_MSG1(DEBUG_AUDIO,"wave_opensles:: (*bq)->Enqueue(bq, (short *)(is->audio_buf),  %d", result);
				}
				else
				{
				  
				}  
			}  
		}
	}
	
    if(iPlayerPlay)
	{
        result = (*iPlayerPlay)->SetPlayState(iPlayerPlay, SL_PLAYSTATE_PLAYING);
        if(result)
		{
            DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESStart() error");
        }    
    }
}

void AudioOpenSLESWrite(const void* buffer, int length)
{

}

void AudioOpenSLESReset()
{
    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESReset()");

}


void AudioOpenSLESPause()
{
    SLresult result;

    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESPause()");
    if (iPlayerPlay)
	{
        result = (*iPlayerPlay)->SetPlayState(iPlayerPlay, SL_PLAYSTATE_PAUSED);
        if(result)
		{
            DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESPause() error");
        }
    }
}

void AudioOpenSLESFlush()
{
    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESFlush()");
	if(!!iPlayerBufferQueue)
	{
		(*iPlayerBufferQueue)->Clear(iPlayerBufferQueue);
	}

	ibFlushed = 1;
}

void AudioOpenSLESStop()
{
    SLresult result;

    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESStop()");
    if ( iPlayerPlay )
	{
        result = (*iPlayerPlay)->SetPlayState(iPlayerPlay, SL_PLAYSTATE_STOPPED);
        if(result)
		{
            DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESPause() error");
        }  
    }
}

void AudioOpenSLESDestroy()
{
    
    DEBUG_MSG(DEBUG_AUDIO,"wave_opensles::AudioOpenSLESDestroy()");
    // destroy buffer queue audio player object, and invalidate all associated interfaces
    if (iPlayerObject != NULL)
	{
        (*iPlayerObject)->Destroy(iPlayerObject);
        iPlayerObject = NULL;
        iPlayerPlay = NULL;
        iPlayerBufferQueue = NULL;
        iPlayerMuteSolo = NULL;
        iPlayerVolume = NULL;
    }


    // destroy output mix object, and invalidate all associated interfaces
    if (iOutputMixObject != NULL)
	{
        (*iOutputMixObject)->Destroy(iOutputMixObject);
        iOutputMixObject = NULL;
        iOutputMixEnvironmentalReverb = NULL;
    }
    
    
    // destroy engine object, and invalidate all associated interfaces
    if (iEngineObject != NULL) 
	{
        (*iEngineObject)->Destroy(iEngineObject);
        iEngineObject = NULL;
        iEngineEngine = NULL;
    }
    
}
