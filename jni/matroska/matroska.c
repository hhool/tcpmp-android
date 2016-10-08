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
 * $Id: matroska.c 548 2006-01-08 22:41:57Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common/common.h"
#include "matroska.h"
#include "MatroskaParser/MatroskaParser.h"

typedef struct rmaudio
{	   
    /// Audio descrambling matrix parameters
    int					sub_packet_size, sub_packet_h, coded_framesize; ///< Descrambling parameters from container
    int					audio_framesize; /// Audio frame size from container
    int					sub_packet_lengths[16]; /// Length of each aac subpacket
	int					block_align;

	int					sample_rate;
	int					channels;
} rmaudio;
typedef struct matroska
{
	format_base Format;
	InputStream IO;
	MatroskaFile* File;
	rmaudio		Rmaudio;

} matroska;

#define MK(p) ((matroska*)((char*)(p)-OFS(matroska,IO)))

static void AudioTrackMS(format_stream* s,TrackInfo* Info)
{
	s->Fragmented = 1;
	Format_WaveFormatMem(s,Info->CodecPrivate,Info->CodecPrivateSize);
}

static void AudioTrack(format_stream* s,TrackInfo* Info,int Format,bool_t PacketBased)
{
	PacketFormatClear(&s->Format);
	s->Format.Type = PACKET_AUDIO;
	s->Format.Format.Audio.Format = Format;
	s->Format.Format.Audio.Channels = Info->AV.Audio.Channels;
	s->Format.Format.Audio.SampleRate = mkv_TruncFloat(Info->AV.Audio.SamplingFreq);
	s->Format.Format.Audio.Bits = Info->AV.Audio.BitDepth;
	s->Fragmented = !PacketBased;
	if (PacketBased)
		s->Format.Format.Audio.Flags |= PCM_PACKET_BASED;
	PacketFormatDefault(&s->Format);
}

static void SetPacketRate(format_stream* s,TrackInfo* Info)
{
	if (Info->DefaultDuration)
	{
		longlong v = Info->DefaultDuration;
		s->Format.PacketRate.Num = 1000000000;
		while (v > MAX_INT)
		{
			v >>= 1;
			s->Format.PacketRate.Num >>= 1;
		}
		s->Format.PacketRate.Den = (int)v;
  	}
}

static NOINLINE void VideoAspect(format_stream* s,TrackInfo* Info)
{
	if (Info->AV.Video.PixelWidth>0 && Info->AV.Video.PixelHeight>0 &&
		Info->AV.Video.DisplayWidth>0 && Info->AV.Video.DisplayHeight>0)
		s->Format.Format.Video.Aspect = (int)((ASPECT_ONE*(int64_t)Info->AV.Video.DisplayWidth*(int64_t)Info->AV.Video.PixelHeight)/
			((int64_t)Info->AV.Video.PixelWidth*(int64_t)Info->AV.Video.DisplayHeight));
}

static NOINLINE void VideoTrackMS(format_stream* s,TrackInfo* Info)
{
	Format_BitmapInfoMem(s,Info->CodecPrivate,Info->CodecPrivateSize);
	VideoAspect(s,Info);
	SetPacketRate(s,Info);
}

static NOINLINE void SubtitleTrack(format_stream* s,int FourCC)
{
	PacketFormatClear(&s->Format);
	s->Format.Type = PACKET_SUBTITLE;
	s->Format.Format.Subtitle.FourCC = FourCC;
}

static NOINLINE void VideoTrack(format_stream* s,TrackInfo* Info,int FourCC)
{
	PacketFormatClear(&s->Format);
	s->Format.Type = PACKET_VIDEO;
	s->Format.Format.Video.Pixel.Flags = PF_FOURCC;
	s->Format.Format.Video.Pixel.FourCC = FourCC;
	s->Format.Format.Video.Width = Info->AV.Video.PixelWidth;
	s->Format.Format.Video.Height = Info->AV.Video.PixelHeight;
	s->Format.Format.Video.Aspect = ASPECT_ONE;
	VideoAspect(s,Info);
	SetPacketRate(s,Info);
}

static NOINLINE void AddChapter(pin* Comment,ulonglong Start,struct ChapterDisplay* p,int No)
{
	tchar_t s[256];

	stprintf_s(s,TSIZEOF(s),T("CHAPTER%02dNAME="),No);
	UTF8ToTcs(s+tcslen(s),TSIZEOF(s)-tcslen(s),p->String);
	Comment->Node->Set(Comment->Node,Comment->No,s,sizeof(s));

	BuildChapter(s,TSIZEOF(s),No,Start,1000000);
	Comment->Node->Set(Comment->Node,Comment->No,s,sizeof(s));
}

