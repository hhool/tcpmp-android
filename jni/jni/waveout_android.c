/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * $Id: waveout_android.c 543 2010-11-10 22:06:24Z picard $
 *
 * The Media Player
 * Copyright (c) 2010- hhool
 *
 ****************************************************************************/

#include "../common/common.h"


#if defined(TARGET_ANDROID)

#include <pthread.h>
#include <math.h>

#include "wave_wrap.h"
#ifdef USESLES
#include "wave_audioopensles.h"
#else
#include "wave_audiotrack.h"
#endif


#define BUFFER_MUSIC		1*TICKSPERSEC
#define BUFFER_VIDEO		2*TICKSPERSEC
#define BENCH_SIZE			32

#ifndef WAVE_FORMAT_PCM
#define WAVE_FORMAT_PCM	1
#endif
/*
 *  extended waveform format structure used for all non-PCM formats. this
 *  structure is common to all non-PCM formats.
 */
typedef struct waveformat
{
	unsigned short wFormatTag; /* format type */
	unsigned short nChannels; /* number of channels (i.e. mono, stereo...) */
	unsigned int nSamplesPerSec; /* sample rate */
	unsigned int nAvgBytesPerSec; /* for buffer estimation */
	unsigned short nBlockAlign; /* block size of data */
	unsigned short wBitsPerSample; /* number of bits per sample of mono data */
	unsigned short cbSize; /* the count in bytes of the size of */
	/* extra information (after cbSize) */
} waveformat;


struct waveout_android;

typedef struct wavebuffer
{
	planes Planes;
	struct wavebuffer* Next; //next in chain
	struct wavebuffer* GlobalNext;
	tick_t RefTime;
	tick_t EstRefTime;
	int Bytes;
	int LeftBytes;
	block Block;

} wavebuffer;

typedef struct waveout_android
{
	node Node;
	node Timer;
	pin Pin;
	packetformat Input;
	packetformat Output;
	packetprocess Process;

	audio_output_device Device;

	wavebuffer* Buffers; // global chain
	wavebuffer* FreeFirst;
	wavebuffer* FreeLast;
	wavebuffer* WFirst;
	wavebuffer* WLast;
	wavebuffer* CurBuffer;

	int BufferLength; // one buffer length (waveout_android format)
	tick_t BufferScaledTime; // scaled time of one waveout_android buffer (BufferLength)
	int BufferScaledAdjust; // waveout_android bytes to scaled time convert (12bit fixed point)

	int Total; // source format
	int Dropped; // dropped packets

	int Bytes; // output format
	int FillPos;
	int Skip;
	wavebuffer* FillFirst;
	wavebuffer* FillLast;
	wavebuffer** Pausing;
	tick_t FillLastTime;

	bool_t   bOpen;
	bool_t 	 bStart;
	pthread_mutex_t	 Section;
	void* PCM;
	int PCMSpeed;
	int BufferLimit;
	int BufferLimitFull;
	tick_t Tick;
	int TimeRef;
	int Waiting;	// number of waiting buffers in WaveOut
	int Used;		// number of used buffers (waiting or in fill chain)

	bool_t Play;
	fraction Speed;
	fraction SpeedTime;
	int AdjustedRate;

	bool_t Dither;
	bool_t BufferMode;
	int Stereo;
	int Quality;

	bool_t ForcePriority;
	bool_t SoftwareVolume;
	bool_t MonoVol;
	bool_t Mute;
	int PreAmp;
	int VolumeDev;	// backup value when mute is turned on
	int VolumeSoft;
	int VolumeSoftLog;
	int VolumeRamp;

	waveformat Format;

	int BenchWait[BENCH_SIZE];
	int BenchSum[BENCH_SIZE];
	int BenchCurrSum;
	int BenchAvg;
	int BenchAdj;
	size_t BenchAvgLimit;
	int BenchSpeedAvg;
	int BenchWaitPos;

} waveout_android;

#define WAVEOUT(p) ((waveout_android*)((char*)(p)-OFS(waveout_android,Timer)))

static void Pause(waveout_android* p);
static void Write(waveout_android* p, tick_t CurrTime);

static int SetVolumeSoft(waveout_android* p,int v,bool_t m)
{
	int OldVolumeSoftLog = p->VolumeSoftLog;
	bool_t OldSkip = p->Mute || p->VolumeSoft==0;

	p->VolumeSoft = v;
	p->Mute = m;

	if (p->SoftwareVolume || p->PreAmp)
	{
		if (p->SoftwareVolume && p->bOpen && (p->Mute || p->VolumeSoft==0)!=OldSkip)
		{
			if (!OldSkip)
				Pause(p);
			else
			if (p->Play)
			{
				// adjust tick so packets without RefTime won't mess up timing
				pthread_mutex_lock(&p->Section);
				p->Tick += Scale(GetTimeTick()-p->TimeRef,p->SpeedTime.Num,p->SpeedTime.Den);
				p->TimeRef = GetTimeTick();
				pthread_mutex_unlock(&p->Section);
			}
		}

		v += p->PreAmp;
		if (v<-40) v=-40;

		p->VolumeSoftLog = (int)(pow(10,(50+v)/62.3));
		if (p->VolumeSoftLog < 3)
			p->VolumeSoftLog = 3;
	}
	else
		p->VolumeSoftLog = 256;

	if (p->bOpen)
	{
		int Adjust = ScaleRound(p->VolumeSoftLog,256,OldVolumeSoftLog);
		if (Adjust != 256)
		{
			wavebuffer* List;
			pthread_mutex_lock(&p->Section);
			for (List=p->Buffers;List;List=List->GlobalNext)
				VolumeMul(Adjust,(void*)List->Block.Ptr,p->BufferLength,&p->Output.Format.Audio);
			pthread_mutex_unlock(&p->Section);
		}
	}

	return ERR_NONE;
}

