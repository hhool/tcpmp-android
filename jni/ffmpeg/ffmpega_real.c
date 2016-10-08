
#include "../common/common.h"

#include "ffmpeg.h"

#ifdef ENABLE_REAL
#undef memcpy
#undef malloc
#undef realloc
#undef free
#include "libavcodec/avcodec.h"

static const unsigned char sipr_swaps[38][2] = {
    {  0, 63 }, {  1, 22 }, {  2, 44 }, {  3, 90 },
    {  5, 81 }, {  7, 31 }, {  8, 86 }, {  9, 58 },
    { 10, 36 }, { 12, 68 }, { 13, 39 }, { 14, 73 },
    { 15, 53 }, { 16, 69 }, { 17, 57 }, { 19, 88 },
    { 20, 34 }, { 21, 71 }, { 24, 46 }, { 25, 94 },
    { 26, 54 }, { 28, 75 }, { 29, 50 }, { 32, 70 },
    { 33, 92 }, { 35, 74 }, { 38, 85 }, { 40, 56 },
    { 42, 87 }, { 43, 65 }, { 45, 59 }, { 48, 79 },
    { 49, 93 }, { 51, 89 }, { 55, 95 }, { 61, 76 },
    { 67, 83 }, { 77, 80 }
};

typedef struct RMAudio
{
	/// Audio descrambling matrix parameters
    uint8_t *audiobuf; ///< place to store reordered audio data
	uint8_t *UnOrderAudiobuf; ///< place to store reordered audio data
	int32_t  audio_len;
    int sub_packet_size, sub_packet_h, coded_framesize; ///< Descrambling parameters from container
    int audio_framesize; /// Audio frame size from container
}RMAudio;
typedef struct ffmpeg_audio_real
{
	codec			Codec;
	buffer			Buffer;
	AVCodecContext	*Context;
	int16_t			*OutBuf;
	int				OutSize; //In Byte
	int				CodecId;
	tick_t			FrameTime;
	RMAudio			rm;
	tick_t			RefTime;

} ffmpeg_audio_real;

typedef struct codecinfo
{
	int Id;
	int CodecId;
	const tchar_t* Name;
	const tchar_t* ContentType;
	
} codecinfo;

#define FFMPEG_AUDIO_REAL_CLASS		FOURCC('F','M','R','A')

static const codecinfo Info[] = 
{
	{FOURCC('F','C','O','K'),CODEC_ID_COOK,T("FFmpeg Cook"),
		T("acodec/0x1010")},
	{FOURCC('F','A','T','R'),CODEC_ID_ATRAC3,T("FFmpeg Atrac"),
		T("acodec/0x1011")},
	{FOURCC('F','R','A','1'),CODEC_ID_RA_144,T("FFmpeg RA144"),
		T("acodec/0x1012")},
	{FOURCC('F','R','A','2'),CODEC_ID_RA_288,T("FFmpeg RA288"),
		T("acodec/0x1013")},
	{FOURCC('F','S','I','P'),CODEC_ID_SIPR,T("FFmpeg SIPR"),
		T("acodec/0x1014")},
		
	{0},
};

