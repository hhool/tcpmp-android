/*****************************************************************************
 *
 * This program is free software ; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * The Core Pocket Media Player (Non official plugin)
 * Copyright (c) 2007-2008 Thomas
 *
 ****************************************************************************/
#include "../network/http.h"
#include "../common/common.h"
#include "flv.h"
#include "math.h"


/* offsets for packed values */
#define FLV_AUDIO_SAMPLESSIZE_OFFSET 1
#define FLV_AUDIO_SAMPLERATE_OFFSET  2
#define FLV_AUDIO_CODECID_OFFSET     4

#define FLV_VIDEO_FRAMETYPE_OFFSET   4

/* bitmasks to isolate specific values */
#define FLV_AUDIO_CHANNEL_MASK    0x01
#define FLV_AUDIO_SAMPLESIZE_MASK 0x02
#define FLV_AUDIO_SAMPLERATE_MASK 0x0c
#define FLV_AUDIO_CODECID_MASK    0xf0

#define FLV_VIDEO_CODECID_MASK    0x0f
#define FLV_VIDEO_FRAMETYPE_MASK  0xf0

#define AMF_END_OF_OBJECT         0x09

#define SAFETAIL 256

enum 
{
    FLV_HEADER_FLAG_HASVIDEO = 1,
    FLV_HEADER_FLAG_HASAUDIO = 4,
};

enum 
{
    FLV_TAG_TYPE_AUDIO = 0x08,
    FLV_TAG_TYPE_VIDEO = 0x09,
    FLV_TAG_TYPE_META  = 0x12,
};

enum 
{
    FLV_MONO   = 0,
    FLV_STEREO = 1,
};

enum 
{
    FLV_SAMPLESSIZE_8BIT  = 0,
    FLV_SAMPLESSIZE_16BIT = 1 << FLV_AUDIO_SAMPLESSIZE_OFFSET,
};

enum 
{
    FLV_SAMPLERATE_SPECIAL = 0, /**< signifies 5512Hz and 8000Hz in the case of NELLYMOSER */
    FLV_SAMPLERATE_11025HZ = 1 << FLV_AUDIO_SAMPLERATE_OFFSET,
    FLV_SAMPLERATE_22050HZ = 2 << FLV_AUDIO_SAMPLERATE_OFFSET,
    FLV_SAMPLERATE_44100HZ = 3 << FLV_AUDIO_SAMPLERATE_OFFSET,
};

enum 
{
    FLV_CODECID_PCM                  = 0,
    FLV_CODECID_ADPCM                = 1,
    FLV_CODECID_MP3                  = 2,
    FLV_CODECID_PCM_LE               = 3,
    FLV_CODECID_NELLYMOSER_8KHZ_MONO = 5,
    FLV_CODECID_NELLYMOSER           = 6,
    FLV_CODECID_AAC                  = 10,
    FLV_CODECID_SPEEX                = 11,
};

enum 
{
    FLV_CODECID_H263    = 2,	//Sorenson H.263
    FLV_CODECID_SCREEN  = 3,	//Screen video
    FLV_CODECID_VP6     = 4,	//On2 VP6
    FLV_CODECID_VP6A    = 5,	//On2 VP6 with alpha channel
    FLV_CODECID_SCREEN2 = 6,	//Screen video version 2
    FLV_CODECID_H264    = 7,	//H264
};

enum 
{
    FLV_FRAME_KEY        = 1 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_INTER      = 2 << FLV_VIDEO_FRAMETYPE_OFFSET,
    FLV_FRAME_DISP_INTER = 3 << FLV_VIDEO_FRAMETYPE_OFFSET,
};