static int GetVolume(waveout_android* p)
{
	if (!p->SoftwareVolume)
	{
		uint32_t Value;
//		if (waveOutGetVolume(NULL,&Value) == MMSYSERR_NOERROR)
//		{
//			if (p->MonoVol)
//			{
//				Value &= 0xFFFF;
//				Value |= Value << 16;
//			}
//			if (Value)
//				p->Mute = 0;
//			if (!p->Mute)
//				p->VolumeDev = ((LOWORD(Value)+HIWORD(Value)+600)*100) / (0xFFFF*2);
//		}
		return p->VolumeDev;
	}
	return p->VolumeSoft;
}

static tick_t Time(waveout_android* p)
{
	if (p->Speed.Num==0)
		return TIME_BENCH;

	if (p->Play)
	{
		tick_t t;
		pthread_mutex_lock(&p->Section);
		t = p->Tick + Scale(GetTimeTick()-p->TimeRef,p->SpeedTime.Num,p->SpeedTime.Den);
		DEBUG_MSG4(DEBUG_AUDIO,T("waveout_android:Time:t %d,real %d,p->SpeedTime.Num %d,p->SpeedTime.Den %d"),t,Scale(t,p->SpeedTime.Den,p->SpeedTime.Num),p->SpeedTime.Num,p->SpeedTime.Den);
		pthread_mutex_unlock(&p->Section);
		return t;
	}

	return p->Tick;
}

static int TimerGet(void* pt, int No, void* Data, int Size)
{
	waveout_android* p = WAVEOUT(pt);
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case TIMER_PLAY: GETVALUE(p->Play,bool_t); break;
	case TIMER_SPEED: GETVALUE(p->Speed,fraction); break;
	case TIMER_TIME: GETVALUE(Time(p),tick_t); break;
	}
	return Result;
}

static int Get(waveout_android* p, int No, void* Data, int Size)
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case OUT_INPUT: GETVALUE(p->Pin,pin); break;
	case OUT_INPUT|PIN_FORMAT: GETVALUE(p->Input,packetformat); break;
	case OUT_INPUT|PIN_PROCESS: GETVALUE(p->Process,packetprocess); break;
	case OUT_OUTPUT|PIN_FORMAT: GETVALUE(p->Output,packetformat); break;
	case OUT_TOTAL:GETVALUE(p->Total,int); break;
	case OUT_DROPPED:GETVALUE(p->Dropped,int); break;
	case AOUT_VOLUME: GETVALUE(GetVolume(p),int); break;
	case AOUT_MUTE: GETVALUE(p->Mute,bool_t); break;
	case AOUT_PREAMP: GETVALUE(p->PreAmp,int); break;
	case AOUT_STEREO: GETVALUE(p->Stereo,int); break;
	case AOUT_MODE: GETVALUE(p->BufferMode,bool_t); break;
	case AOUT_QUALITY: GETVALUE(p->Quality,int); break;
	case AOUT_TIMER: GETVALUE(&p->Timer,node*); break;
	}
	return Result;
}

static void UpdateBenchAvg(waveout_android* p)
{
	int n;

	p->BenchAvg += 4;
	p->BenchAdj = (24*16) / p->BenchAvg;
	p->BenchSpeedAvg = SPEED_ONE/(p->BenchAvg*BENCH_SIZE);

	n = p->Input.Format.Audio.Bits >> 3;
	if (!(p->Input.Format.Audio.Flags & PCM_PLANES))
		n *= p->Input.Format.Audio.Channels;
	n = p->Output.Format.Audio.SampleRate;
	if (n>0)
		p->BenchAvgLimit = (n * p->BenchAvg) / 160;
	else
		p->BenchAvgLimit = MAX_INT;

	p->BenchCurrSum += 2 * BENCH_SIZE;
	for (n=0;n<BENCH_SIZE;++n)
	{
		p->BenchSum[n] += 2 * BENCH_SIZE;
		p->BenchWait[n] += 2;
	}
}

static void ReleaseBuffer(waveout_android* p,wavebuffer* Buffer,bool_t UpdateTick)
{
	pthread_mutex_lock(&p->Section);

	if (UpdateTick)
	{
		tick_t Old;
		int Time = GetTimeTick();

		Old = p->Tick + Scale(Time-p->TimeRef,p->SpeedTime.Num,p->SpeedTime.Den);

		if (Buffer->RefTime >= 0)
			p->Tick = Buffer->RefTime + p->BufferScaledTime;
		else
			p->Tick += p->BufferScaledTime;
		p->TimeRef = Time;

		DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::ReleaseBuffer:new:WaveOutTime: Old %d p->Tick %d"),Old,p->Tick);

		// if difference is little then just adjust (because GetTimeTick() is more linear)
		if (abs(Old - p->Tick) < TICKSPERSEC/2)
			p->Tick = Old + ((p->Tick - Old) >> 2);
	}

	Buffer->Next = NULL;
	if (p->FreeLast)
		p->FreeLast->Next = Buffer;
	else
		p->FreeFirst = Buffer;
	p->FreeLast = Buffer;
	p->Used--;
	pthread_mutex_unlock(&p->Section);
}

