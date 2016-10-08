
#include "../common/common.h"
#include "ffmpeg.h"

#undef memcpy
#undef malloc
#undef realloc
#undef free
#include "libavcodec/avcodec.h"

typedef struct ffmpeg_audio
{
	codec			Codec;
	buffer			Buffer;
	AVCodecContext	*Context;
	int16_t			*OutBuf;
	int				OutSize; //In Byte
	int				CodecId;
	tick_t			FrameTime;
	bool_t			bOut;

} ffmpeg_audio;

typedef struct codecinfo
{
	int Id;
	int CodecId;
	const tchar_t* Name;
	const tchar_t* ContentType;
	
} codecinfo;

#define FFMPEG_AUDIO_ORIGIN_CLASS		FOURCC('F','M','A','O')

static const codecinfo Info[] = 
{
	{FOURCC('F','W','A','1'),CODEC_ID_WMAV1,T("FFmpeg WMA1"),
		T("acodec/0x0160")},
	{FOURCC('F','W','A','2'),CODEC_ID_WMAV2,T("FFmpeg WMA2"),
		T("acodec/0x0161")},	
	{FOURCC('F','A','A','C'),CODEC_ID_AAC,T("FFmpeg AAC"),
		T("acodec/0xaac0")},
	{FOURCC('F','N','M','R'),CODEC_ID_NELLYMOSER,T("FFmpeg Nellymoser"),
		T("acodec/0x1020")},
	{FOURCC('F','A','W','F'),CODEC_ID_ADPCM_SWF,T("FFmpeg ADPCM Shockwave Flash"),
		T("acodec/0x1021")},
	{FOURCC('F','Q','D','M'),CODEC_ID_QDM2,T("FFmpeg QDesign Music Codec 2"),
		T("acodec/0x0450")},
	{0},
};

static int UpdateInput(ffmpeg_audio* p)
{
	if (p->Context)
	{
		avcodec_close(p->Context);
		av_free(p->Context);
	}
	
	if(p->OutBuf)
		av_free(p->OutBuf);

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
		DEBUG_MSG1(DEBUG_ACODEC,T("ffmpeg_audio::UpdateInput:Find Audio ID %d"),i->Id);
		if (!i->Id)
			return ERR_INVALID_DATA;
			
		DEBUG_MSG(DEBUG_ACODEC,T("ffmpeg_audio::UpdateInput:avcodec_find_decoder"));

		Codec = avcodec_find_decoder(i->CodecId);
		if (!Codec)
		{
			DEBUG_MSG(DEBUG_ACODEC,T("ffmpeg_audio::UpdateInput:avcodec_find_decoder NULL"));
			return ERR_INVALID_DATA;
		}

		p->Context = avcodec_alloc_context();
		
		if (!p->Context)
			return ERR_OUT_OF_MEMORY;

		PacketFormatPCM(&p->Codec.Out.Format,&p->Codec.In.Format,16);
		
		p->Context->bit_rate		= p->Codec.In.Format.ByteRate*8;
		p->Context->bits_per_coded_sample	= p->Codec.In.Format.Format.Audio.Channels*8;
		p->Context->sample_rate		= p->Codec.In.Format.Format.Audio.SampleRate;
		p->Context->channels		= p->Codec.In.Format.Format.Audio.Channels;
		p->Context->extradata		= p->Codec.In.Format.Extra;
		p->Context->extradata_size	= p->Codec.In.Format.ExtraLength;
		p->Context->codec_type		= CODEC_TYPE_AUDIO;
		p->Context->block_align		= p->Codec.In.Format.Format.Audio.BlockAlign;
		DEBUG_MSG(DEBUG_ACODEC,T("ffmpeg_audio::UpdateInput:avcodec_open Before"));
		if (avcodec_open(p->Context,Codec)<0)
		{
			DEBUG_MSG(DEBUG_ACODEC,T("ffmpeg_audio::UpdateInput:avcodec_open Failed"));
			// avoid calling avcodec_close at next UpdateInput
			av_free(p->Context);
			p->Context = NULL;
			return ERR_INVALID_DATA;
		}
		DEBUG_MSG(DEBUG_ACODEC,T("ffmpeg_audio::UpdateInput:avcodec_open Ok"));
	}

	return ERR_NONE;
}