typedef enum 
{
    AMF_DATA_TYPE_NUMBER      = 0x00,
    AMF_DATA_TYPE_BOOL        = 0x01,
    AMF_DATA_TYPE_STRING      = 0x02,
    AMF_DATA_TYPE_OBJECT      = 0x03,
    AMF_DATA_TYPE_NULL        = 0x05,
    AMF_DATA_TYPE_UNDEFINED   = 0x06,
    AMF_DATA_TYPE_REFERENCE   = 0x07,
    AMF_DATA_TYPE_MIXEDARRAY  = 0x08,
    AMF_DATA_TYPE_OBJECT_END  = 0x09,
    AMF_DATA_TYPE_ARRAY       = 0x0a,
    AMF_DATA_TYPE_DATE        = 0x0b,
    AMF_DATA_TYPE_LONG_STRING = 0x0c,
    AMF_DATA_TYPE_UNSUPPORTED = 0x0d,
} AMFDataType;

typedef struct Tag
{
	uint32_t PreviousTagSize;
	uint8_t	 TagType;
	uint32_t DataSize;
	uint32_t TimeStamp;
	uint32_t StreamID;
}Tag;

typedef struct AudioTag
{
	uint8_t SoundFormat;
	uint8_t SoundRate;
	uint8_t SoundSize;
	uint8_t SoundType;
}AudioTag;
typedef struct VideoTag
{
	uint8_t FrameType;
	uint8_t CodecID;
}VideoTag;

typedef union
{
	int64_t src;
	double dst;
}conv;

typedef struct flvindex
{
	int64_t times;
	int64_t pos;
} flvindex;

typedef struct flv
{
	format_base		Format;
	filepos_t		StartOffset;
	block			IndexBuffer;
	int32_t			IndexNum;
	int32_t			IndexCur;
	bool_t			bHasV;
	bool_t			bHasA;
} flv;

static void Done(flv* p)
{
	FreeBlock(&p->IndexBuffer);
}

static unsigned int ReadBE24(format_reader * Reader)
{
    unsigned int val;
    val = Reader->Read8(Reader) << 16;
    val |= Reader->Read8(Reader) << 8;
    val |= Reader->Read8(Reader);
    return val;
}

static bool_t ReadTag(flv *p,format_reader* Reader,Tag* t)
{
	if(p->Format.FileSize-Reader->FilePos < 15) 
		return 0;

	t->PreviousTagSize = Reader->ReadBE32(Reader);
	t->TagType = Reader->Read8(Reader);
	t->DataSize = ReadBE24(Reader);
	t->TimeStamp = ReadBE24(Reader);
	/*t.Reserved =*/ Reader->ReadBE32(Reader);

	return (p->Format.FileSize-Reader->FilePos) >= t->DataSize;
}

static bool_t ReadATag(flv *p,format_reader* Reader,AudioTag* at)
{
	uint8_t b;

	if(p->Format.FileSize-Reader->FilePos <= 0) 
		return 0;

	b = Reader->Read8(Reader);
	at->SoundFormat =  (b>>4)&0xf;; //4:2:2:1
	at->SoundRate	=  ((b<<4)>>6)&3;
	at->SoundSize	=  ((b<<6)>>7)&1;
	at->SoundType	=  (b&1);

	return 1;
}

static bool_t ReadVTag(flv *p,format_reader* Reader,VideoTag* vt)
{
	uint8_t b;

	if(p->Format.FileSize-Reader->FilePos <= 0) 
		return 0;

	b = Reader->Read8(Reader);

	vt->FrameType = (b>>4)&0xf;//4:4
	vt->CodecID	  = (b&0xf);

	return 1;
}

static int ReadHeader(flv *p, format_reader* Reader)
{
	char magic[3];
	int flags;

	if (Reader->Read(Reader, magic, 3) != 3)
		return ERR_END_OF_FILE;

	if (magic[0] != 'F' || magic[1] != 'L' || magic[2] != 'V')
		return ERR_NOT_SUPPORTED;

	Reader->Skip(Reader, 1);

	flags = Reader->Read8(Reader);
	p->StartOffset = Reader->ReadBE32(Reader);

	if (Reader->Input->Class != HTTP_ID) 
	{
		if (Reader->Input->Class != MMS_ID) 
		{
			Reader->Seek(Reader, 0, SEEK_END);
			p->Format.FileSize = Reader->FilePos;
		}
	}
	Reader->Seek(Reader, p->StartOffset, SEEK_SET);

	return ERR_NONE;
}