static void Reset(waveout_android* p)
{
	int n;

	// release buffers already sended to device
	DEBUG_MSG4(DEBUG_AUDIO,T("waveout_android::Reset p->Tick %d,while (p->WFirst 0x%x) p->WLast 0x%x,p->Waiting %d"),p->Tick,p->WFirst,p->WLast,p->Waiting);
	if (p->bOpen)
	{
		tick_t OldTick = p->Tick;
		p->Device.AudioFuncReset();
		p->Tick = OldTick;
	}
	p->bStart = 0;

	// release W chain
	while (p->WFirst)
	{
		wavebuffer* Buffer = p->WFirst;
		p->WFirst = Buffer->Next;
		ReleaseBuffer(p,Buffer,0);
		pthread_mutex_lock(&p->Section);
		p->Waiting--;
		pthread_mutex_unlock(&p->Section);
	}
	p->WLast = p->WFirst;
	if(p->CurBuffer)
	{
		ReleaseBuffer(p, p->CurBuffer, 0);
		p->CurBuffer = NULL;
	}
	DEBUG_MSG3(DEBUG_AUDIO,T("waveout_android::Reset while (p->WFirst 0x%x) p->WLast 0x%x,p->Waiting %d"),p->WFirst,p->WLast,p->Waiting);

	if (p->FillLast && p->FillLast->RefTime >= 0)
		p->FillLastTime = p->FillLast->RefTime;

	DEBUG_MSG4(DEBUG_AUDIO,T("waveout_android::Reset p->FillLastTime %d,while (p->WFirst 0x%x) p->WLast 0x%x,p->Waiting %d"),p->FillLastTime,p->WFirst,p->WLast,p->Waiting);
	// release fill chain
	while (p->FillFirst)
	{
		wavebuffer* Buffer = p->FillFirst;
		p->FillFirst = Buffer->Next;
		ReleaseBuffer(p,Buffer,0);
	}

	p->FillLast = NULL;
	p->FillPos = p->BufferLength;
	p->Skip = 0;
	p->Bytes = 0;
	p->BufferLimit = p->BufferLimitFull;

	p->BenchAvg = 16-4;
	UpdateBenchAvg(p);

	p->BenchWaitPos = 0;
	p->BenchCurrSum = p->BenchAvg * BENCH_SIZE;
	for (n=0;n<BENCH_SIZE;++n)
	{
		p->BenchSum[n] = p->BenchCurrSum;
		p->BenchWait[n] = p->BenchAvg;
	}

	PCMReset(p->PCM); // init dither and subsample position
}

static wavebuffer* GetBuffer(waveout_android* p)
{
	wavebuffer* Buffer;

	// try to find a free buffer

	pthread_mutex_lock(&p->Section);
	Buffer = p->FreeFirst;
	if (Buffer)
	{
		p->FreeFirst = Buffer->Next;
		if (Buffer == p->FreeLast)
			p->FreeLast = NULL;
	}
	if (!Buffer)
	{
		block Block;
		if (AllocBlock(p->BufferLength,&Block,p->Used>=20,HEAP_DYNAMIC))
		{
			Buffer = (wavebuffer*)malloc(sizeof(wavebuffer));
			if (!Buffer)
				FreeBlock(&Block);
		}

		if (Buffer)
		{
			Buffer->Block = Block;
			Buffer->Planes[0] = (uint8_t*)Block.Ptr;
			Buffer->Next = NULL;

			Buffer->GlobalNext = p->Buffers;
			p->Buffers = Buffer;

		}
		else
		{
			p->BufferLimitFull = p->Used;
			if (p->BufferLimit > p->Used)
				p->BufferLimit = p->Used;
			else
			if (p->BufferLimit > 4)
				p->BufferLimit--;
		}

	}

	if (Buffer)
	{
		p->Used++;
		Buffer->RefTime = -1;
		Buffer->Next =NULL;
	}

	pthread_mutex_unlock(&p->Section);
	return Buffer;
}

static void Write(waveout_android* p, tick_t CurrTime)
{
	DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::Write Begin CurrTime %d p->Play %d"),CurrTime,p->Play);

	if (p->Play)
	{
		while (p->FillFirst != p->FillLast)
		{
			wavebuffer* Buffer = p->FillFirst;

			DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::Write ING"));

			if (!p->Waiting&&CurrTime >= 0 && Buffer->EstRefTime >= 0 && Buffer->EstRefTime > CurrTime + SHOWAHEAD)
			{
				DEBUG_MSG3(DEBUG_AUDIO,T("waveout_android::Write break p->Waiting %d,CurrTime %d,Buffer->EstRefTime %d"),p->Waiting,CurrTime,Buffer->EstRefTime);
				break;
			}

			p->FillFirst = Buffer->Next;

			if (p->SoftwareVolume && (p->Mute || p->VolumeSoft==0))
				ReleaseBuffer(p,Buffer,0);
			else
			{
				pthread_mutex_lock(&p->Section);
				DEBUG_MSG5(DEBUG_AUDIO,T("waveout_android::AudioWrite::Save Buffer 0x%x,Len %d,p->Waiting %d p->WFirst 0x%x,p->WLast 0x%x"),Buffer,p->BufferLength,p->Waiting,p->WFirst,p->WLast);
				Buffer->Next = NULL;
				if (p->WLast)
					p->WLast->Next = Buffer;
				else
					p->WFirst = Buffer;
				p->WLast = Buffer;

				p->Waiting++;
				pthread_mutex_unlock(&p->Section);
				
				DEBUG_MSG5(DEBUG_AUDIO,T("waveout_android::AudioWrite::record Buffer 0x%x,Len %d,p->Waiting %d p->WFirst 0x%x,p->WLast 0x%x"),Buffer,p->BufferLength,p->Waiting,p->WFirst,p->WLast);
				if (!p->bStart)
				{
					DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::AudioWrite:if (!p->bStart) p->Waiting %d p->Used %d"),p->Waiting,p->Used);
					p->Device.AudioFuncStart();
					p->bStart = 1;
				}
			}
		}
	}
	DEBUG_MSG1(DEBUG_AUDIO,T("waveout_android::Write End CurrTime %d"),CurrTime);
}

