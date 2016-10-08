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
 * $Id: ffmpeg.c 603 2006-01-19 13:00:33Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common/common.h"
#include "ffmpeg.h"

#ifdef ENABLE_REAL
#undef malloc
#undef realloc
#undef free
#include "libavcodec/avcodec.h"
#include "libavutil/intreadwrite.h"

typedef struct RMVideo{
    int nb_packets;
    int old_format;
    int current_stream;
    int remaining_len;
    uint8_t *videobuf; ///< place to store merged video frame
    int videobufsize;  ///< current assembled frame size
    int videobufpos;   ///< position for the next slice in the video buffer
    int curpic_num;    ///< picture number of current frame
    int cur_slice, slices;
    int64_t pktpos;    ///< first slice position in file
} RMVideo;

typedef struct ffmpeg_video_real
{
	codec Codec;
	buffer Buffer;
    AVCodecContext *Context;
    AVFrame *Picture;
	AVRational Aspect;
	int PixelFormat;
	int SkipToKey;
	int DropToKey;
	bool_t Dropping;
	int CodecId;
	tick_t FrameTime;
    AVPaletteControl Palette;
	RMVideo rm;
	tick_t RefTime;

} ffmpeg_video_real;

typedef struct codecinfo
{
	int Id;
	int CodecId;
	const tchar_t* Name;
	const tchar_t* ContentType;
	
} codecinfo;

#define FFMPEG_VIDEO_REAL_CLASS		FOURCC('F','M','R','V')

static const codecinfo Info[] = 
{
	{FOURCC('R','V','1','0'),CODEC_ID_RV10,T("FFmpeg rv10"),
		T("vcodec/RV10")},
	{FOURCC('R','V','2','0'),CODEC_ID_RV20,T("FFmpeg rv20"),
		T("vcodec/RV20")},
	{FOURCC('R','V','3','0'),CODEC_ID_RV30,T("FFmpeg rv30"),
		T("vcodec/RV30")}, //crash 
	{FOURCC('R','V','4','0'),CODEC_ID_RV40,T("FFmpeg rv40"),
		T("vcodec/RV40")},
	{0},
};


static bool_t BuildOutputFormat(ffmpeg_video_real* p)
{
	int pix_fmt = p->Context->pix_fmt;
	if (pix_fmt<0)
		pix_fmt = PIX_FMT_YUV420P; // is this needed?

	PacketFormatClear(&p->Codec.Out.Format);
	p->Codec.Out.Format.Type = PACKET_VIDEO;

	switch (pix_fmt)
	{
	case PIX_FMT_YUV410P:
		p->Codec.Out.Format.Format.Video.Pixel.Flags = PF_YUV410;
		break;
	case PIX_FMT_YUV420P:
		p->Codec.Out.Format.Format.Video.Pixel.Flags = PF_YUV420;
		break;
	case PIX_FMT_BGR24:
		DefaultRGB(&p->Codec.Out.Format.Format.Video.Pixel,24,8,8,8,0,0,0);
		break;;
	case PIX_FMT_RGB32:
		DefaultRGB(&p->Codec.Out.Format.Format.Video.Pixel,32,8,8,8,0,0,0);
		break;
	case PIX_FMT_RGB555:
		DefaultRGB(&p->Codec.Out.Format.Format.Video.Pixel,16,5,5,5,0,0,0);
		break;
	case PIX_FMT_PAL8:
		p->Codec.Out.Format.Format.Video.Pixel.Flags = PF_PALETTE;
		p->Codec.Out.Format.Format.Video.Pixel.BitCount = 8;
		p->Codec.Out.Format.Format.Video.Pixel.Palette = p->Codec.In.Format.Format.Video.Pixel.Palette;
		break;
	default:
		return 0;
	}

	p->Aspect = p->Context->sample_aspect_ratio;
	p->Codec.Out.Format.Format.Video.Aspect = p->Codec.In.Format.Format.Video.Aspect;
	if (p->Context->sample_aspect_ratio.num>0 && !p->Codec.Out.Format.Format.Video.Aspect)
		p->Codec.Out.Format.Format.Video.Aspect = Scale(ASPECT_ONE,p->Context->sample_aspect_ratio.num,p->Context->sample_aspect_ratio.den);

	p->Codec.In.Format.Format.Video.Width = p->Codec.Out.Format.Format.Video.Width = p->Context->width;
	p->Codec.In.Format.Format.Video.Height = p->Codec.Out.Format.Format.Video.Height = p->Context->height;
	
	if (p->Picture->linesize[0])
		p->Codec.Out.Format.Format.Video.Pitch = p->Picture->linesize[0];
	else
		DefaultPitch(&p->Codec.Out.Format.Format.Video);

	p->PixelFormat = p->Context->pix_fmt;

	if (p->Context->bit_rate > 0 && p->Context->bit_rate < 100000000)
		p->Codec.In.Format.ByteRate = p->Context->bit_rate/8;
	if (p->Context->time_base.num > 0)
	{
		p->Codec.In.Format.PacketRate.Num = p->Context->time_base.den;
		p->Codec.In.Format.PacketRate.Den = p->Context->time_base.num;
		p->FrameTime = Scale(TICKSPERSEC,p->Codec.In.Format.PacketRate.Den,p->Codec.In.Format.PacketRate.Num);
	}
	else
		p->FrameTime = TIME_UNKNOWN;

	//ShowMessage("","%d %d %d",p->Context->pix_fmt,p->Context->width,p->Context->height);
	return 1;
}