static int Process(ffmpeg_audio* p, const packet* Packet, const flowstate* State)
{
	int Len;
	
	if (Packet)
	{
		if (Packet->RefTime >= 0)
			p->Codec.Packet.RefTime = Packet->RefTime;

		// add new packet to buffer
		BufferPack(&p->Buffer,0);
		BufferWrite(&p->Buffer,Packet->Data[0],Packet->Length,2048);
	}
	else
	{
		p->Codec.Packet.RefTime = TIME_UNKNOWN;
	}


	if (p->Buffer.WritePos-p->Buffer.ReadPos < 8)
		return ERR_NEED_MORE_DATA;

	if (p->Codec.Node.Class ==  FOURCC('F','Q','D','M'))
	{
		 //QDM2Context *s = avctx->priv_data;
		if (p->Buffer.WritePos-p->Buffer.ReadPos < 4096) //4096 s->group_size = AV_RB32(extradata);
			return ERR_NEED_MORE_DATA;
	}
	if(p->OutBuf==NULL)
		p->OutBuf = av_malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE);

	p->OutSize = AVCODEC_MAX_AUDIO_FRAME_SIZE;

    Len = avcodec_decode_audio2(p->Context,p->OutBuf,&p->OutSize,\
		p->Buffer.Data+p->Buffer.ReadPos,p->Buffer.WritePos-p->Buffer.ReadPos);
	
	if (Len < 0)
	{
		BufferDrop(&p->Buffer);
		return ERR_INVALID_DATA;
	}

	p->Buffer.ReadPos+=Len;
	p->Codec.Packet.Length = p->OutSize;
	p->Codec.Packet.Data[0] = p->OutBuf;
	return ERR_NONE;
}

static int Flush(ffmpeg_audio* p)
{
	BufferDrop(&p->Buffer);
	p->OutSize				= 0;
	p->bOut					= 0;
	return ERR_NONE;
}

static int Create( ffmpeg_audio* p )
{
	p->Codec.Process		= (packetprocess)Process;
	p->Codec.UpdateInput	= (nodefunc)UpdateInput;
	p->Codec.Flush			= (nodefunc)Flush;
	p->OutBuf				= NULL;
	p->OutSize				= 0;
	p->bOut					= 0;
	return ERR_NONE;
}

static const nodedef FFMPEGAudio_Origin =
{
	sizeof(ffmpeg_audio)|CF_ABSTRACT,
	FFMPEG_AUDIO_ORIGIN_CLASS,
	CODEC_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
	NULL,
};
#define REGISTER_DECODER(X,x) { \
	extern AVCodec x##_decoder; \
	if(CONFIG_##X##_DECODER)  avcodec_register(&x##_decoder); }

void FFMPEGA_Origin_Init()
{
	nodedef Def;
	const codecinfo* i;
	
	avcodec_init();

	REGISTER_DECODER(WMAV1,wmav1);
	REGISTER_DECODER(WMAV2,wmav2);
	REGISTER_DECODER(AAC,aac);
	REGISTER_DECODER(NELLYMOSER,nellymoser);
	REGISTER_DECODER(ADPCM_SWF,adpcm_swf);	
	REGISTER_DECODER(QDM2,qdm2);	
	
	NodeRegisterClass(&FFMPEGAudio_Origin);
	memset(&Def,0,sizeof(Def));
	
	for (i=Info;i->Id;++i)
	{
		StringAdd(1,i->Id,NODE_NAME,i->Name);
		StringAdd(1,i->Id,NODE_CONTENTTYPE,i->ContentType);
		
		Def.Class = i->Id;
		Def.ParentClass = FFMPEG_AUDIO_ORIGIN_CLASS;
		Def.Priority = PRI_DEFAULT-10; // do not override ARM optimized codecs by default
		Def.Flags = 0; // parent size
		
		NodeRegisterClass(&Def);
	}
	NodeRegisterClass(&WMAF);
}

void FFMPEGA_Origin_Done()
{
	NodeUnRegisterClass(FFMPEG_AUDIO_ORIGIN_CLASS);
	NodeUnRegisterClass(WMAF_ID);
}