static flvindex* IndexBuffer(flv* p,int n)
{
	return ((flvindex*)p->IndexBuffer.Ptr)+n;
}
static bool_t  Sync(flv* p,format_reader* Reader,filepos_t pos)
{
	int Type;
	filepos_t next;
	int	TimeStamp, Reserved, DataSize;

	Reader->Seek(Reader,pos,SEEK_SET);

	while(p->Format.FileSize-Reader->FilePos >= 11)
	{
		do
		{
			Type = Reader->Read8(Reader);
		}
		while(Type != 8 && Type != 9 && !Reader->Eof(Reader));

		pos = Reader->FilePos;

		DataSize = ReadBE24(Reader);
		TimeStamp= ReadBE24(Reader);
		Reserved = Reader->ReadBE32(Reader);

		next = Reader->FilePos + DataSize;

		if(next <= p->Format.FileSize)
		{
			Reader->Seek(Reader,next,SEEK_SET);
			
			if(next == p->Format.FileSize || Reader->ReadBE32(Reader) == DataSize + 11)
			{
				Reader->Seek(Reader,pos - 5,SEEK_SET);

				return 1;
			}
		}
		
		Reader->Seek(Reader, pos, SEEK_SET);
	}

	return 0;
}
static int SeekNoIndex(flv* p, tick_t Time, filepos_t FilePos,bool_t PrevKey)
{
	filepos_t	LastFilePos;
	bool_t		fAudio = p->bHasA;
	bool_t		fVideo = p->bHasV;
	Tag t;
	AudioTag at;
	VideoTag vt;

	format_reader* Reader =	p->Format.Reader;

	if(Time < 0 || p->Format.Duration <= 0)
		return ERR_NOT_SUPPORTED;

	LastFilePos = p->StartOffset + 1.0 * Time/p->Format.Duration*(p->Format.FileSize - p->StartOffset);
	
	if(!Sync(p,Reader,LastFilePos))
	{
		Format_Seek(&p->Format,p->StartOffset,SEEK_SET);
		return ERR_NONE;
	}

	while(ReadTag(p,Reader,&t))
	{
		tick_t TagTime = Scale(t.TimeStamp,TICKSPERSEC,1000);
		
		if(TagTime >= Time)
		{
			LastFilePos = Reader->FilePos - 15;
			Reader->Seek(Reader,Reader->FilePos - 15,SEEK_SET);
			break;
		}
		
		LastFilePos = Reader->FilePos + t.DataSize;
		Reader->Seek(Reader,Reader->FilePos + t.DataSize,SEEK_SET);
	}

	while(Reader->FilePos >= p->StartOffset && (fAudio || fVideo) && ReadTag(p,Reader,&t))
	{
		filepos_t prev = Reader->FilePos - 15 - t.PreviousTagSize - 4;
		
		tick_t TimeStamp = Scale(t.TimeStamp,TICKSPERSEC,1000);

		if(TimeStamp <= Time)
		{
			if(t.TagType == 8 && ReadATag(p,Reader,&at))
			{
				fAudio = 0;
			}
			else if(t.TagType == 9 && ReadVTag(p,Reader,&vt) && vt.FrameType == 1)
			{
				fVideo = 0;
			}
		}

		LastFilePos = prev;

		Reader->Seek(Reader,prev,SEEK_SET);
	}

	if(fAudio || fVideo)
	{
		LastFilePos = p->StartOffset;
		Format_Seek(&p->Format,p->StartOffset,SEEK_SET);
		return ERR_NONE;
	}
	return Format_Seek(&p->Format,LastFilePos,SEEK_SET);
}