static int UpdateInput(ffmpeg_audio_real* p)
{
	if (p->Context)
	{
		avcodec_close(p->Context);
		av_free(p->Context);
	}
	
	if(p->rm.audiobuf)
	{
		av_free(p->rm.audiobuf);
	}
	if(p->rm.UnOrderAudiobuf)
	{
		av_free(p->rm.UnOrderAudiobuf);
	}
	if(p->OutBuf)
		av_free(p->OutBuf);

	p->RefTime = TIME_UNKNOWN;
	p->rm.audiobuf = NULL;
	p->rm.UnOrderAudiobuf = NULL;
	p->Context	= NULL;
	p->OutBuf	= NULL;
	BufferClear(&p->Buffer);

	if (p->Codec.In.Format.Type == PACKET_AUDIO)
	{
		AVCodec *Codec;
		
		const codecinfo *i;
		for (i=Info;i->Id;++i)
			if (i->Id == p->Codec.Node.Class)
				break;
		if (!i->Id)
			return ERR_INVALID_DATA;
			
		Codec = avcodec_find_decoder(i->CodecId);
		if (!Codec)
			return ERR_INVALID_DATA;

		p->Context = avcodec_alloc_context();
		
		if (!p->Context)
			return ERR_OUT_OF_MEMORY;

		PacketFormatPCM(&p->Codec.Out.Format,&p->Codec.In.Format,16);
		
		p->Context->bit_rate		= p->Codec.In.Format.ByteRate*8;
		p->Context->bits_per_coded_sample	= p->Codec.In.Format.Format.Audio.Channels*8;
		p->Context->sample_rate		= p->Codec.In.Format.Format.Audio.SampleRate;
		p->Context->channels		= p->Codec.In.Format.Format.Audio.Channels;

		if(i->Id == FOURCC('F','C','O','K')||
		   i->Id == FOURCC('F','A','T','R')||
		   i->Id == FOURCC('F','S','I','P'))
		{
			uint32_t* pExtral = p->Codec.In.Format.Extra;
			p->rm.sub_packet_size	= pExtral[0];
			p->rm.coded_framesize	= pExtral[1];
			p->rm.sub_packet_h		= pExtral[2];
			p->rm.audio_framesize	= pExtral[3];
			
			
			p->rm.audiobuf = av_malloc(p->rm.sub_packet_h * p->rm.audio_framesize);
			p->rm.UnOrderAudiobuf= av_malloc(p->rm.sub_packet_h * p->rm.audio_framesize);

			p->Context->extradata = (uint8_t*)(&pExtral[4]);
			p->Context->extradata_size	= p->Codec.In.Format.ExtraLength-16;
		}
		else
		{
			p->Context->extradata		= p->Codec.In.Format.Extra;
			p->Context->extradata_size	= p->Codec.In.Format.ExtraLength;
		}
		p->Context->codec_type		= CODEC_TYPE_AUDIO;
		p->Context->block_align		= p->Codec.In.Format.Format.Audio.BlockAlign;
		
		if (avcodec_open(p->Context,Codec)<0)
		{
			// avoid calling avcodec_close at next UpdateInput
			av_free(p->Context);
			p->Context = NULL;
			
			if(p->rm.audiobuf)
				av_free(p->rm.audiobuf);
			p->rm.audiobuf = NULL;

			if(p->rm.UnOrderAudiobuf)
				av_free(p->rm.UnOrderAudiobuf);
			p->rm.UnOrderAudiobuf = NULL;

			return ERR_INVALID_DATA;
		}
	}

	return ERR_NONE;
}
static void ff_rm_reorder_sipr_data(ffmpeg_audio_real* p)
{
	int n, bs = p->rm.sub_packet_h * p->rm.audio_framesize * 2 / 96; // nibbles per subpacket
	uint8_t *buf = p->rm.audiobuf;

    for (n = 0; n < 38; n++) {
        int j;
        int i = bs * sipr_swaps[n][0];
        int o = bs * sipr_swaps[n][1];

        /* swap 4bit-nibbles of block 'i' with 'o' */
        for (j = 0; j < bs; j++, i++, o++) {
            int x = (buf[i >> 1] >> (4 * (i & 1))) & 0xF,
                y = (buf[o >> 1] >> (4 * (o & 1))) & 0xF;

            buf[o >> 1] = (x << (4 * (o & 1))) |
                (buf[o >> 1] & (0xF << (4 * !(o & 1))));
            buf[i >> 1] = (y << (4 * (i & 1))) |
                (buf[i >> 1] & (0xF << (4 * !(i & 1))));
        }
    }
}
static int Process(ffmpeg_audio_real* p, const packet* Packet, const flowstate* State)
{
	int Len;
	
	p->Codec.Packet.RefTime = TIME_UNKNOWN;

	if (Packet)
	{
		
		DEBUG_MSG1(-1,T("RefTime %d"),p->Codec.Packet.RefTime);
		
		if((p->Codec.In.Format.Format.Audio.Format == AUDIOFMT_COOK||
			p->Codec.In.Format.Format.Audio.Format == AUDIOFMT_ATRC||
			p->Codec.In.Format.Format.Audio.Format == AUDIOFMT_SIPR))
		{
            int sps = p->rm.sub_packet_size;
            int cfs = p->rm.coded_framesize;
						
			if(p->rm.audio_len == 0 &&p->RefTime ==TIME_UNKNOWN)
			{
				if(Packet->RefTime < 0)
					return ERR_INVALID_DATA;
				
				p->RefTime = Packet->RefTime;
			}

			memcpy(p->rm.UnOrderAudiobuf+p->rm.audio_len,Packet->Data[0],Packet->Length);
		
			p->rm.audio_len+=Packet->Length;

			if(p->rm.audio_len<p->rm.sub_packet_h * p->rm.audio_framesize)
			{				
				if(p->Buffer.WritePos-p->Buffer.ReadPos < p->rm.sub_packet_size || p->rm.sub_packet_size <= 0)//only cook audio seekºó3Ãë¾²Òô
				{
					return ERR_NEED_MORE_DATA;
				}
			}
			else
			{
				int x,w2,y;
				uint8_t* dst = p->rm.audiobuf;
				uint8_t* src = p->rm.UnOrderAudiobuf;

				BufferPack(&p->Buffer,0);
				
				if((p->Codec.In.Format.Format.Audio.Format == AUDIOFMT_COOK||
				 p->Codec.In.Format.Format.Audio.Format == AUDIOFMT_ATRC))
				{
					for(y = 0; y < p->rm.sub_packet_h; y++)
					{
						for(x = 0, w2 = p->rm.audio_framesize / sps; x < w2; x++)
						{
							//DEBUG_MSG1(-1,T("%d"),sps*(p->rm.sub_packet_h*x+((p->rm.sub_packet_h+1)/2)*(y&1)+(y>>1)));
							memcpy(dst + sps*(p->rm.sub_packet_h*x+((p->rm.sub_packet_h+1)/2)*(y&1)+(y>>1)), src, sps);
							src += sps;
						}
					}	
				}
				else if(p->Codec.In.Format.Format.Audio.Format == AUDIOFMT_SIPR)
				{
					memcpy(dst,src,cfs*p->rm.sub_packet_h);
					ff_rm_reorder_sipr_data(p);
				}
				BufferWrite(&p->Buffer,dst,p->rm.sub_packet_h * p->rm.audio_framesize,2048);
				p->rm.audio_len = 0;
				memset(p->rm.audiobuf,0,p->rm.sub_packet_h * p->rm.audio_framesize);
				memset(p->rm.UnOrderAudiobuf,0,p->rm.sub_packet_h * p->rm.audio_framesize);

				if (p->RefTime >= 0)
				{
					p->Codec.Packet.RefTime = p->RefTime;
					p->RefTime = TIME_UNKNOWN;
				}

			}
			
		}
	}

	if (p->Buffer.WritePos-p->Buffer.ReadPos < 8)
		return ERR_NEED_MORE_DATA;

	if(p->OutBuf==NULL)
		p->OutBuf = av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);
