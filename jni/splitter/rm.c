#include "../common/common.h"
#include "rm.h"
#ifdef ENABLE_REAL
typedef struct rmindex
{
	int64_t times;
	int64_t pos;
} rmindex;

#define KEY_NULL_STREAM  0x0000
#define KEY_VIDEO_STREAM 0x0001
#define KEY_AUDIO_STREAM 0x0002

typedef struct rm
{
	format_base			Format;
	
	tick_t				Timestamp;
	filepos_t			IndexOffset;
	filepos_t			DataOffset;
	block				IndexBuffer;
	int32_t				IndexNum;
	uint32_t			StreamFlag;

    int					old_format;
	   
    /// Audio descrambling matrix parameters
    int					sub_packet_size, sub_packet_h, coded_framesize; ///< Descrambling parameters from container
    int					audio_framesize; /// Audio frame size from container
    int					sub_packet_lengths[16]; /// Length of each aac subpacket
	int					block_align;

	int					sample_rate;
	int					channels;
} rm;

#define AV_WL32(p, d) do { \
	((uint8_t*)(p))[0] = (d); \
	((uint8_t*)(p))[1] = (d)>>8; \
	((uint8_t*)(p))[2] = (d)>>16; \
                    ((uint8_t*)(p))[3] = (d)>>24; } while(0)

#define META_NUM 4

const static tchar_t * ff_rm_metadata[META_NUM] = 
{
    T("title"),
    T("author"),
    T("copyright"),
    T("comment")
};
/*
const AVCodecTag ff_rm_codec_tags[] = 
{
    { CODEC_ID_RV10,   MKTAG('R','V','1','0') },
    { CODEC_ID_RV20,   MKTAG('R','V','2','0') },
    { CODEC_ID_RV20,   MKTAG('R','V','T','R') },
    { CODEC_ID_RV30,   MKTAG('R','V','3','0') },
    { CODEC_ID_RV40,   MKTAG('R','V','4','0') },
    { CODEC_ID_AC3,    MKTAG('d','n','e','t') },
    { CODEC_ID_RA_144, MKTAG('l','p','c','J') },
    { CODEC_ID_RA_288, MKTAG('2','8','_','8') },
    { CODEC_ID_COOK,   MKTAG('c','o','o','k') },
    { CODEC_ID_ATRAC3, MKTAG('a','t','r','c') },
    { CODEC_ID_SIPR,   MKTAG('s','i','p','r') },
    { CODEC_ID_AAC,    MKTAG('r','a','a','c') },
    { CODEC_ID_AAC,    MKTAG('r','a','c','p') },
    { CODEC_ID_NONE },
};
*/