static int Seek(flv* p, tick_t Time, filepos_t FilePos,bool_t PrevKey)
{
	int	i=0;
	tick_t preIndexNum = -1;
	if(Time>=0)
	{
		for( i = 0; i < p->IndexNum; i++)
		{			
			tick_t RefTime = Scale(((flvindex*)IndexBuffer(p,i))->times, TICKSPERSEC, 1);
			if(Time<=RefTime)
				break;	
			preIndexNum = i;
		}
	}
	if(PrevKey&&preIndexNum!=-1)
		i = preIndexNum;

	if(p->IndexNum == 0)
	{
		if(Time>=0 && p->Format.Duration > 0)
			return SeekNoIndex(p,Time,FilePos,PrevKey);
		return ERR_NOT_SUPPORTED;
	}
	else if(i >= 0 && i < p->IndexNum)
	{
		FilePos = ((flvindex*)IndexBuffer(p,i))->pos-4;
	}
	else
	{
		FilePos = p->StartOffset;
	}

	return Format_Seek(&p->Format,FilePos,SEEK_SET);
}

static int amf_get_string(flv* p, format_reader* Reader, char *buffer, int buffsize) 
{
	int length = Reader->ReadBE16(Reader);

    if(length >= buffsize) 
	{
		Reader->Skip(Reader,length);
        return -1;
    }

	Reader->Read(Reader,buffer,length);
    
    buffer[length] = '\0';

    return length;
}

static int amf_parse_object(flv* p, format_reader* Reader, const char *key, int64_t max_pos, int depth) 
{
    AMFDataType amf_type;

    char str_val[256];

    conv c;

	double num_val;

    num_val = 0;

	amf_type = Reader->Read8(Reader);

    switch(amf_type) 
	{
        case AMF_DATA_TYPE_NUMBER:
			{
				c.src = Reader->ReadBE64(Reader);
				num_val =c.dst;
			}
			break;
        case AMF_DATA_TYPE_BOOL:
			num_val = Reader->Read8(Reader);
			break;
        case AMF_DATA_TYPE_STRING:
            if(amf_get_string(p,Reader, str_val, sizeof(str_val)) < 0)
                return -1;
            break;
        case AMF_DATA_TYPE_OBJECT: 
			{
				while(Reader->FilePos < max_pos - 2 && amf_get_string(p,Reader, str_val, sizeof(str_val)) > 0)
				{
					if(amf_parse_object(p,Reader, str_val, max_pos, depth + 1) < 0)
					{
						return -1; //if we couldn't skip, bomb out.
					}

				}
				if(Reader->Read8(Reader) != AMF_END_OF_OBJECT)
					return -1;
			}
            break;
        case AMF_DATA_TYPE_NULL:
        case AMF_DATA_TYPE_UNDEFINED:
        case AMF_DATA_TYPE_UNSUPPORTED:
            break; //these take up no additional space
        case AMF_DATA_TYPE_MIXEDARRAY:
			Reader->Skip(Reader,4); //skip 32-bit max array index
			while(Reader->FilePos < max_pos - 2 && amf_get_string(p,Reader, str_val, sizeof(str_val)) > 0) 
			{
                //this is the only case in which we would want a nested parse to not skip over the object
                if(amf_parse_object(p,Reader, str_val, max_pos, depth + 1) < 0)
                    return -1;
            }
			if(Reader->Read8(Reader) != AMF_END_OF_OBJECT)
				return -1;
            break;
        case AMF_DATA_TYPE_ARRAY: 
			{
				unsigned int arraylen, i;

				arraylen = Reader->ReadBE32(Reader);
				if(strcmp(key,"times")==0||strcmp(key,"filepositions")==0)
				{
					if(p->IndexNum == 0)
					{
						int32_t size;
						p->IndexNum  = arraylen;
						size = sizeof(flvindex)*p->IndexNum;
						size = ((size+SAFETAIL-1)/SAFETAIL)*SAFETAIL;
						if (!AllocBlock(size,&p->IndexBuffer,0,HEAP_ANY))
							return ERR_OUT_OF_MEMORY;
					}
				}

				p->IndexCur  = 0;

				for(i = 0; i < arraylen && Reader->FilePos < max_pos - 1; i++) 
				{
					if(amf_parse_object(p,Reader,  key, max_pos, depth + 1) < 0)
						return -1; //if we couldn't skip, bomb out.
				}
        }
            break;
        case AMF_DATA_TYPE_DATE:
			Reader->Skip(Reader, 8 + 2); //timestamp (double) and UTC offset (int16)
            break;
        default: //unsupported type, we couldn't skip
            return -1;
    }

    if(depth == 1 &&key) 
	{	//only look for metadata values when we are not nested and key != NULL
        if(amf_type == AMF_DATA_TYPE_BOOL) 
		{
        } 
		else if(amf_type == AMF_DATA_TYPE_NUMBER) 
		{
            if(!tcscmp(key, "duration")) 
				p->Format.Duration = Scale(num_val, TICKSPERSEC, 1);
        } 
		else if (amf_type == AMF_DATA_TYPE_STRING)
		{
			
		}
		else if(amf_type == AMF_DATA_TYPE_DATE)
		{

		}
    }
	else if(key)
	{
		if(tcscmp(key,"times")==0)
		{
			((flvindex*)IndexBuffer(p,p->IndexCur))->times = num_val;
			p->IndexCur++;
		}
		else if(tcscmp(key,"filepositions")==0)
		{
			((flvindex*)IndexBuffer(p,p->IndexCur))->pos = num_val;
			p->IndexCur++;
		}
	}

    return 0;
}