static NOINLINE void AddAttachment(pin* Comment,Attachment *At) \
{
	tchar_t MimeType[32];
	StrToTcs(MimeType,TSIZEOF(MimeType),At->MimeType);

	if (tcsnicmp(MimeType,T("image"),5)==0)
	{
		tchar_t s[256];
		stprintf_s(s,TSIZEOF(s),T("%s=:%d:%d:%s"),PlayerComment(COMMENT_COVER),(int)At->Position,(int)At->Length,MimeType);
		Comment->Node->Set(Comment->Node,Comment->No,s,sizeof(s));
	}
}

static int AddChapters(pin* Comment,Chapter *Ch,int No) 
{
	uint32_t i,j;

	if (Ch)
		for (i=0;i<Ch->nChildren;++i) 
		{
			for (j=0;j<Ch->Children[i].nDisplay;++i)
				if (Ch->Children[i].Display[j].String)
				{
					AddChapter(Comment,Ch->Children[i].Start,
						&Ch->Children[i].Display[j],No++);
					break;
				}

			No = AddChapters(Comment,&Ch->Children[i],No);
		}

	return No;
}

static void AddComment(pin* Comment,tchar_t* Name,char* Value) 
{
	if (Comment->Node && Value)
	{
		tchar_t s[256];
		tcscpy_s(s,TSIZEOF(s),Name);
		tcscat_s(s,TSIZEOF(s),T("="));
		UTF8ToTcs(s+tcslen(s),TSIZEOF(s)-tcslen(s),Value);
		Comment->Node->Set(Comment->Node,Comment->No,s,sizeof(s));
	}
}

static void BuildConfig(format_stream* s,const char* CodecID,int OutputFreq)
{
	static const int Rates[12] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000};
	int Rate;
	int Profile;
	uint8_t* Config;
	int Len = 2;
	bool_t SBR = 0;

	if (OutputFreq > s->Format.Format.Audio.SampleRate)
	{
		if (OutputFreq > 24000)
			OutputFreq >>= 1;
		s->Format.Format.Audio.SampleRate = OutputFreq;
		SBR = 1;
	}

	for (Rate=0;Rate<11;++Rate)
		if (s->Format.Format.Audio.SampleRate >= Rates[Rate])
			break;

	if (strcmp(CodecID+12, "MAIN")==0)
		Profile = 1;
	else if (strcmp(CodecID+12, "SSR")==0)
		Profile = 3;
	else if (strcmp(CodecID+12, "LTP")==0)
		Profile = 4;
	else if (strcmp(CodecID+12, "SBR")==0)
		Profile = 5;
	else if (strcmp(CodecID+12, "LC/SBR")==0 || SBR)
	{
		Profile = 2;
		Len = 5;
	}
	else
		Profile = 2; //assuming LC

	if (PacketFormatExtra(&s->Format,Len))
	{
		Config = (uint8_t*)s->Format.Extra;
	    Config[0] = (uint8_t)(Profile << 3);
	    Config[0] |= (uint8_t)(Rate >> 1);
	    Config[1] = (uint8_t)(Rate << 7);
	    Config[1] |= (uint8_t)(s->Format.Format.Audio.Channels << 3);

		if (Len>2)
		{
			if (s->Format.Format.Audio.SampleRate <= 24000)
				s->Format.Format.Audio.SampleRate <<= 1;

			for (Rate=0;Rate<11;++Rate)
				if (s->Format.Format.Audio.SampleRate >= Rates[Rate])
					break;

			Config[2] = 0x56;
			Config[3] = 0xE5;
			Config[4] = 0x80;
			Config[4] |= (uint8_t)(Rate << 3);
		}
	}
}

static int Stream_Read8(stream* s)
{
	uint8_t v;
	s->Read(s,&v,1);
	return v;
}

static int Stream_ReadBE16(stream* s)
{
	int v = Stream_Read8(s) << 8;
	v |= Stream_Read8(s);
	return v;
}


static int Stream_ReadBE32(stream* s)
{
	int v = Stream_Read8(s) << 24;
	v |= Stream_Read8(s) << 16;
	v |= Stream_Read8(s) << 8;
	v |= Stream_Read8(s);
	return v;
}