//retry: //2 only cook audio seekºó3Ãë¾²Òô
	p->OutSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;

    Len = avcodec_decode_audio2(p->Context,p->OutBuf,&p->OutSize,\
		p->Buffer.Data+p->Buffer.ReadPos,p->Buffer.WritePos-p->Buffer.ReadPos);
	
	if (Len < 0)
	{
		BufferDrop(&p->Buffer);
		return ERR_INVALID_DATA;
	}

	p->Buffer.ReadPos+=Len;
//	if(p->OutSize == 0) //2 end only cook audio seekºó3Ãë¾²Òô
//		goto retry;
	p->Codec.Packet.Length = p->OutSize;
	p->Codec.Packet.Data[0] = p->OutBuf;
	return ERR_NONE;
}

static int Flush(ffmpeg_audio_real* p)
{
	BufferDrop(&p->Buffer);
	p->OutSize				= 0;
	p->rm.audio_len			= 0;
	if(p->rm.audiobuf)
	memset(p->rm.audiobuf,0,p->rm.sub_packet_h * p->rm.audio_framesize);
	if(p->rm.UnOrderAudiobuf)
	memset(p->rm.UnOrderAudiobuf,0,p->rm.sub_packet_h * p->rm.audio_framesize);
	
	return ERR_NONE;
}

static int Create( ffmpeg_audio_real* p )
{
	p->Codec.Process		= (packetprocess)Process;
	p->Codec.UpdateInput	= (nodefunc)UpdateInput;
	p->Codec.Flush			= (nodefunc)Flush;
	p->OutBuf				= NULL;
	p->OutSize				= 0;
	return ERR_NONE;
}

static const nodedef FFMPEGRealAudio =
{
	sizeof(ffmpeg_audio_real)|CF_ABSTRACT,
	FFMPEG_AUDIO_REAL_CLASS,
	CODEC_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
	NULL,
};
#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(CONFIG_##X##_DECODER)  avcodec_register(&x##_decoder); }

void FFMPEGA_Real_Init()
{
	nodedef Def;
	const codecinfo* i;
	
	avcodec_init();

	REGISTER_DECODER (COOK, cook);
	REGISTER_DECODER (ATRAC3, atrac3);
	REGISTER_DECODER (RA_144, ra_144);
	REGISTER_DECODER (RA_288, ra_288);
	REGISTER_DECODER (SIPR,	sipr);
	

	NodeRegisterClass(&FFMPEGRealAudio);
	memset(&Def,0,sizeof(Def));
	
	for (i=Info;i->Id;++i)
	{
		StringAdd(1,i->Id,NODE_NAME,i->Name);
		StringAdd(1,i->Id,NODE_CONTENTTYPE,i->ContentType);
		
		Def.Class = i->Id;
		Def.ParentClass = FFMPEG_AUDIO_REAL_CLASS;
		Def.Priority = PRI_DEFAULT-10; // do not override ARM optimized codecs by default
		Def.Flags = 0; // parent size
		
		NodeRegisterClass(&Def);
	}
}

void FFMPEGA_Real_Done()
{
	NodeUnRegisterClass(FFMPEG_AUDIO_REAL_CLASS);

}
#else
void FFMPEGA_Real_Init(){}
void FFMPEGA_Real_Done(){}
#endif