static const unsigned char sipr_swaps[38][2] = 
{
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

static const unsigned char ff_sipr_subpk_size[4] = { 29, 19, 37, 20 };

#define FF_INPUT_BUFFER_PADDING_SIZE 8

static rmindex* IndexBuffer(rm* p,int n)
{
	return ((rmindex*)p->IndexBuffer.Ptr)+n;
}

static inline void get_strl(rm *p, format_reader* Reader, char *buf, int buf_size, int len)
{
    int i;
    char *q, r;

    q = buf;
    for(i=0;i<len;i++)
	{
        r = Reader->Read8(Reader);
        if (i < buf_size - 1)
            *q++ = r;
    }
    if (buf_size > 0) *q = '\0';
}

static void get_str8(rm *p, format_reader* Reader,char *buf, int buf_size)
{
	int byte = Reader->Read8(Reader);
    
	get_strl(p, Reader,buf, buf_size, byte);
}
static void AddComment(pin* Comment,tchar_t* Name,char* Value) 
{
	if (Comment->Node && Value)
	{
		tchar_t s[256];
		tcscpy_s(s,TSIZEOF(s),Name);
		tcscat_s(s,TSIZEOF(s),T("="));
		tcscpy_s(s+tcslen(s),TSIZEOF(s)-tcslen(s),Value);
		Comment->Node->Set(Comment->Node,Comment->No,s,sizeof(s));
	}
}

static void rm_read_metadata(rm *p, format_reader *Reader, int wide)
{
    char buf[1024];
    int i;
	
    for (i=0; i<META_NUM; i++)
	{
        int len = wide ? Reader->ReadBE16(Reader) : Reader->Read8(Reader);
        get_strl(p,Reader, buf, sizeof(buf), len);
		AddComment(&p->Format.Comment,(tchar_t*)ff_rm_metadata[i],buf);
    }
}
static int rm_read_extradata(rm *p, format_reader *Reader, unsigned Length)
{
	format_stream* s;

    if (Length >= 1<<24)
        return ERR_INVALID_DATA;

	s = p->Format.Streams[p->Format.StreamCount-1];

	if (PacketFormatExtra(&s->Format,Length))
		Reader->Read(Reader,s->Format.Extra,Length);

    return ERR_NONE;
}

static int rm_read_audio_stream_info(rm *p, format_reader *Reader,int read_all)
{
    char			buf[256];
    uint32_t		version;
    int				ret;
	format_stream*	s= p->Format.Streams[p->Format.StreamCount-1];

    /* ra type header */
    version = Reader->ReadBE16(Reader); /* version */
    if (version == 3) 
	{
        int header_size = Reader->ReadBE16(Reader);
        int64_t startpos = Reader->FilePos;
        Reader->Skip(Reader,14);//url_fskip(pb, 14);
        rm_read_metadata(p,Reader, 0);
        if ((startpos + header_size) >= Reader->FilePos + 2) 
		{
            // fourcc (should always be "lpcJ")
            Reader->Read8(Reader);
            get_str8(p,Reader,buf, sizeof(buf));
        }
        // Skip extra header crap (this should never happen)
        if ((startpos + header_size) > Reader->FilePos)
            Reader->Skip(Reader, header_size + startpos - Reader->FilePos);
       
		p->sample_rate = 8000;
        p->channels = 1;
        s->Format.Format.Audio.Format = AUDIOFMT_RA14;
    } 
	else
	{
        int flavor;
        int codecdata_length;
		uint32_t codec_tag;
        /* old version (4) */
        Reader->Skip(Reader,2);//url_fskip(pb, 2); /* unused */
        Reader->ReadBE32(Reader);//get_be32(pb); /* .ra4 */
        Reader->ReadBE32(Reader);//get_be32(pb); /* data size */
        Reader->ReadBE16(Reader);//get_be16(pb); /* version2 */
        Reader->ReadBE32(Reader);//get_be32(pb); /* header size */
        flavor= Reader->ReadBE16(Reader);//get_be16(pb); /* add codec info / flavor */
        p->coded_framesize = Reader->ReadBE32(Reader);//get_be32(pb); /* coded frame size */
        Reader->ReadBE32(Reader);//get_be32(pb); /* ??? */
        Reader->ReadBE32(Reader);//get_be32(pb); /* ??? */
        Reader->ReadBE32(Reader);//get_be32(pb); /* ??? */
        p->sub_packet_h = Reader->ReadBE16(Reader);//get_be16(pb); /* 1 */
        p->block_align= Reader->ReadBE16(Reader);//get_be16(pb); /* frame size */
        p->sub_packet_size = Reader->ReadBE16(Reader);//get_be16(pb); /* sub packet size */
        Reader->ReadBE16(Reader);//get_be16(pb); /* ??? */
        if (version == 5)
		{
			Reader->ReadBE16(Reader);
			Reader->ReadBE16(Reader);
			Reader->ReadBE16(Reader);
        }
        p->sample_rate = Reader->ReadBE16(Reader);//get_be16(pb);
        Reader->ReadBE32(Reader);//get_be32(pb);
        p->channels = Reader->ReadBE16(Reader);//get_be16(pb);

        if (version == 5)
		{
            Reader->ReadBE32(Reader);//get_be32(pb);
			Reader->Read(Reader,buf,4);
        } 
		else 
		{
            get_str8(p, Reader,buf, sizeof(buf)); /* desc */
            get_str8(p, Reader,buf, sizeof(buf)); /* desc */
        }

        codec_tag  = buf[0]<<0|buf[1]<<8|buf[2]<<16|buf[3]<<24;
	
        switch (codec_tag) 
		{
        case FOURCC('d','n','e','t')://CODEC_ID_AC3
			s->Format.Format.Audio.Format = AUDIOFMT_A52;
            break;
		case FOURCC('l','p','c','J')://CODEC_ID_RA_144
			s->Format.Format.Audio.Format = AUDIOFMT_RA14;
			break;
        case FOURCC('2','8','_','8')://CODEC_ID_RA_288
			s->Format.Format.Audio.Format = AUDIOFMT_RA28;
			p->audio_framesize = p->block_align;
            break;
        case FOURCC('c','o','o','k')://CODEC_ID_COOK:
        case FOURCC('a','t','r','c')://CODEC_ID_ATRAC3:
       //case FOURCC('s','i','p','r')://CODEC_ID_SIPR: ffmpeg not soupport
			if(codec_tag == FOURCC('c','o','o','k'))
			{
				s->Format.Format.Audio.Format = AUDIOFMT_COOK;
			}
			else if(codec_tag == FOURCC('a','t','r','c'))
			{
				s->Format.Format.Audio.Format = AUDIOFMT_ATRC;
			}
			else 
			{
				return ERR_NOT_SUPPORTED;
			}

            Reader->ReadBE16(Reader); Reader->Read8(Reader);
            if (version == 5)
                Reader->Read8(Reader);
            codecdata_length = Reader->ReadBE32(Reader);
            if(codecdata_length + FF_INPUT_BUFFER_PADDING_SIZE <= (unsigned)codecdata_length)
			{
                return ERR_INVALID_DATA;
            }

            p->audio_framesize = p->block_align;
            if (codec_tag == FOURCC('s','i','p','r'))
			{
                if (flavor > 3) 
				{
                    return ERR_INVALID_DATA;
                }
                p->block_align = ff_sipr_subpk_size[flavor];
            } 
			else
			{
                if(p->sub_packet_size <= 0)
				{
                    return ERR_INVALID_DATA;
                }
                p->block_align = p->sub_packet_size;
            }

			{
				format_stream* s;

				int32_t RealExtralLen = codecdata_length;
				codecdata_length+=16;
				
				if (codecdata_length >= 1<<24)
					return ERR_INVALID_DATA;
				
				s = p->Format.Streams[p->Format.StreamCount-1];
				
				if (PacketFormatExtra(&s->Format,codecdata_length))
				{
					uint32_t* pExtra = (uint32_t*)s->Format.Extra;
					pExtra[0] = p->sub_packet_size;
					pExtra[1] = p->coded_framesize;
					pExtra[2] = p->sub_packet_h;
					pExtra[3] = p->audio_framesize;
					pExtra+=4;

					Reader->Read(Reader,pExtra,RealExtralLen);
				}

			}
            if(p->audio_framesize >= 0xffffffff / p->sub_packet_h)
			{
                return ERR_INVALID_DATA;
            }

            break;
        case FOURCC('r','a','a','c')://CODEC_ID_AAC:
		case FOURCC('r','a','c','p'):
			Reader->ReadBE16(Reader);
			Reader->Read8(Reader);
         
            if (version == 5)
                Reader->Read8(Reader);
            codecdata_length = Reader->ReadBE32(Reader);
            if(codecdata_length + FF_INPUT_BUFFER_PADDING_SIZE <= (unsigned)codecdata_length)
			{
                return ERR_INVALID_DATA;
            }
            if (codecdata_length >= 1) 
			{
                Reader->Read8(Reader);
                if ((ret = rm_read_extradata(p, Reader,codecdata_length - 1)) < 0)
                    return ret;
            }
			s->Format.Format.Audio.Format = AUDIOFMT_AAC;
            break;
        default:
            return ERR_NOT_SUPPORTED;
        }
        if (read_all)
		{
			Reader->Read8(Reader);
			Reader->Read8(Reader);
			Reader->Read8(Reader);
        }
    }
    return ERR_NONE;
}

static int rm_read_mdpr_codecdata (rm *p, format_reader *Reader)
{
    unsigned int	v;
    int				size;
    int64_t			codec_pos;
	int				i;
	format_stream*	s;
	int32_t			ByteRate;
	char			buf[256];
	int				codec_data_size;
	int				Id;
	int32_t			ret = ERR_NONE;

	Id = Reader->ReadBE16(Reader); /*id*/
	Reader->ReadBE32(Reader); /* max bit rate */
	ByteRate = Reader->ReadBE32(Reader)/8; /* bit rate */
	Reader->ReadBE32(Reader); /* max packet size */
	Reader->ReadBE32(Reader); /* avg packet size */
	p->Format.StartTime = Reader->ReadBE32(Reader); /* start time */
	Reader->ReadBE32(Reader); /* preroll */
	Reader->ReadBE32(Reader); /* duration */
	get_str8(p, Reader,buf, sizeof(buf)); /* desc */
	get_str8(p, Reader,buf, sizeof(buf)); /* mimetype */

	codec_data_size = Reader->ReadBE32(Reader);

	for (i=0;i<p->Format.StreamCount;++i)
	{
		if (p->Format.Streams[i]->Id == Id)
			return ERR_INVALID_DATA;
	}
   // av_set_pts_info(st, 64, 1, 1000);
    codec_pos = Reader->FilePos;
    v = Reader->ReadBE32(Reader);

    if (v == FOURCC(0xfd, 'a', 'r', '.')) 
	{
		s = Format_AddStream(&p->Format,sizeof(format_stream));
        /* ra type header */
        if (s)
		{
			s->Id		   = Id;
			PacketFormatClear(&s->Format);
			s->Format.Type = PACKET_AUDIO;
			s->Format.ByteRate = ByteRate;
		}
		
		if(rm_read_audio_stream_info(p, Reader, 0))
			return ERR_INVALID_DATA;

		s->Format.Format.Audio.BlockAlign = p->block_align;
		s->Format.Format.Audio.Channels = p->channels;
		s->Format.Format.Audio.SampleRate = p->sample_rate;
		Format_PrepairStream(&p->Format,s);
    } 
	else
	{
        int fps, fps2;
		uint32_t codec_tag;
		int width,height;
        if ( Reader->ReadLE32(Reader) != FOURCC('V', 'I', 'D', 'O')) 
		{
            goto skip;
        }

        codec_tag = Reader->ReadLE32(Reader);

        width = Reader->ReadBE16(Reader);
        height = Reader->ReadBE16(Reader);
        fps= Reader->ReadBE16(Reader);
        
        Reader->ReadBE32(Reader);
        fps2= Reader->ReadBE16(Reader);
        Reader->ReadBE16(Reader);

		s = Format_AddStream(&p->Format,sizeof(format_stream));
        /* ra type header */
        if (s)
		{
			s->Id		   = Id;
			PacketFormatClear(&s->Format);
			s->Format.Type = PACKET_VIDEO;
			s->Format.Format.Video.Width = width;
			s->Format.Format.Video.Height= height;
			s->Format.Format.Video.Pixel.Flags = PF_FOURCC | PF_FRAGMENTED;
			s->Format.ByteRate = ByteRate;
		
			if ((ret = rm_read_extradata(p, Reader, codec_data_size - (Reader->FilePos - codec_pos))) < 0)
				return ERR_INVALID_DATA;

			
			switch(*((uint8_t*)s->Format.Extra+4)>>4)
			{
				case 1: 
					s->Format.Format.Video.Pixel.FourCC = FOURCC('R','V','1','0');
					break;
				case 2: 
					s->Format.Format.Video.Pixel.FourCC = FOURCC('R','V','2','0');
					break;
				case 3: 
					s->Format.Format.Video.Pixel.FourCC = FOURCC('R','V','3','0');
					break;
				case 4: 
					s->Format.Format.Video.Pixel.FourCC = FOURCC('R','V','4','0');
					break;
				default:  
					{
						ret = ERR_NOT_SUPPORTED;
						goto skip;
					}
			}
			
		}
		Format_PrepairStream(&p->Format,s);
    }

skip:
    /* skip codec info */
    size = Reader->FilePos - codec_pos;
    Reader->Seek(Reader, codec_data_size - size,SEEK_CUR);

    return ret;
}
static int rm_read_index(rm *p,format_reader* Reader)
{
    unsigned int size, n_pkts, str_id, next_off, n, pos, pts;
	format_stream* s;
    do 
	{
        if (Reader->ReadLE32(Reader) != FOURCC('I','N','D','X'))
            return ERR_INVALID_DATA;
        
		size = Reader->ReadBE32(Reader);
        if (size < 20)
            return -1;

		Reader->Skip(Reader,2);
        
        n_pkts   = Reader->ReadBE32(Reader);
        str_id   = Reader->ReadBE16(Reader);
        next_off = Reader->ReadBE32(Reader);

        for (n = 0; n < p->Format.StreamCount; n++)
		{
			if (p->Format.Streams[n]->Id == str_id) 
			{
                s = p->Format.Streams[n];
                break;
            }
		}

		if (n == p->Format.StreamCount)
			goto skip1;
		
		if(s->Format.Type == PACKET_VIDEO)
		{
			p->StreamFlag = KEY_VIDEO_STREAM;
		}
		else if(s->Format.Type == PACKET_AUDIO)
		{
			if(p->StreamFlag == KEY_VIDEO_STREAM)
			{
				goto skip1;
			}
			p->StreamFlag = KEY_AUDIO_STREAM;
		}

		if(p->StreamFlag&KEY_VIDEO_STREAM&&s->Format.Type == PACKET_VIDEO)
		{
			FreeBlock(&p->IndexBuffer);
			p->IndexNum = 0;
		}
		else if(p->StreamFlag==KEY_AUDIO_STREAM&&s->Format.Type == PACKET_AUDIO)
		{
			FreeBlock(&p->IndexBuffer);
			p->IndexNum = 0;
		}
		if(p->IndexNum == 0)
		{
			int32_t size;
			p->IndexNum  = n_pkts;
			size = sizeof(rmindex)*p->IndexNum;
			size = ((size+SAFETAIL-1)/SAFETAIL)*SAFETAIL;
			if (!AllocBlock(size,&p->IndexBuffer,0,HEAP_ANY))
				return ERR_OUT_OF_MEMORY;

			for (n = 0; n < n_pkts; n++) 
			{
				Reader->Skip(Reader,2);
				pts = Reader->ReadBE32(Reader);
				pos = Reader->ReadBE32(Reader);
				Reader->Skip(Reader,4); /* packet no. */
				
				
				((rmindex*)IndexBuffer(p,n))->times = pts;
				((rmindex*)IndexBuffer(p,n))->pos   = pos;
				DEBUG_MSG4(-1,T("Id %d, Pos %d,times %d,Reftime %d"),str_id,pos,pts,(tick_t)Scale(pts, TICKSPERSEC, 1000));
			}
		}
skip1:
		if (next_off && Reader->FilePos != next_off &&Reader->Seek(Reader,next_off,SEEK_SET) < 0)
			return ERR_INVALID_DATA;
    } while (next_off);
	
    return ERR_NONE;
}
static int ReadHeader(rm *p, format_reader* Reader)
{
	int tag = 0;
	int tag_size = 0;
	int flags = 0;
	
	tag = Reader->ReadLE32(Reader);

	if (tag == FOURCC('.', 'r', 'a', 0xfd)) 
	{
		return ERR_NOT_SUPPORTED;
    }
	else if (tag != FOURCC('.', 'R', 'M', 'F')) 
	{
        return ERR_NOT_SUPPORTED;
    }
	Reader->ReadBE32(Reader);   /* header size */
    Reader->ReadBE16(Reader);   
    Reader->ReadBE32(Reader);
    Reader->ReadBE32(Reader);/* number of headers */
	
	for (;;)
	{
		if (Reader->Eof(Reader))
			return ERR_INVALID_DATA;
		
		tag = Reader->ReadLE32(Reader);
		tag_size = Reader->ReadBE32(Reader);
		Reader->ReadBE16(Reader);
		
		if (tag_size < 10 && tag != FOURCC('D', 'A', 'T', 'A'))
            return ERR_INVALID_DATA;
		switch(tag) 
		{
		case FOURCC('P', 'R', 'O', 'P'):
			/* file header */
			Reader->ReadBE32(Reader); /* max bit rate */
			Reader->ReadBE32(Reader); /* avg bit rate */
			Reader->ReadBE32(Reader); /* max packet size */
			Reader->ReadBE32(Reader); /* avg packet size */
			Reader->ReadBE32(Reader); /* nb packets */
			p->Format.Duration = Scale(Reader->ReadBE32(Reader),TICKSPERSEC,1000); /* duration */
			Reader->ReadBE32(Reader); /* preroll */
			p->IndexOffset = Reader->ReadBE32(Reader); /* index offset */
			p->DataOffset = Reader->ReadBE32(Reader); /* data offset */
			Reader->ReadBE16(Reader); /* nb streams */
			flags = Reader->ReadBE16(Reader); /* flags */
			break;
		case FOURCC('C', 'O', 'N', 'T'):
			rm_read_metadata(p,Reader,1);
			break;
		case FOURCC('M', 'D', 'P', 'R'):
			if (rm_read_mdpr_codecdata(p, Reader) < 0)
				return ERR_INVALID_DATA;
			break;
		case FOURCC('D', 'A', 'T', 'A'):
			goto header_end;
		default:
			/* unknown tag: skip it */
			Reader->Skip(Reader,tag_size-10);
			break;
			
		}
	}
header_end:
	Reader->ReadBE32(Reader); //packets
	Reader->ReadBE32(Reader); /* next data header */
	
	if (!p->DataOffset)
	{
		p->DataOffset = Reader->FilePos - 18;
	}
	if(p->IndexOffset)
	{
		Reader->Seek(Reader,p->IndexOffset,SEEK_SET);
		rm_read_index(p,Reader);
		Reader->Seek(Reader,p->DataOffset+18,SEEK_SET);
	}

	return ERR_NONE;
}

static  void rm_ac3_swap_bytes (rm* p, format_reader* Reader, format_packet* Packet)
{
	int j;
	format_ref *Ref = (format_ref*)Packet->Data;
	int32_t endpos	= Ref->Begin+Ref->Length;
	int32_t pos		= Ref->Begin;
	uint8_t *ptr	= (uint8_t*)Ref->Buffer->Block.Ptr+Ref->Begin;
	int size		= Ref->Length;

    for (j=0;j<size;) 
	{
		int tmp= ptr[1];
		ptr[1]= ptr[0];
		ptr[0]= tmp;
        ptr += 2;
		pos += 2;
		j	+= 2;
		if(j >= size)
			break;
		if(pos>=endpos)
		{
			Ref		= Ref->Next;
			endpos	= Ref->Begin+Ref->Length;
			pos		= Ref->Begin;
			ptr		= (uint8_t*)Ref->Buffer->Block.Ptr+Ref->Begin;
		}
    }
}

static void Done(rm* p)
{
	FreeBlock(&p->IndexBuffer);
}

static int Init(rm* p)
{
	format_reader* Reader =	p->Format.Reader;
	
	p->IndexOffset = -1;
	p->IndexNum		= 0;
	p->StreamFlag	= KEY_NULL_STREAM;
	if(ReadHeader(p,Reader))
		return ERR_INVALID_DATA;

	p->Format.HeaderLoaded = 1;
	return ERR_NONE;
}
 /* multiple of 20 bytes for ra144 (ugly) */
#define RAW_PACKET_SIZE 1000

static int sync(rm *p, format_reader* Reader,int64_t *Timestamp, int *flags, int *stream_index, filepos_t *pos)
{
    int len, Id,  i;
    uint32_t state=0xFFFFFFFF;
	format_stream* s;
	
    while(!Reader->Eof(Reader))
	{
        *pos= Reader->FilePos - 3;
       
        state= (state<<8) + Reader->Read8(Reader);
		
        if(state == FOURCC('I', 'N', 'D', 'X'))
		{
            len = Reader->ReadBE16(Reader) - 6;
            if(len<0)
                continue;
            goto skip2;
        }
		
        if(state > (unsigned)0xFFFF || state < 12)
            continue;

        len=state;
        state= 0xFFFFFFFF;
		
        Id = Reader->ReadBE16(Reader);
        *Timestamp = Reader->ReadBE32(Reader);
        Reader->Read8(Reader); /* reserved */
        *flags = Reader->Read8(Reader); /* flags */
		
        len -= 12;
        
		s = NULL;
		for (i=0;i<p->Format.StreamCount;++i)
			if (p->Format.Streams[i]->Id == Id)
			{
				s = p->Format.Streams[i];
				break;
			}
        
        if (Id == p->Format.StreamCount) 
		{
skip2:
			/* skip packet if unknown Stream Id*/
			Reader->Skip(Reader, len);
			
			continue;
        }
        *stream_index= Id;
		
        return len;
    }
    return -1;
}
static int ReadPacket(rm* p, format_reader* Reader, format_packet* Packet)
{
	format_stream* s;
	int i = 0;
	int Id;
	int  len;
    int64_t Timestamp;
    filepos_t pos;
    int flags;
	tick_t RefTime = TIME_UNKNOWN;

	len=sync(p,Reader,&Timestamp,&flags,&Id,&pos);
	
	if(Reader->Eof(Reader)||len == -1||(Reader->FilePos>=p->IndexOffset&&p->IndexNum>0&&p->IndexOffset>=0))
	{
		return ERR_END_OF_FILE;
	}
	
	if (Timestamp>=0)
	{
		RefTime = (tick_t)Scale(Timestamp, TICKSPERSEC, 1000);
		if (RefTime < 0)
			RefTime = 0;
	}
	else
	{
		Packet->RefTime = RefTime;
	}
	
	s = NULL;
	for (i=0;i<p->Format.StreamCount;++i)
	{
		if (p->Format.Streams[i]->Id == Id)
		{
			s = p->Format.Streams[i];
			break;
		}
	}
	
	if(!s)
	{
		Reader->Skip(Reader,len);

		return ERR_NEED_MORE_DATA;
	}

	if(flags&2)
	{
		Packet->Key = 1;
		Packet->RefTime = RefTime;
	}
	else
	{
		Packet->Key = 0;
		if(s->Format.Type == PACKET_AUDIO)
		{
			Packet->RefTime = TIME_UNKNOWN;
		}
		else
			Packet->RefTime = RefTime;
	}

	if(s->Format.Type == PACKET_AUDIO)
	{
		DEBUG_MSG6(-1,T("PACKET_AUDIO len %d,RefTime %d,Timestamp %d,flag %d,Id %d, pos %d =PACKET_AUDIO"),len,Packet->RefTime,(int32_t)Timestamp,flags,Id,pos);	
	}
	else
	{
		DEBUG_MSG6(-1,T("len %d,RefTime %d,Timestamp %d,flag %d,Id %d, pos %d =PACKET_VIDEO"),len,Packet->RefTime,(int32_t)Timestamp,flags,Id,pos);	
	}
	Packet->Stream = s;
	
	if(p->old_format)
	{
		return ERR_NOT_SUPPORTED;	
	}
	else
	{
		if(s->Format.Type == PACKET_VIDEO)
		{		
			Packet->Data = Reader->ReadAsRef(Reader,len);
		}
		else if(s->Format.Type == PACKET_AUDIO)
		{
			
			if ((s->Format.Format.Audio.Format == AUDIOFMT_COOK) ||
				(s->Format.Format.Audio.Format == AUDIOFMT_ATRC)) 
			{
				Packet->Data = Reader->ReadAsRef(Reader,len);
			}
			else if(s->Format.Format.Audio.Format == AUDIOFMT_AAC)
			{
				int x;
				
				int sub_packet_cnt = (Reader->ReadBE16(Reader) & 0xf0) >> 4;

				if (sub_packet_cnt) 
				{
					int nLen = 0;

					for (x = 0; x < sub_packet_cnt; x++)
					{
						nLen+= Reader->ReadBE16(Reader);
					}
					
					Packet->Data = Reader->ReadAsRef(Reader,nLen);
				}
			}
			else
			{
				Packet->Data = Reader->ReadAsRef(Reader,len);
				
				rm_ac3_swap_bytes(p, Reader,Packet);
			}
		}
		else
		{
			return ERR_NOT_SUPPORTED;
		}
	}

	return ERR_NONE;
}
static int Seek(rm* p, tick_t Time, filepos_t FilePos,bool_t PrevKey)
{
	int	i=0xffffffff;
	tick_t preIndexNum = -1;
	if(Time>=0)
	{
		for( i = 0; i < p->IndexNum; i++)
		{			
			tick_t IndexTime = (tick_t)Scale(((rmindex*)IndexBuffer(p,i))->times, TICKSPERSEC, 1000);
			if(Time<=IndexTime)
				break;	
			preIndexNum = i;
		}
	}
	if(PrevKey&&preIndexNum!=-1)
		i = preIndexNum;

	if(p->IndexNum == 0)
	{
		FilePos = p->DataOffset+18;
	}
	else if(i >= 0 && i < p->IndexNum)
	{
		FilePos = ((rmindex*)IndexBuffer(p,i))->pos;
	}
	else
	{
		FilePos = p->DataOffset+18;
	}

	return Format_Seek(&p->Format,FilePos,SEEK_SET);;
}
static int Create(rm* p)
{
	p->Format.Init = (fmtfunc)Init;
	p->Format.Done = (fmtvoid)Done;
	p->Format.Seek = (fmtseek)Seek;
	p->Format.ReadPacket = (fmtreadpacket)ReadPacket;
	//p->Format.MinHeaderLoad = MINBUFFER/2;
	
	return ERR_NONE;
}
static const nodedef RM =
{
	sizeof(rm),
	RM_ID,
	FORMATBASE_CLASS,
	PRI_DEFAULT,		// lower priority so others can override
	(nodecreate)Create,
	(NULL),
};

void RM_Init()
{
	StringAdd(1, RM.Class, NODE_NAME, T("Real Media File (RM,RMVB,RAM,RA)"));
	StringAdd(1, RM.Class, NODE_EXTS, T("rm:V;rmvb:V;ra:A;ram:A"));
	StringAdd(1, RM.Class, NODE_CONTENTTYPE, T("video/rv10,video/rv20,video/rv30,video/rv40"));

	NodeRegisterClass(&RM);
}

void RM_Done()
{
	NodeUnRegisterClass(RM_ID);
}
#else
void RM_Init(){}
void RM_Done(){}
#endif