static int Send(waveout_android* p, const constplanes Planes, int Length, tick_t RefTime, tick_t CurrTime, int Speed)
{
	wavebuffer* Buffer;
	int DstLength;
	int SrcLength;
	planes DstPlanes;
	constplanes SrcPlanes;

	p->Total += Length;

	SrcPlanes[0] = Planes[0];
	SrcPlanes[1] = Planes[1];

	if (p->Skip > 0)
	{
		SrcLength = min(p->Skip,Length);
		SrcPlanes[0] = (uint8_t*)SrcPlanes[0] + SrcLength;
		SrcPlanes[1] = (uint8_t*)SrcPlanes[1] + SrcLength;
		Length -= SrcLength;
		p->Skip -= SrcLength;
	}

	while (Length > 0)
	{
		if (p->FillPos >= p->BufferLength)
		{
			// allocate new buffer
			Buffer = GetBuffer(p);
			if (!Buffer)
				break;

			if (p->FillLast)
			{
				wavebuffer* Last = p->FillLast;
				if (Last->RefTime>=0)
					p->FillLastTime = Last->EstRefTime = Last->RefTime;
				else
				{
					if (p->FillLastTime>=0)
						p->FillLastTime += p->BufferScaledTime;
					Last->EstRefTime = p->FillLastTime;
				}
				Last->Next = Buffer;
			}
			else
				p->FillFirst = Buffer;
			p->FillLast = Buffer;
			p->FillPos = 0;
			Buffer->Bytes = p->Bytes;
		}
		else
			Buffer = p->FillLast;

		if (RefTime >= 0)
		{
			Buffer->RefTime = RefTime - ((p->FillPos * p->BufferScaledAdjust) >> 12);
			if (Buffer->RefTime < 0)
				Buffer->RefTime = 0;
			RefTime = TIME_UNKNOWN;
		}

		SrcLength = Length;
		DstLength = p->BufferLength - p->FillPos;
		DstPlanes[0] = (uint8_t*)Buffer->Block.Ptr + p->FillPos;

		PCMConvert(p->PCM,DstPlanes,SrcPlanes,&DstLength,&SrcLength,Speed,p->VolumeSoftLog);

		if (p->VolumeRamp < RAMPLIMIT)
			p->VolumeRamp = VolumeRamp(p->VolumeRamp,DstPlanes[0],DstLength,&p->Output.Format.Audio);

		p->Bytes += DstLength;
		p->FillPos += DstLength;

		SrcPlanes[0] = (uint8_t*)SrcPlanes[0] + SrcLength;
		SrcPlanes[1] = (uint8_t*)SrcPlanes[1] + SrcLength;
		Length -= SrcLength;

		if (!SrcLength)
			break;
	}

	if (Length && p->Input.Format.Audio.BlockAlign>0)
		p->Skip = p->Input.Format.Audio.BlockAlign - Length % p->Input.Format.Audio.BlockAlign;

	Write(p,CurrTime);
	return ERR_NONE;
}
static void wavout_cbf(void* thisz, unsigned char* buffer,int size,int* RealSize)
{
	waveout_android* p = (waveout_android*)thisz;

	DEBUG_MSG5(DEBUG_AUDIO,T("waveout_android::wavout_cbf: buffer 0x%x,size %d,p->WFirst 0x%x p->WLast 0x%x,p->Waiting %d"),buffer,size,p->WFirst,p->WLast,p->Waiting);

	if(p->CurBuffer&&p->CurBuffer->LeftBytes<=0)
	{
		//释放上次渲染完毕的Buffer
		ReleaseBuffer(p, p->CurBuffer, 1);
		
		pthread_mutex_lock(&p->Section);
		p->Waiting--;
		pthread_mutex_unlock(&p->Section);
		
		p->CurBuffer = NULL;
	}

	pthread_mutex_lock(&p->Section);

	if (p->WFirst&&p->CurBuffer==NULL)
	{
		p->CurBuffer = p->WFirst;

		p->WFirst = p->CurBuffer->Next;
		
		p->CurBuffer->Next = NULL;

		p->CurBuffer->LeftBytes = p->BufferLength;

		DEBUG_MSG5(DEBUG_AUDIO,T("waveout_android::wavout_cbf:Pop;p->CurBuffer 0x%x,p->CurBuffer->LeftBytes %d, p->WFirst 0x%x p->WLast 0x%x,p->Waiting %d"),\
								p->CurBuffer,p->CurBuffer->LeftBytes,p->WFirst,p->WLast,p->Waiting);
	}
	
	if(p->WFirst==NULL)
	{
		p->WLast = p->WFirst;
	}
	if(p->CurBuffer)
	{
		if(p->CurBuffer->LeftBytes<=size)
		{
			memcpy(buffer,(uint8_t*)p->CurBuffer->Block.Ptr+p->BufferLength-p->CurBuffer->LeftBytes,p->CurBuffer->LeftBytes);

			*RealSize = p->CurBuffer->LeftBytes;

			p->CurBuffer->LeftBytes = 0;
		}
		else if(p->CurBuffer->LeftBytes>size)
		{
			*RealSize = size;

			memcpy(buffer,(uint8_t*)p->CurBuffer->Block.Ptr+p->BufferLength-p->CurBuffer->LeftBytes,size);

			p->CurBuffer->LeftBytes-=size;
		}
	}
	else 
	{
		*RealSize = 0;
		DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::wavout_cbf: is read nothing p->Waiting %d,RealSize %d"),p->Waiting,*RealSize);
	}
	if(p->CurBuffer)
	{
		DEBUG_MSG5(DEBUG_AUDIO,T("waveout_android::wavout_cbf:end p->CurBuffer->LeftBytes %d,RealSize %d,p->WFirst 0x%x p->WLast 0x%x,p->Waiting %d"),p->CurBuffer->LeftBytes,*RealSize,p->WFirst,p->WLast,p->Waiting);
	}

	pthread_mutex_unlock(&p->Section);

}
static int UpdatePCM(waveout_android* p,const audio* InputFormat)
{
	p->SpeedTime = p->Speed;
	p->SpeedTime.Num *= TICKSPERSEC;
	p->SpeedTime.Den *= GetTimeFreq();

	p->BufferScaledTime = Scale(TICKSPERSEC,p->Speed.Num*p->BufferLength,p->Speed.Den*p->Format.nAvgBytesPerSec);
	if (p->BufferLength)
		p->BufferScaledAdjust = (p->BufferScaledTime*4096) / p->BufferLength;
	else
		p->BufferScaledAdjust = 0;
	if (!p->Speed.Num)
		p->PCMSpeed = SPEED_ONE;
	else
		p->PCMSpeed = Scale(SPEED_ONE,p->Speed.Num,p->Speed.Den);
	p->AdjustedRate = Scale(p->Output.Format.Audio.SampleRate,p->Speed.Den,p->Speed.Num);

	PCMRelease(p->PCM);
	p->PCM = PCMCreate(&p->Output.Format.Audio,InputFormat,p->Dither,p->SoftwareVolume || p->PreAmp);
	return ERR_NONE;
}