static void UpdateSettings(ffmpeg_video_real* p)
{
	p->Context->skip_loop_filter = QueryAdvanced(ADVANCED_NODEBLOCKING)? AVDISCARD_ALL : AVDISCARD_DEFAULT;
}

static int UpdateInput( ffmpeg_video_real* p )
{
	if (p->Context)
		avcodec_close(p->Context);
    av_free(p->Context);
    av_free(p->Picture);
	if(p->rm.videobuf)
	{
		av_free(p->rm.videobuf);
		p->rm.videobuf = NULL;
	}

	p->RefTime = TIME_UNKNOWN;
	p->Context = NULL;
	p->Picture = NULL;
	BufferClear(&p->Buffer);

	if (p->Codec.In.Format.Type == PACKET_VIDEO)
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
		p->Picture = avcodec_alloc_frame();

		if (!p->Context || !p->Picture)
			return ERR_OUT_OF_MEMORY;

	    if ((p->Codec.In.Format.Format.Video.Pixel.Flags & PF_FRAGMENTED) && 
			(Codec->capabilities & CODEC_CAP_TRUNCATED))
			p->Context->flags|= CODEC_FLAG_TRUNCATED;

		UpdateSettings(p);
		p->Context->palctrl = NULL;
	    p->Context->bit_rate = 0;
		p->Context->extradata = p->Codec.In.Format.Extra;
		p->Context->extradata_size = p->Codec.In.Format.ExtraLength;
		p->Context->width = p->Codec.In.Format.Format.Video.Width;
		p->Context->height = p->Codec.In.Format.Format.Video.Height;
		p->Context->bits_per_coded_sample = p->Codec.In.Format.Format.Video.Pixel.BitCount;
		if (p->Codec.In.Format.Format.Video.Pixel.Palette && 
			p->Codec.In.Format.Format.Video.Pixel.BitCount<=8)
		{
			int i,n = 1 << p->Codec.In.Format.Format.Video.Pixel.BitCount;
			for (i=0;i<n;++i)
				p->Palette.palette[i] = INT32LE(p->Codec.In.Format.Format.Video.Pixel.Palette[i].v);
			p->Palette.palette_changed = 1;
			p->Context->palctrl = &p->Palette;
		}

		p->CodecId = i->CodecId;

	    if (avcodec_open(p->Context,Codec)<0)
		{
			// avoid calling avcodec_close at next UpdateInput
		    av_free(p->Context);
			p->Context = NULL;
			return ERR_INVALID_DATA;
		}

		if (!BuildOutputFormat(p))
			return ERR_INVALID_DATA;

		p->SkipToKey = 1;
		p->DropToKey = 1;
		p->Dropping = 0;
	}

	return ERR_NONE;
}

static INLINE bool_t SupportDrop( ffmpeg_video_real* p )
{
	return p->CodecId != CODEC_ID_H264 && (!p->Context->has_b_frames || p->CodecId != CODEC_ID_SVQ3);
}