static inline void get_strl(matroska *p, stream* s, char *buf, int buf_size, int len)
{
    int i;
    char *q, r;

    q = buf;
    for(i=0;i<len;i++)
	{
        r = Stream_Read8(s);
        if (i < buf_size - 1)
            *q++ = r;
    }
    if (buf_size > 0) *q = '\0';
}

static void get_str8(matroska *p, stream* s,char *buf, int buf_size)
{
	int byte = Stream_Read8(s);
    
	get_strl(p, s,buf, buf_size, byte);
}
static int Init(matroska* p)
{
	unsigned n,Count;
	SegmentInfo* Info;

	p->File = mkv_Open(&p->IO,NULL,0);
	if (!p->File)
		return ERR_INVALID_DATA;

	Info = mkv_GetFileInfo(p->File);
	if (Info)
		p->Format.Duration = (tick_t)((Info->Duration * TICKSPERSEC) / 1000000000);

	Count = mkv_GetNumTracks(p->File);
	for (n=0;n<Count;++n)
	{
		format_stream* s = Format_AddStream(&p->Format,sizeof(format_stream));
		if (s)
		{
			TrackInfo *Info = mkv_GetTrackInfo(p->File,n);
			if (Info)
			{
				int CodecPrivateSize = Info->CodecPrivateSize;

				if (strcmp(Info->CodecID, "V_VP8")==0)
				{
					VideoTrack(s,Info,FOURCC('F','V','P','8'));
				}
				else if (strcmp(Info->CodecID, "V_ON2VP8")==0)
				{
					VideoTrack(s,Info,FOURCC('F','O','P','8'));
				}
				else if (strcmp(Info->CodecID, "V_MS/VFW/FOURCC")==0)
				{
					VideoTrackMS(s,Info);
					CodecPrivateSize = 0;
				}
				else if (strcmp(Info->CodecID, "V_THEORA")==0)
				{
					VideoTrack(s,Info,FOURCC('F','T','H','E'));
				}
				else if (strncmp(Info->CodecID, "V_REAL/RV", 9)==0)
					VideoTrack(s,Info,LOAD32LE(Info->CodecID+7));
				else if (strncmp(Info->CodecID, "V_MPEG4/ISO/AVC", 14)==0)
				{
					VideoTrack(s,Info,FOURCC('A','V','C','1'));
					s->Format.Format.Video.Pixel.Flags |= PF_PTS;
				}
				else if (strncmp(Info->CodecID, "V_MPEG4/ISO", 11)==0)
					VideoTrack(s,Info,FOURCC('M','P','4','V'));
				else if (strcmp(Info->CodecID, "V_MPEG4/MS/V3")==0)
					VideoTrack(s,Info,FOURCC('M','P','4','3'));
				else if (strcmp(Info->CodecID, "V_MPEG1")==0 || strcmp(Info->CodecID, "V_MPEG2")==0)
					VideoTrack(s,Info,FOURCC('M','P','E','G'));
				else if (strcmp(Info->CodecID, "V_MJPEG")==0)
					VideoTrack(s,Info,FOURCC('M','J','P','G'));
				else if (strcmp(Info->CodecID, "A_REAL/COOK")==0)
				{
					uint32_t tag;
					uint32_t version;
					char	 buf[256];
					stream*  node;

					AudioTrack(s,Info,AUDIOFMT_COOK,0);

					node=(stream*)NodeCreate(MEMSTREAM_ID);
					node->Set(node,MEMSTREAM_DATA,Info->CodecPrivate,CodecPrivateSize);
					tag = Stream_ReadBE32(node);
					if (tag == FOURCC(0xfd, 'a', 'r', '.'))
					{
						/* ra type header */
						version = Stream_ReadBE16(node);
						
						if (version == 3) 
						{
							return ERR_NOT_SUPPORTED;//fixme
						}
						else
						{
							int flavor;
							int codecdata_length;
							uint32_t codec_tag;
							int32_t RealExtralLen;
							/* old version (4) */
							node->Seek(node,2,SEEK_CUR);//Reader->Skip(Reader,2);//url_fskip(pb, 2); /* unused */
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb); /* .ra4 */
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb); /* data size */
							Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb); /* version2 */
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb); /* header size */
							flavor= Stream_ReadBE16(node);//get_be16(pb); /* add codec info / flavor */
							p->Rmaudio.coded_framesize = Stream_ReadBE32(node);//get_be32(pb); /* coded frame size */
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb); /* ??? */
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb); /* ??? */
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb); /* ??? */
							p->Rmaudio.sub_packet_h = Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb); /* 1 */
							p->Rmaudio.block_align= Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb); /* frame size */
							p->Rmaudio.sub_packet_size = Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb); /* sub packet size */
							Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb); /* ??? */

							if (version == 5)
							{
								Stream_ReadBE16(node);//
								Stream_ReadBE16(node);//
								Stream_ReadBE16(node);//
							}
							p->Rmaudio.sample_rate = Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb);
							Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb);
							p->Rmaudio.channels = Stream_ReadBE16(node);//Reader->ReadBE16(Reader);//get_be16(pb);

							if (version == 5)
							{
								Stream_ReadBE32(node);//Reader->ReadBE32(Reader);//get_be32(pb);
								node->Read(node,buf,4);
							} 
							else 
							{
								get_str8(p, node,buf, sizeof(buf)); /* desc */
								get_str8(p, node,buf, sizeof(buf)); /* desc */
							}

							//codec_tag  = buf[0]<<0|buf[1]<<8|buf[2]<<16|buf[3]<<24;
							 Stream_ReadBE16(node);Stream_Read8(node);//Reader->ReadBE16(Reader); Reader->Read8(Reader);
							if (version == 5)
								Stream_Read8(node);//Reader->Read8(Reader);

							codecdata_length = Stream_ReadBE32(node);//Reader->ReadBE32(Reader);

							if(codecdata_length + 8 <= (unsigned)codecdata_length)
							{
								return ERR_INVALID_DATA;
							}

							p->Rmaudio.audio_framesize = p->Rmaudio.block_align;

							if(p->Rmaudio.sub_packet_size <= 0)
							{
								return ERR_INVALID_DATA;
							}

							p->Rmaudio.block_align = p->Rmaudio.sub_packet_size;
	
							if(p->Rmaudio.audio_framesize >= 0xffffffff / p->Rmaudio.sub_packet_h)
							{
								return ERR_INVALID_DATA;
							}
							RealExtralLen = codecdata_length;
							codecdata_length+=16;
							
							if (codecdata_length >= 1<<24)
								return ERR_INVALID_DATA;
							
							if (PacketFormatExtra(&s->Format,codecdata_length))
							{
								uint32_t* pExtra = (uint32_t*)s->Format.Extra;
								pExtra[0] = p->Rmaudio.sub_packet_size;
								pExtra[1] = p->Rmaudio.coded_framesize;
								pExtra[2] = p->Rmaudio.sub_packet_h;
								pExtra[3] = p->Rmaudio.audio_framesize;
								pExtra+=4;

								node->Read(node,pExtra,RealExtralLen);

								CodecPrivateSize = 0;
							}
							s->Format.Format.Audio.BlockAlign = p->Rmaudio.block_align;
							s->Format.Format.Audio.Channels = p->Rmaudio.channels;
							s->Format.Format.Audio.SampleRate = p->Rmaudio.sample_rate;
						}
					}
					NodeDelete(node);

				}
				else if (strcmp(Info->CodecID, "A_MPEG/L3")==0)
					AudioTrack(s,Info,AUDIOFMT_MP3,0);
				else if (strcmp(Info->CodecID, "A_MPEG/L2")==0 || strcmp(Info->CodecID, "A_MPEG/L1")==0)
					AudioTrack(s,Info,AUDIOFMT_MP2,0);
				else if (strcmp(Info->CodecID, "A_PCM/INT/LIT")==0)
					AudioTrack(s,Info,AUDIOFMT_PCM,0);
				else if (strstr(Info->CodecID, "A_AC3")!=NULL || strcmp(Info->CodecID, "A_DTS")==0)
					AudioTrack(s,Info,AUDIOFMT_A52,0);
				else if (strstr(Info->CodecID, "A_AAC")!=NULL)
				{
					AudioTrack(s,Info,AUDIOFMT_AAC,1);
					if (!CodecPrivateSize)
						BuildConfig(s,Info->CodecID,mkv_TruncFloat(Info->AV.Audio.OutputSamplingFreq));
				}
				else if (strcmp(Info->CodecID, "A_VORBIS")==0)
					AudioTrack(s,Info,AUDIOFMT_VORBIS_PACKET,1);
				else if (strcmp(Info->CodecID, "A_QUICKTIME/QDM2")==0)
					AudioTrack(s,Info,AUDIOFMT_QDESIGN2,1);
				else if (strcmp(Info->CodecID, "A_TTA1")==0)
					AudioTrack(s,Info,AUDIOFMT_TTA,1);
				else if (strcmp(Info->CodecID, "A_MS/ACM")==0)
				{
					AudioTrackMS(s,Info);
					CodecPrivateSize = 0;
				}
				else if (strcmp(Info->CodecID, "S_TEXT/UTF8")==0)
					SubtitleTrack(s,SUBTITLE_UTF8);
				else if (strcmp(Info->CodecID, "S_TEXT/SSA")==0)
					SubtitleTrack(s,SUBTITLE_SSA);
				else if (strcmp(Info->CodecID, "S_TEXT/ASS")==0)
					SubtitleTrack(s,SUBTITLE_ASS);
				else if (strcmp(Info->CodecID, "S_TEXT/USF")==0)
					SubtitleTrack(s,SUBTITLE_USF);

				if (CodecPrivateSize && PacketFormatExtra(&s->Format,CodecPrivateSize))
					memcpy(s->Format.Extra,Info->CodecPrivate,s->Format.ExtraLength);
			}
			Format_PrepairStream(&p->Format,s);

			AddComment(&s->Comment,T("TITLE"),Info->Name);
			AddComment(&s->Comment,T("LANGUAGE"),Info->Language);
		}
	}

	if (p->Format.StreamCount)
	{
		pin* Comment = &p->Format.Streams[0]->Comment;
		Tag* Tags;
		Chapter* Ch;
		Attachment* At;

		mkv_GetTags(p->File,&Tags,&Count);

		if (Tags && Comment->Node)
			for (n=0;n<Count;++n,++Tags)
			{
				uint32_t i;
				for (i=0;i<Tags->nSimpleTags;++i)
				{
					tchar_t	s[256];
					UTF8ToTcs(s,TSIZEOF(s),Tags->SimpleTags[i].Name);
					tcscat_s(s,TSIZEOF(s),T("="));
					UTF8ToTcs(s+tcslen(s),TSIZEOF(s)-tcslen(s),Tags->SimpleTags[i].Value);
					Comment->Node->Set(Comment->Node,Comment->No,s,sizeof(s));
				}
			}

		mkv_GetAttachments(p->File,&At,&Count);
		if (Count>0)
			for (n=0;n<Count;++n)
				AddAttachment(Comment,At+n);

		mkv_GetChapters(p->File,&Ch,&Count);
		if (Count>0 && Info)
			AddChapters(Comment,Ch,1);
	}

	mkv_SetTrackMask(p->File,0);
	
	p->Format.HeaderLoaded = 1;
	return ERR_NONE;
}