static int UpdateBufferTime(waveout_android* p)
{
	p->BufferLimit = Scale(p->BufferMode?BUFFER_VIDEO:BUFFER_MUSIC,p->Format.nAvgBytesPerSec,p->BufferLength*TICKSPERSEC);
	p->BufferLimitFull = p->BufferLimit;
	return ERR_NONE;
}

static int Process(waveout_android* p,const packet* Packet,const flowstate* State)
{
	DEBUG_MSG1(DEBUG_AUDIO,T("waveout_android::Process::p->Used %d"),p->Used);

	if (!Packet)
	{
		if (State->DropLevel)
			++p->Dropped;
		else
			Write(p,State->CurrTime);

		DEBUG_MSG1(DEBUG_AUDIO,T("waveout_android::Process::if (!Packet) ::p->Waiting %d"),p->Waiting);

		return (p->Waiting<=0 || p->Speed.Num==0) ? ERR_NONE : ERR_BUFFER_FULL;
	}

	if (p->Speed.Num==0) // benchmark mode (auto adjust speed)
	{
		int Pos = p->BenchWaitPos;
		int OldSum;
		int Speed;

		if (p->Play)
		{
			while (Packet->Length > p->BenchAvgLimit)
				UpdateBenchAvg(p);

			p->BenchCurrSum -= p->BenchWait[Pos];
			p->BenchWait[Pos] = (p->Waiting * p->BufferLength) >> 12;
			p->BenchCurrSum += p->BenchWait[Pos];

			OldSum = p->BenchSum[Pos];
			p->BenchSum[Pos] = p->BenchCurrSum;

			if (++Pos == BENCH_SIZE)
				Pos = 0;

			p->BenchWaitPos = Pos;

			if (p->BenchCurrSum < 2*BENCH_SIZE*p->BenchAvg)
				Speed = (p->BenchCurrSum+1) * p->BenchSpeedAvg;
			else
				Speed = 2*SPEED_ONE+(p->BenchCurrSum-2*BENCH_SIZE*p->BenchAvg+1) * 4*p->BenchSpeedAvg;

			Speed -= p->BenchAdj*(p->BenchCurrSum - OldSum);

			if (p->Waiting < 3)
				Speed -= SPEED_ONE/5;
		}
		else
			Speed = SPEED_ONE;

		DEBUG_MSG3(DEBUG_AUDIO,T("waveout_android::Process::Audio speed:%d length:%d (p->Waiting:%d)"),Speed,Packet->Length,p->Waiting);
		return Send(p,Packet->Data,Packet->Length,Packet->RefTime,State->CurrTime,Speed);
	}

	DEBUG_MSG1(DEBUG_AUDIO,T("waveout_android::Process::State->DropLevel %d "),State->DropLevel);
	if (State->DropLevel)
		return ERR_NONE;

	if (p->Used >= p->BufferLimit)
	{
		DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::Process::if (p->Used >= p->BufferLimit):p->Used: %d p->BufferLimit %d "),p->Used , p->BufferLimit);
		Write(p,State->CurrTime);
		return ERR_BUFFER_FULL;
	}

	DEBUG_MSG3(DEBUG_AUDIO,T("waveout_android::Process::Send reftime:%d used:%d p->Waiting:%d"),Packet->RefTime,p->Used,p->Waiting);
	return Send(p,Packet->Data,Packet->Length,Packet->RefTime,State->CurrTime,p->PCMSpeed);
}

static bool_t FreeBuffers(waveout_android* p);