static int get_num(ffmpeg_video_real* p,uint8_t* buf,int32_t* used)
{
    int n, n1;
	
    n = AV_RB16(buf);
    (*used)+=2;
	buf+=2;
    n &= 0x7FFF;
    if (n >= 0x4000) 
	{
        return n - 0x4000;
    }
	else
	{
        n1 = AV_RB16(buf);
        (*used)+=2;
		buf+=2;
        return (n << 16) | n1;
    }
}
static int rm_assemble_video_frame(ffmpeg_video_real* p,const packet* Packet)
{
    int32_t hdr, seq, pic_num, len2, pos;
    int32_t type;
	uint8_t* buf = Packet->Data[0];
	int32_t len = Packet->Length;
	int32_t used=0;
	RMVideo* rm = &p->rm;

    hdr = *buf++; len--;
    type = hdr >> 6;
    switch(type)
	{
    case 0: // slice
    case 2: // last slice
        seq = *buf++; len--;
		
        len2 = get_num(p,buf, &used);
		buf+=used;
		len-=used;

		used=0;
        pos = get_num(p,buf, &used);
		buf+=used;
		len-=used;

        pic_num = *buf++; len--;

        break;
    case 1: //whole frame
        {
			uint8_t addbuf[9]={0,1,1,1,1,0,0,0,0};
			seq = *buf++; len--;

			BufferWrite(&p->Buffer,addbuf,9,1);
			BufferWrite(&p->Buffer,buf,len,2048);
		}
        return ERR_NONE;
    case 3: //frame as a part of packet
		{
			uint8_t addbuf[9]={0,1,1,1,1,0,0,0,0};
			
			len2 = get_num(p,buf, &used);
			buf+=used;
			len-=used;

			used=0;
			pos = get_num(p,buf, &used);
			buf+=used;
			len-=used;

			pic_num = *buf++; len--;

			BufferWrite(&p->Buffer,addbuf,9,1);
			BufferWrite(&p->Buffer,buf,len2,2048);
		}
        return ERR_NONE;
    }
    //now we have to deal with single slice
	
    if((seq & 0x7F) == 1 || rm->curpic_num != pic_num)
	{
        rm->slices = ((hdr & 0x3F) << 1) + 1;
        rm->videobufsize = len2 + 8*rm->slices + 1;
        av_free(rm->videobuf);
        if(!(rm->videobuf = av_malloc(rm->videobufsize)))
            return ERR_OUT_OF_MEMORY;
        rm->videobufpos = 8*rm->slices + 1;
        rm->cur_slice = 0;
        rm->curpic_num = pic_num;
    }
    if(type == 2)
        len = FFMIN(len, pos);
	
    if(++rm->cur_slice > rm->slices)
        return ERR_OUT_OF_MEMORY;

    AV_WL32(rm->videobuf - 7 + 8*rm->cur_slice, 1);
    AV_WL32(rm->videobuf - 3 + 8*rm->cur_slice, rm->videobufpos - 8*rm->slices - 1);

    if(rm->videobufpos + len > rm->videobufsize)
        return ERR_OUT_OF_MEMORY;

	memcpy(rm->videobuf+rm->videobufpos,buf,len);

    rm->videobufpos += len;
    rm->remaining_len-= len;
	
    if(type == 2 || (rm->videobufpos) == rm->videobufsize)
	{
		rm->videobuf[0] = rm->cur_slice-1;

		BufferWrite(&p->Buffer,rm->videobuf,1 + 8*rm->cur_slice,1);
		BufferWrite(&p->Buffer,rm->videobuf + 1 + 8*rm->slices,rm->videobufpos - 1 - 8*rm->slices,2048);

		return ERR_NONE;
    }
	
    return ERR_NEED_MORE_DATA;
}
static int Process( ffmpeg_video_real* p, const packet* Packet, const flowstate* State )
{
	int Picture;
	int Len;

	if (Packet)
	{
		if (State->DropLevel)
		{
			if (State->DropLevel>1)
			{
				p->SkipToKey = 1;
				p->DropToKey = 1;
				p->Dropping = 1;
				p->Context->hurry_up = 5;
			}
			else
				p->Context->hurry_up = 1;
			if (!SupportDrop(p))
				p->Context->hurry_up = 0;
		}
		else
			p->Context->hurry_up = 0;

		if (!Packet->Key && p->DropToKey)
		{
			if (p->Dropping)
			{
				flowstate DropState;
				DropState.CurrTime = TIME_UNKNOWN;
				DropState.DropLevel = 1;
				p->Codec.Out.Process(p->Codec.Out.Pin.Node,NULL,&DropState);
			}
			if (SupportDrop(p))
				avcodec_flush_buffers(p->Context);
			return ERR_DROPPING;
		}

		if (p->DropToKey)
			p->DropToKey = 0;

		if (Packet->RefTime >= 0 && p->RefTime < 0)
			p->RefTime = Packet->RefTime;
		/*	if (Packet->RefTime >= 0)
			p->Codec.Packet.RefTime = Packet->RefTime;
*/
		DEBUG_MSG1(-1,T("Packet->RefTime %d"),Packet->RefTime);

		BufferPack(&p->Buffer,0);
		
		if(p->Codec.In.Pin.Node->Class==FOURCC('R','V','_','0')&&
			(p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','1','0')||
			p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','2','0')||
			p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','3','0')||
			p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','4','0')))
		{
		
				int32_t ret = rm_assemble_video_frame(p,Packet);
				if(ret != ERR_NONE)
					return ERR_NEED_MORE_DATA;
				else
				{
					DEBUG_MSG1(-1,T("VPASS:Packet->RefTime %d"),Packet->RefTime);
					p->Codec.Packet.RefTime = p->RefTime;
				}
		}
		else
		{
			BufferWrite(&p->Buffer,Packet->Data[0],Packet->Length,2048);//for other demuxer eg.mkv rv10-rv40 demuxer;
			p->Codec.Packet.RefTime = Packet->RefTime;
		}
	}
	else
	{
		if (p->FrameTime<0)
			p->Codec.Packet.RefTime = TIME_UNKNOWN;
		else
		if (!State)
			p->Codec.Packet.RefTime += p->FrameTime;

		if (!State && p->Buffer.WritePos == p->Buffer.ReadPos)
			return ERR_NEED_MORE_DATA;
	}

	if (p->SkipToKey)
		p->Picture->pict_type = 0;

	Len = avcodec_decode_video(p->Context, p->Picture, &Picture, p->Buffer.Data + p->Buffer.ReadPos, 
		p->Buffer.WritePos - p->Buffer.ReadPos);

	if (Len < 0)
	{
		BufferDrop(&p->Buffer);
		return ERR_INVALID_DATA;
	}

	p->Buffer.ReadPos += Len;

	if (!Picture)
	{
		if (p->SkipToKey>1 && p->Picture->pict_type)
			--p->SkipToKey;

		return ERR_NONE;
	}

	if (p->SkipToKey>0)
	{
		if ((!p->Picture->key_frame && p->Picture->pict_type) || p->SkipToKey>1)
		{
			if (p->SkipToKey>1)
				--p->SkipToKey;
			if (p->Dropping)
			{
				flowstate DropState;
				DropState.CurrTime = TIME_UNKNOWN;
				DropState.DropLevel = 1;
				p->Codec.Out.Process(p->Codec.Out.Pin.Node,NULL,&DropState);
				p->RefTime = TIME_UNKNOWN;
			}
			return ERR_DROPPING;
		}
		p->SkipToKey = 0;
	}

	if (p->Context->pix_fmt != p->PixelFormat ||
		p->Context->sample_aspect_ratio.num != p->Aspect.num ||
		p->Context->sample_aspect_ratio.den != p->Aspect.den ||
		p->Context->width != p->Codec.Out.Format.Format.Video.Width ||
		p->Context->height != p->Codec.Out.Format.Format.Video.Height ||
		p->Picture->linesize[0] != p->Codec.Out.Format.Format.Video.Pitch)
	{
		if (!BuildOutputFormat(p))
			return ERR_INVALID_DATA;

		ConnectionUpdate(&p->Codec.Node,CODEC_OUTPUT,p->Codec.Out.Pin.Node,p->Codec.Out.Pin.No);
	}

	p->Codec.Packet.Data[0] = p->Picture->data[0];
	p->Codec.Packet.Data[1] = p->Picture->data[1];
	p->Codec.Packet.Data[2] = p->Picture->data[2];
	p->RefTime = TIME_UNKNOWN;
	return ERR_NONE;
}

static int ReSend( ffmpeg_video_real* p )
{
	flowstate State;

	if (p->SkipToKey || !p->Codec.Out.Pin.Node || !p->Picture->data[0])
		return ERR_NONE;

	State.CurrTime = TIME_RESEND;
	State.DropLevel = 0;

	p->Codec.Packet.RefTime = TIME_UNKNOWN;
	p->Codec.Packet.Data[0] = p->Picture->data[0];
	p->Codec.Packet.Data[1] = p->Picture->data[1];
	p->Codec.Packet.Data[2] = p->Picture->data[2];

	return p->Codec.Out.Process(p->Codec.Out.Pin.Node,&p->Codec.Packet,&State);
}

static int Flush( ffmpeg_video_real* p )
{
	p->SkipToKey = 1;
	p->DropToKey = 1;
	p->Dropping = 0;
	if (p->Context)
	{
		if (SupportDrop(p))
			avcodec_flush_buffers(p->Context);
		if (p->Context->has_b_frames && p->Context->frame_number>0)
			p->SkipToKey = 2;
		if((p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','1','0')||
			p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','2','0')||
			p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','3','0')||
			p->Codec.In.Format.Format.Video.Pixel.FourCC == FOURCC('R','V','4','0')))
		{
			p->SkipToKey = 1;
		}
	}
	BufferDrop(&p->Buffer);
	return ERR_NONE;
}

static int Set(ffmpeg_video_real* p, int No, const void* Data, int Size)
{
	int Result = CodecSet(&p->Codec,No,Data,Size);
	switch (No)
	{
	case NODE_SETTINGSCHANGED: 
		if (p->Context)
			UpdateSettings(p);
		break;
	}
	return Result;
}

static int Create( ffmpeg_video_real* p )
{
	p->rm.videobuf = NULL;
	p->Codec.Node.Set = (nodeset)Set;
	p->Codec.Process = (packetprocess)Process;
	p->Codec.UpdateInput = (nodefunc)UpdateInput;
	p->Codec.Flush = (nodefunc)Flush;
	p->Codec.ReSend = (nodefunc)ReSend;
	p->Codec.NoHardDropping = 1;
	return ERR_NONE;
}

static const nodedef FFMPEGRealVideo =
{
	sizeof(ffmpeg_video_real)|CF_ABSTRACT,
	FFMPEG_VIDEO_REAL_CLASS,
	CODEC_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
	NULL,
};

#define REGISTER_DECODER(X,x) { \
          extern AVCodec x##_decoder; \
          if(CONFIG_##X##_DECODER)  avcodec_register(&x##_decoder); }

void FFMPEGV_Real_Init()
{
	nodedef Def;
	const codecinfo* i;

	avcodec_init();

	/* video codecs */
	REGISTER_DECODER (RV10, rv10);
    REGISTER_DECODER (RV20, rv20);
    REGISTER_DECODER (RV30, rv30);
    REGISTER_DECODER (RV40, rv40);  
    
	NodeRegisterClass(&FFMPEGRealVideo);
	memset(&Def,0,sizeof(Def));

	for (i=Info;i->Id;++i)
	{
		StringAdd(1,i->Id,NODE_NAME,i->Name);
		StringAdd(1,i->Id,NODE_CONTENTTYPE,i->ContentType);

		Def.Class = i->Id;
		Def.ParentClass = FFMPEG_VIDEO_REAL_CLASS;
		Def.Priority = PRI_DEFAULT-10; // do not override ARM optimized codecs by default
		Def.Flags = 0; // parent size

		NodeRegisterClass(&Def);
	}
}

void FFMPEGV_Real_Done()
{
	NodeUnRegisterClass(FFMPEG_VIDEO_REAL_CLASS);
}
#else
void FFMPEGV_Real_Init(){}
void FFMPEGV_Real_Done(){}
#endif