static void Done(matroska* p)
{
	if (p->File)
	{
		mkv_Close(p->File);
		p->File = NULL;
	}
}

static int ReadPacket(matroska* p, format_reader* Reader, format_packet* Packet)
{
	ulonglong Start,End;
	void* Ref;
	ulonglong Pos;
	unsigned int Flags,Track,Size;
	int Result = mkv_ReadFrame(p->File,0,&Track,&Start,&End,&Pos,&Size,&Ref,&Flags);

	if (Result == EOF)
		return ERR_END_OF_FILE;

	if (Result != 0)
		return ERR_INVALID_DATA;

	Packet->Data = (format_ref*)Ref;
	Packet->Key = (Flags & FRAME_KF) != 0;
	Packet->Stream = p->Format.Streams[Track];

	if (!(Flags & FRAME_UNKNOWN_START))
	{
		Packet->RefTime = (tick_t)((Start * TICKSPERSEC) / 1000000000);
		if (Packet->Stream->Format.Type == PACKET_SUBTITLE && !(Flags & FRAME_UNKNOWN_END))
			Packet->EndRefTime = (tick_t)((End * TICKSPERSEC) / 1000000000);
	}

	return ERR_NONE;
}

static int Seek(matroska* p, tick_t Time, filepos_t FilePos,bool_t PrevKey)
{
	if (Time < 0 && FilePos >= 0 && p->Format.Duration > 0 && p->Format.FileSize > 0)
		Time = Scale(FilePos, p->Format.Duration, p->Format.FileSize);

	if (Time >= 0)
	{
		p->Format.SyncRead = 0; // don't use partial buffers
		mkv_Seek(p->File,((ulonglong)Time * 1000000000 + (TICKSPERSEC/2))/TICKSPERSEC,PrevKey?MKVF_SEEK_TO_PREV_KEYFRAME:0);

		Format_AfterSeek(&p->Format);
		return ERR_NONE;
	}
	return ERR_NOT_SUPPORTED;
}