static int UpdateInput(waveout_android* p)
{
	DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::UpdateInput"));

	Reset(p);
	FreeBuffers(p);

	if (p->bOpen)
	{
		DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::UpdateInput::AudioDestroy"));
		p->Device.AudioFuncDestroy();
		p->bOpen = 0;
		p->bStart = 0;
	}
	p->Total = 0;
	p->Dropped = 0;
	p->Process = DummyProcess;

	if (p->Input.Type == PACKET_AUDIO)
	{
		int Result;
		int Try;

		if (p->Input.Format.Audio.Format != AUDIOFMT_PCM)
		{
			PacketFormatClear(&p->Input);
			DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::UpdateInput::ERR_INVALID_DATA !AUDIOFMT_PCM"));
			return ERR_INVALID_DATA;
		}

		if (p->Input.Format.Audio.Channels == 0 ||
			p->Input.Format.Audio.SampleRate == 0)
			return ERR_NONE; // probably initialized later

		p->Output.Type = PACKET_AUDIO;
		p->Output.Format.Audio = p->Input.Format.Audio;
		p->Output.Format.Audio.Flags = 0;
		p->Dither = 0;

		if (p->Stereo==STEREO_SWAPPED)
			p->Output.Format.Audio.Flags |= PCM_SWAPPEDSTEREO;
		else
		if (p->Stereo!=STEREO_NORMAL)
		{
			p->Output.Format.Audio.Channels = 1;
			if (p->Stereo==STEREO_LEFT)
				p->Output.Format.Audio.Flags |= PCM_ONLY_LEFT;
			if (p->Stereo==STEREO_RIGHT)
				p->Output.Format.Audio.Flags |= PCM_ONLY_RIGHT;
		}

		switch (p->Quality)
		{
		case 0: // low quality for very poor devices
			p->Output.Format.Audio.Bits = 8;
			p->Output.Format.Audio.FracBits = 7;
			p->Output.Format.Audio.Channels = 1;
			p->Output.Format.Audio.SampleRate = 22050;
			break;

		case 1: // no dither and only standard samplerate
			p->Output.Format.Audio.Bits = 16;
			p->Output.Format.Audio.FracBits = 15;
			if (p->Output.Format.Audio.SampleRate >= 44100)
				p->Output.Format.Audio.SampleRate = 44100;
			else
				p->Output.Format.Audio.SampleRate = 22050;
			break;

		default:
		case 2: // original samplerate
			p->Output.Format.Audio.Bits = 16;
			p->Output.Format.Audio.FracBits = 15;
			p->Dither = 1;
			break;
		}


		if (p->Output.Format.Audio.Bits <= 8)
			p->Output.Format.Audio.Flags |= PCM_UNSIGNED;

		p->Format.wFormatTag = WAVE_FORMAT_PCM;
		p->Format.nChannels = (int16_t)p->Output.Format.Audio.Channels;
		p->Format.nSamplesPerSec = p->Output.Format.Audio.SampleRate;
		p->Format.wBitsPerSample = (int16_t)p->Output.Format.Audio.Bits;
		p->Format.nBlockAlign = (int16_t)((p->Format.nChannels * p->Format.wBitsPerSample) >> 3);
		p->Format.nAvgBytesPerSec = p->Format.nSamplesPerSec * p->Format.nBlockAlign;
		p->Format.cbSize = 0;

		p->BufferLength = 4096;
		if (p->Format.nAvgBytesPerSec > 65536)
			p->BufferLength *= 2;
		if (p->Format.nAvgBytesPerSec > 2*65536)
			p->BufferLength *= 2;

		DEBUG_MSG6(DEBUG_AUDIO,T("waveout_android::UpdateInput::AudioCreate p->Format.nAvgBytesPerSec %d,p->BufferLength %d nSamplesPerSec %d,wBitsPerSample %d,FormatTag %d,nChannel %d"),\
				p->Format.nAvgBytesPerSec,p->BufferLength,p->Format.nSamplesPerSec,p->Format.wBitsPerSample,p->Format.wFormatTag,p->Format.nChannels);

		Result = p->Device.AudioFunCreate(p->Format.nSamplesPerSec, (p->Format.wBitsPerSample-1)/8,p->Format.nChannels,p->BufferLength,wavout_cbf,(void*)p);

		DEBUG_MSG1(DEBUG_AUDIO,T("waveout_android::UpdateInput::AudioCreate ret %d"),Result);

		if(Result == ERR_NONE )
			p->bOpen = 1;

		if (p->bOpen)
		{
			p->Process = Process;
			p->TimeRef = GetTimeTick();

			UpdateBufferTime(p);

			p->FillPos = p->BufferLength;
			p->VolumeRamp = 0;
			DEBUG_MSG4(DEBUG_AUDIO,T("waveout_android::UpdateInput::UpdatePCM(p,&p->Input.Format.Audio) p->Input.Format.Audio.SampleRate %d,p->Input.Format.Audio.Bits %d,p->Input.Format.Audio.Channels %d,p->Input.Format.Audio.FracBits %d"),
							p->Input.Format.Audio.SampleRate,p->Input.Format.Audio.Bits,p->Input.Format.Audio.Channels,p->Input.Format.Audio.FracBits);
			UpdatePCM(p,&p->Input.Format.Audio);
			Reset(p);
		}
		else
		{
			PacketFormatClear(&p->Input);
			ShowError(p->Node.Class,ERR_ID,ERR_DEVICE_ERROR);
			DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::UpdateInput::ERR_DEVICE_ERROR !AUDIOFMT_PCM"));
			return ERR_DEVICE_ERROR;
		}
	}
	else
	if (p->Input.Type != PACKET_NONE)
		return ERR_INVALID_DATA;

	return ERR_NONE;
}

static int Update(waveout_android* p)
{
	wavebuffer *OldFirst;
	wavebuffer *OldLast;
	audio OldFormat;
	wavebuffer* OldBuffers;
	wavebuffer* OldFill;
	int OldFillPos;
	int OldUsed;
	bool_t OldVolume;
	int OldPreAmp;
	wavebuffer *Buffer,*Next;

	pthread_mutex_lock(&p->Section);

	OldVolume = p->SoftwareVolume;
	OldPreAmp = p->PreAmp;
	OldUsed = p->Used;
	OldFirst = p->FreeFirst;
	OldLast = p->FreeLast;
	OldFormat = p->Output.Format.Audio;
	OldFormat.SampleRate = p->AdjustedRate;

	OldFill = p->FillFirst;
	OldFillPos = p->FillPos;

	p->FillFirst = NULL;
	p->FillLast = NULL;
	p->FillPos = p->BufferLength;
	p->FreeFirst = NULL;
	p->FreeLast = NULL;

	pthread_mutex_unlock(&p->Section);

	Reset(p);

	OldBuffers = p->FreeFirst;
	if (p->FreeLast)
		p->FreeLast->Next = OldFill;
	else
		OldBuffers = OldFill;

	if (OldBuffers)
		p->FillLastTime = OldBuffers->EstRefTime - p->BufferScaledTime;

	p->PreAmp = 0;
	p->SoftwareVolume = 0;
	p->Used = OldUsed;
	p->FreeFirst = OldFirst;
	p->FreeLast = OldLast;

	// setup temporary format
	UpdatePCM(p,&OldFormat);

	for (Buffer=OldBuffers;Buffer;Buffer=Next)
	{
		Next = Buffer->Next;
		Send(p,Buffer->Planes,Next ? p->BufferLength:OldFillPos,Buffer->RefTime,-1,p->PCMSpeed);
		ReleaseBuffer(p,Buffer,0);
	}

	p->SoftwareVolume = OldVolume;
	p->PreAmp = OldPreAmp;
	UpdatePCM(p,&p->Input.Format.Audio);
	return ERR_NONE;
}