static int flv_read_metabody(flv* p, format_reader* Reader, int64_t next_pos) 
{
    AMFDataType type;
    char buffer[11]; //only needs to hold the string "onMetaData". Anything longer is something we don't want.

    //first object needs to be "onMetaData" string
	type = Reader->Read8(Reader);

    if(type != AMF_DATA_TYPE_STRING || amf_get_string(p,Reader, buffer, sizeof(buffer)) < 0 || tcscmp(buffer, "onMetaData"))
        return -1;

    //parse the second object (we want a mixed array)
    if(amf_parse_object(p,Reader, buffer, next_pos, 0) < 0)
        return -1;

    return 0;
}
static int ReadPacket(flv* p, format_reader* Reader, format_packet* Packet)
{
	format_stream* s;
    int		i, type, size, TimeStamp, flags, is_audio;
	int64_t next;
	bool_t	isNewStream = 0;
	filepos_t	startfilepos;
	
	startfilepos = Reader->FilePos;

	for (;;) 
	{
		Reader->Skip(Reader, 4);//prvious type size
		type = Reader->Read8(Reader);
		size = ReadBE24(Reader);
		TimeStamp = ReadBE24(Reader);
		if (Reader->Eof(Reader))
		{
			return ERR_END_OF_FILE;
		}

		// Reserved
		Reader->Skip(Reader, 4);

		flags = 0;

		if (size == 0)
			continue;

		next = size + Reader->FilePos;

		if (type == FLV_TAG_TYPE_AUDIO)
		{
			is_audio = 1;
			flags = Reader->Read8(Reader);
			size--;
			p->bHasA = 1;
		} 
		else if (type == FLV_TAG_TYPE_VIDEO)
		{
			is_audio = 0;
			flags = Reader->Read8(Reader);
			size--;
			p->bHasV = 1;
		}
		else if (type == FLV_TAG_TYPE_META && size > 13+1+4)
		{
			if(flv_read_metabody(p,Reader, next)<0)
			{
				Reader->Seek(Reader,(filepos_t)next,SEEK_SET);
			}
			continue;
		}
		else
		{
			// Skip the packet
			Reader->Skip(Reader, size);
			continue;
		}

		// Now find stream
		s = NULL;
		for (i=0;i<p->Format.StreamCount;++i)
		{
			if (p->Format.Streams[i]->Id == is_audio)
			{
				s = p->Format.Streams[i];
				break;
			}
		}

		if (!s)
		{
			s = Format_AddStream(&p->Format,sizeof(format_stream));
	        if (!s)
		        return ERR_OUT_OF_MEMORY;

			s->Id = is_audio;
			isNewStream = 1;

			s->Format.PacketRate.Den = 24;
			s->Format.PacketRate.Num = 1000;
		}

		break;
	}

    if(is_audio)
	{
		if (isNewStream)
		{
			int bInited = 0;

			PacketFormatClear(&s->Format);
			s->Format.Type = PACKET_AUDIO;
			s->Format.Format.Audio.Channels = (flags&1)+1;
			if((flags >> 4) == 5)
				s->Format.Format.Audio.SampleRate = 8000;
			else
				s->Format.Format.Audio.SampleRate = (44100<<((flags>>2)&3))>>3;

			switch(flags >> 4)
			{/* 0: uncompressed 1: ADPCM 2: mp3 5: Nellymoser 8kHz mono 6: Nellymoser */
			case FLV_CODECID_PCM:
				s->Format.Format.Audio.Format = AUDIOFMT_PCM;
				if (!(flags&2))
				{
					s->Format.Format.Audio.Bits = 8;
					s->Format.Format.Audio.FracBits = 7;
					bInited = 1;
				}
				break;
			case FLV_CODECID_ADPCM:
				s->Format.Format.Audio.Format = AUDIOFMT_ADPCM_SWF;
				break;
			case FLV_CODECID_MP3: 
				s->Format.Format.Audio.Format = AUDIOFMT_MP3;
				break;
			case FLV_CODECID_PCM_LE:
				s->Format.Format.Audio.Format = AUDIOFMT_PCM;
				if (!(flags&2))
				{
					s->Format.Format.Audio.Bits = 8;
					s->Format.Format.Audio.FracBits = 7;
					bInited = 1;
				}
				break;
			case FLV_CODECID_NELLYMOSER_8KHZ_MONO:
				s->Format.Format.Audio.SampleRate = 8000;
			case FLV_CODECID_NELLYMOSER:
				s->Format.Format.Audio.Format = AUDIOFMT_NELLYMOSER;
				break;
			case FLV_CODECID_AAC:
				{
					format_stream* s;
					int type = Reader->Read8(Reader);
					size--;
					if(type == 0)
					{
						s = p->Format.Streams[p->Format.StreamCount-1];

						if (PacketFormatExtra(&s->Format,size))
							Reader->Read(Reader,s->Format.Extra,size);
					}
					s->Format.Format.Audio.Format = AUDIOFMT_AAC;
					Format_PrepairStream(&p->Format, s);

				}
				return ERR_NONE;
			case FLV_CODECID_SPEEX:
				s->Format.Format.Audio.Format = AUDIOFMT_SPEEX;
				s->Format.Format.Audio.SampleRate = 16000;
				break;
			default:
				return ERR_NOT_SUPPORTED;
			}
			if(!bInited)
			{
				s->Format.Format.Audio.Bits = 16;
				s->Format.Format.Audio.FracBits = 15;
			}
			s->Format.ByteRate = s->Format.Format.Audio.SampleRate * s->Format.Format.Audio.Channels * 2;
			Format_PrepairStream(&p->Format,s);
		}
    }
	else
	{
		if (isNewStream)
		{
			PacketFormatClear(&s->Format);
			s->Format.Type = PACKET_VIDEO;
			s->Format.Format.Video.Pixel.Flags = PF_FOURCC | PF_FRAGMENTED;
			switch(flags & 0xF)
			{
			case FLV_CODECID_H263: 
				s->Format.Format.Video.Pixel.FourCC = FOURCC('F','L','V','1');
				break;
			case FLV_CODECID_SCREEN:
				s->Format.Format.Video.Pixel.FourCC = FOURCC('F','L','S','1');
				break;
			case FLV_CODECID_VP6:
				s->Format.Format.Video.Pixel.FourCC = FOURCC('V','P','6','F');
				break;
			case FLV_CODECID_VP6A:
				s->Format.Format.Video.Pixel.FourCC = FOURCC('V','P','6','A');
				break;
			case FLV_CODECID_SCREEN2:
				return ERR_NOT_SUPPORTED;
			case FLV_CODECID_H264:
				{
					format_stream* s;
					int type = Reader->Read8(Reader);
					size--;
					Reader->Read8(Reader);
					Reader->Read8(Reader);
					Reader->Read8(Reader);
					size-=3;
					if(type == 0)
					{
						s = p->Format.Streams[p->Format.StreamCount-1];

						if (PacketFormatExtra(&s->Format,size))
							Reader->Read(Reader,s->Format.Extra,size);
					}
					s->Format.Format.Video.Pixel.FourCC = FOURCC('H','2','6','4');

					Format_PrepairStream(&p->Format, s);

					return ERR_NONE;
					
				}
				break;
			default:
				return ERR_NOT_SUPPORTED;
			}
			Format_PrepairStream(&p->Format, s);

		}
    }

	Packet->RefTime = Scale(TimeStamp, TICKSPERSEC, 1000);
	Packet->Stream = s;

	if (!is_audio && ((flags & 0xF) == FLV_CODECID_VP6 || (flags & 0xF) == FLV_CODECID_VP6A))
	{
		Reader->Seek(Reader, 1, SEEK_CUR);
		Packet->Data = Reader->ReadAsRef(Reader, size - 1);
	}
	else if (!is_audio && (flags & 0xF) == FLV_CODECID_H264)
	{
		Reader->Seek(Reader, 4, SEEK_CUR);
		Packet->Data = Reader->ReadAsRef(Reader, size - 4);
	}
	else if(is_audio && (flags >> 4) == FLV_CODECID_AAC)
	{
		Reader->Seek(Reader, 1, SEEK_CUR);
		Packet->Data = Reader->ReadAsRef(Reader, size - 1);
	}
	else
	{
		Packet->Data = Reader->ReadAsRef(Reader, size);
	}

    if (!is_audio)
	{
		Packet->Key = (flags >> 4)==1;
	}

	return ERR_NONE;
}