static int Read(struct InputStream *cc,ulonglong pos,void *buffer,int count)
{
	format_reader* Reader = &MK(cc)->Format.Reader[0];

	if (Reader->FilePos != pos && Reader->Seek(Reader,(filepos_t)pos,SEEK_SET) != ERR_NONE)
		return -1;

	return Reader->Read(Reader,buffer,count);
}

static longlong Scan(struct InputStream *cc,ulonglong start,unsigned signature)
{
	format_reader* Reader = &MK(cc)->Format.Reader[0];
	unsigned v = ~signature;
	int ch;

	if (Reader->FilePos != start && Reader->Seek(Reader,(filepos_t)start,SEEK_SET) != ERR_NONE)
		return -1;

	while ((ch = Reader->Read8(Reader)) >= 0)
	{
		v = ((v << 8) | ch) & 0xFFFFFFFF;
		if (v == signature)
			return Reader->FilePos - 4;
	}

	return -1;
}

static unsigned GetSize(struct InputStream *cc)
{
	return MK(cc)->Format.Reader->BufferAvailable;
}

static const char* GetError(struct InputStream *cc)
{
	return "";
}

static void* MemAlloc(struct InputStream *cc,size_t size)
{
	size = ((size+SAFETAIL-1)/SAFETAIL)*SAFETAIL;
	return malloc(size);
}