static bool_t FreeBuffers(waveout_android* p)
{
	wavebuffer** Ptr;
	wavebuffer* Buffer;
	bool_t Changed = 0;

	while (p->FreeFirst)
	{
		Buffer = p->FreeFirst;
		p->FreeFirst = Buffer->Next;

		// remove from global chain
		Ptr = &p->Buffers;
		while (*Ptr && *Ptr != Buffer)
			Ptr = &(*Ptr)->GlobalNext;
		if (*Ptr == Buffer)
			*Ptr = Buffer->Next;

		FreeBlock(&Buffer->Block);
		free(Buffer);
		Changed = 1;
	}
	p->FreeLast = NULL;

	return Changed;
}

static int Hibernate(waveout_android* p,int Mode)
{
	bool_t Changed = 0;

	pthread_mutex_lock(&p->Section);
	Changed = FreeBuffers(p);
	pthread_mutex_unlock(&p->Section);

	return Changed ? ERR_NONE : ERR_OUT_OF_MEMORY;
}

static void Pause(waveout_android* p)
{
	DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::Pause"));

	p->Device.AudioFuncPause();
	p->bStart = 0;

	if(p->CurBuffer)
	{
		//释放上次渲染完毕的Buffer
		ReleaseBuffer(p, p->CurBuffer, 0);
		pthread_mutex_lock(&p->Section);
		p->Waiting--;
		pthread_mutex_unlock(&p->Section);
		p->CurBuffer = NULL;
	}
	p->Pausing = &p->FillFirst;
	// release W chain to fill chain
	while (p->WFirst)
	{
		wavebuffer* Buffer = p->WFirst;
		DEBUG_MSG3(DEBUG_AUDIO,T("waveout_android::Pause p->WFirst 0x%x p->WLast 0x%x,p->Waiting %d"),p->WFirst,p->WLast,p->Waiting);
		p->WFirst = Buffer->Next;
		if (p->Pausing)
		{
			// add buffer to fill chain
			Buffer->Next = *p->Pausing;
			*p->Pausing = Buffer;
			p->Pausing = &Buffer->Next;

		}
		pthread_mutex_lock(&p->Section);
		p->Waiting--;
		pthread_mutex_unlock(&p->Section);
		DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::Pause while (p->WFirst 0x%x) p->Waiting %d"),p->WFirst,p->Waiting);
	}
	p->WLast = p->WFirst;
	p->Pausing = NULL;
	DEBUG_MSG3(DEBUG_AUDIO,T("waveout_android::Pause while (p->WFirst 0x%x) p->WLast 0x%x,p->Waiting %d"),p->WFirst,p->WLast,p->Waiting);
	p->FillLast = p->FillFirst;
	if (p->FillLast)
		while (p->FillLast->Next) p->FillLast = p->FillLast->Next;
}

static int UpdatePlay(waveout_android* p)
{
	if (p->Play)
	{
		p->TimeRef = GetTimeTick();
		if (p->bOpen)
		{
			DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::UpdatePlay:if (p->bOpen)Write(p,p->Tick) p->Waiting %d p->Used %d"),p->Waiting,p->Used);
			Write(p,p->Tick);
		}
	}
	else
	{
		DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::UpdatePlay:p->Play ==0 !! p->Waiting %d p->Used %d"),p->Waiting,p->Used);
		if (p->bOpen)
		{
			int OldTick = p->Tick;
			Pause(p);
			p->Tick = OldTick;
		}
	}
	return ERR_NONE;
}

static int SetVolumeDev(waveout_android* p,int v)
{
	p->VolumeDev = v;
//	if (p->Mute)
//		waveOutSetVolume(NULL,0);
//	else
//		waveOutSetVolume(NULL,0x10001 * ((0xFFFF * p->VolumeDev) / 100));
	return ERR_NONE;
}

static int TimerSet(void* pt, int No, const void* Data, int Size)
{
	waveout_android* p = WAVEOUT(pt);
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case TIMER_PLAY: SETVALUECMP(p->Play,bool_t,UpdatePlay(p),EqBool); break;
	case TIMER_SPEED: SETVALUECMP(p->Speed,fraction,Update(p),EqFrac); break;
	case TIMER_TIME:
		assert(Size == sizeof(tick_t));
		pthread_mutex_lock(&p->Section);
		p->Tick = *(tick_t*)Data;
		p->TimeRef = GetTimeTick();
		DEBUG_MSG2(DEBUG_AUDIO,T("waveout_android::TimerSet p->Tick %d,p->TimeRef %d"),p->Tick,p->TimeRef);
		pthread_mutex_unlock(&p->Section);
		Result = ERR_NONE;
		break;
	}
	return Result;
}

static void UpdateSoftwareVolume(waveout_android* p)
{
	bool_t SoftwareVolume = !QueryAdvanced(ADVANCED_SYSTEMVOLUME);
	if (SoftwareVolume != p->SoftwareVolume)
	{
		p->SoftwareVolume = SoftwareVolume;
		SetVolumeSoft(p,p->VolumeSoft,p->Mute);
		if (p->bOpen)
			UpdatePCM(p,&p->Input.Format.Audio);
	}
}