static int Init(flv* p)
{
	int errcode;

	format_reader* Reader =	p->Format.Reader;
	
	errcode = ReadHeader(p, Reader);
	if (ERR_NONE != errcode)
		return errcode;

	p->IndexNum = 0;
	
	return ERR_NONE;
}

static int Create(flv* p)
{
	p->Format.Init = (fmtfunc)Init;
	p->Format.Seek = (fmtseek)Seek;
	p->Format.ReadPacket = (fmtreadpacket)ReadPacket;
	p->Format.Done = (fmtvoid)Done;
	return ERR_NONE;
}

static const nodedef FLV =
{
	sizeof(flv),
	FLV_ID,
	FORMATBASE_CLASS,
	PRI_DEFAULT-10,		// lower priority so others can override
	(nodecreate)Create,
	NULL,
};

void FLV_Init()
{
	StringAdd(1, FLV.Class, NODE_NAME, T("Flash Video"));
	StringAdd(1, FLV.Class, NODE_EXTS, T("flv:V;f4v:V;f4a:A"));
	StringAdd(1, FLV.Class, NODE_CONTENTTYPE, T("video/flv,video/x-flv"));
	NodeRegisterClass(&FLV);
}

void FLV_Done()
{
	NodeUnRegisterClass(FLV_ID);
}