static void* MemReAlloc(struct InputStream *cc,void *mem,size_t newsize)
{
	newsize = ((newsize+SAFETAIL-1)/SAFETAIL)*SAFETAIL;
	return realloc(mem,newsize);
}

static void	MemFree(struct InputStream *cc,void *mem)
{
	free(mem);
}

static int Progress(struct InputStream *cc,ulonglong cur,ulonglong max)
{
	return 1;
}

static int IOReadCh(struct InputStream *IO)
{
	return MK(IO)->Format.Reader->Read8(MK(IO)->Format.Reader);
}

static int IORead(struct InputStream *IO,void *buffer,int count)
{
	return MK(IO)->Format.Reader->Read(MK(IO)->Format.Reader,buffer,count);
}

static void IOSeek(struct InputStream *IO,longlong where,int how)
{
    MK(IO)->Format.Reader->Seek(MK(IO)->Format.Reader,(filepos_t)where,how);
}

static longlong IOTell(struct InputStream *IO)
{
	return MK(IO)->Format.Reader->FilePos;
}

static void* IOMakeRef(struct InputStream *IO,int count)
{
	return MK(IO)->Format.Reader->ReadAsRef(MK(IO)->Format.Reader,count);
}

static void	IOReleaseRef(struct InputStream *IO,void* ref)
{
	Format_ReleaseRef(&MK(IO)->Format,(format_ref*)ref);
}

static int Create(matroska* p)
{
	p->Format.Init = (fmtfunc)Init;
	p->Format.Done = (fmtvoid)Done;
	p->Format.Seek = (fmtseek)Seek;
	p->Format.ReadPacket = (fmtreadpacket) ReadPacket;
  	p->IO.read = Read;
	p->IO.scan = Scan;
	p->IO.getsize = GetSize;
	p->IO.geterror = GetError;
	p->IO.memalloc = MemAlloc;
	p->IO.memrealloc = MemReAlloc;
	p->IO.memfree = MemFree;
	p->IO.progress = Progress;
	p->IO.ioread = IORead;
	p->IO.ioreadch = IOReadCh;
	p->IO.ioseek = IOSeek;
	p->IO.iotell = IOTell;
	p->IO.makeref = IOMakeRef;
	p->IO.releaseref = IOReleaseRef;
	return ERR_NONE;
}

static const nodedef Matroska = 
{
	sizeof(matroska),
	MATROSKA_ID,
	FORMATBASE_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
};

void Matroska_Init()
{
	NodeRegisterClass(&Matroska);
}

void Matroska_Done()
{
	NodeUnRegisterClass(MATROSKA_ID);
}


static const nodedef WEBM = 
{
	sizeof(matroska),
	WEBM_ID,
	FORMATBASE_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
};

void WEBM_Init()
{
	NodeRegisterClass(&WEBM);
}
void WEBM_Done()
{
	NodeUnRegisterClass(WEBM_ID);
}