static int UpdatePreAmp(waveout_android* p)
{
	SetVolumeSoft(p,p->VolumeSoft,p->Mute);
	if (p->bOpen)
		UpdatePCM(p,&p->Input.Format.Audio);
	return ERR_NONE;
}

static int Set(waveout_android* p, int No, const void* Data, int Size)
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case OUT_INPUT|PIN_FORMAT:
		if (PacketFormatSimilarAudio(&p->Input,(const packetformat*)Data))
		{
			PacketFormatCopy(&p->Input,(const packetformat*)Data);
			Result = UpdatePCM(p,&p->Input.Format.Audio);
		}
		else
			SETPACKETFORMATCMP(p->Input,packetformat,UpdateInput(p));
		break;
	case OUT_INPUT: SETVALUE(p->Pin,pin,ERR_NONE); break;
	case OUT_TOTAL: SETVALUE(p->Total,int,ERR_NONE); break;
	case OUT_DROPPED: SETVALUE(p->Dropped,int,ERR_NONE); break;

	case AOUT_VOLUME:
		assert(Size==sizeof(int));
		UpdateSoftwareVolume(p);
		if (p->SoftwareVolume)
			Result = SetVolumeSoft(p,*(int*)Data,p->Mute);
		else
			Result = SetVolumeDev(p,*(int*)Data);
		break;

	case AOUT_PREAMP: SETVALUECMP(p->PreAmp,int,UpdatePreAmp(p),EqInt); break;

	case AOUT_MUTE:
		assert(Size==sizeof(bool_t));
		UpdateSoftwareVolume(p);
		if (p->SoftwareVolume)
			Result = SetVolumeSoft(p,p->VolumeSoft,*(bool_t*)Data);
		else
		{
			if (!p->Mute) GetVolume(p); // save old volume to p->VolumeDev
			p->Mute = *(bool_t*)Data;
			Result = SetVolumeDev(p,p->VolumeDev);
		}
		break;

	case AOUT_STEREO: SETVALUECMP(p->Stereo,int,UpdateInput(p),EqInt); break;
	case AOUT_QUALITY: SETVALUECMP(p->Quality,int,UpdateInput(p),EqInt); break;
	case AOUT_MODE: SETVALUE(p->BufferMode,bool_t,UpdateBufferTime(p)); break;

	case FLOW_FLUSH:
		Reset(p);
		p->FillLastTime = TIME_UNKNOWN;
		p->VolumeRamp = 0;
		Result = ERR_NONE;
		break;

	case NODE_SETTINGSCHANGED:
		p->ForcePriority = QueryAdvanced(ADVANCED_WAVEOUTPRIORITY);
		break;

	case NODE_HIBERNATE:
		assert(Size == sizeof(int));
		Result = Hibernate(p,*(int*)Data);
		break;
	}
	return Result;
}

static int Create(waveout_android* p)
{
	DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::Create"));
	p->Node.Get = (nodeget)Get;
	p->Node.Set = (nodeset)Set;

	p->Timer.Class = TIMER_CLASS;
	p->Timer.Enum = TimerEnum;
	p->Timer.Get = TimerGet;
	p->Timer.Set = TimerSet;

	p->VolumeDev = 70; //default when device is muted
	p->VolumeSoftLog = 256;
	p->Quality = 2;
	p->Speed.Den = p->Speed.Num = 1;
	p->SpeedTime.Num = TICKSPERSEC;
	p->SpeedTime.Den = GetTimeFreq();

	DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::Create::pthread_mutex_init(&p->Section, NULL);"));
	pthread_mutex_init(&p->Section, NULL);

	p->bStart		= 0;
	p->WLast		= NULL;
	p->WFirst		= NULL;
	p->CurBuffer	= NULL;

#ifdef USESLES
	p->Device.AudioFunCreate 	= AudioOpenSLESCreate;
	p->Device.AudioFuncStart 	= AudioOpenSLESStart;	
	p->Device.AudioFuncReset 	= AudioOpenSLESReset;
	p->Device.AudioFuncWrite 	= AudioOpenSLESWrite;
	p->Device.AudioFuncPause 	= AudioOpenSLESPause;
	p->Device.AudioFuncFlush 	= AudioOpenSLESFlush;
	p->Device.AudioFuncStop 	= AudioOpenSLESStop;
	p->Device.AudioFuncDestroy  = AudioOpenSLESDestroy;
#else
	p->Device.AudioFunCreate 	= AudioTrackCreate;
	p->Device.AudioFuncStart 	= AudioTrackStart;	
	p->Device.AudioFuncReset 	= AudioTrackReset;
	p->Device.AudioFuncWrite 	= AudioTrackWrite;
	p->Device.AudioFuncPause 	= AudioTrackPause;
	p->Device.AudioFuncFlush 	= AudioTrackFlush;
	p->Device.AudioFuncStop 	= AudioTrackStop;
	p->Device.AudioFuncDestroy  = AudioTrackDestroy;
#endif
	return ERR_NONE;
}

static void Delete(waveout_android* p)
{
	DEBUG_MSG(DEBUG_AUDIO,T("waveout_android::Delete"));
	PacketFormatClear(&p->Input);
	PCMRelease(p->PCM);

	pthread_mutex_destroy(&p->Section);
}

static const nodedef WaveOut =
{
	sizeof(waveout_android)|CF_GLOBAL,
	WAVEOUT_ID,
	AOUT_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
	(nodedelete)Delete,
};

void WaveOut_Init()
{
	NodeRegisterClass(&WaveOut);
}

void WaveOut_Done()
{
	NodeUnRegisterClass(WAVEOUT_ID);
}


#endif
