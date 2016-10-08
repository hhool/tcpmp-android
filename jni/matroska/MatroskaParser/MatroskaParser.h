/*
* Copyright (c) 2004-2005 Mike Matsnev.  All Rights Reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice immediately at the beginning of the file, without modification,
*    this list of conditions, and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Absolutely no warranty of function or purpose is made by the author
*    Mike Matsnev.
*
* THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
* THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* $Id: MatroskaParser.h,v 1.15 2005/06/07 17:11:31 mike Exp $
* 
*/

#ifndef MATROSKA_PARSER_H
#define	MATROSKA_PARSER_H

/* Random notes:
*
* The parser does not process frame data in any way and does not read it into
* the queue. The app should read it via mkv_ReadData if it is interested.
*
* The code here is 64-bit clean and was tested on FreeBSD/sparc 64-bit big endian
* system
*/
#define SAFETAIL 256

#ifdef MPDLLBUILD
#define	X __declspec(dllexport)
#else
#ifdef MPDLL
#define X __declspec(dllimport)
#pragma comment(lib,"MatroskaParser")
#else
#define X
#endif
#endif

#define MATROSKA_COMPRESSION_SUPPORT
#define	MATROSKA_INTEGER_ONLY

#ifdef __cplusplus
extern "C" {
#endif

	/* 64-bit integers */
#ifdef _MSC_VER //Picard
	typedef signed __int64	    longlong;
	typedef unsigned __int64    ulonglong;
#else
	typedef signed long long    longlong;
	typedef unsigned long long    ulonglong;
#endif

	/* MKFLOATing point */
#ifdef MATROSKA_INTEGER_ONLY
	typedef struct 
	{
		longlong	v;
	} MKFLOAT;
#else
	typedef	double	MKFLOAT;
#endif

	/* generic I/O */
	struct InputStream 
	{
		/* read bytes from stream */
		int	      (*read)(struct InputStream *cc,ulonglong pos,void *buffer,int count);
		/* scan for a four byte signature, bytes must be nonzero */
		longlong    (*scan)(struct InputStream *cc,ulonglong start,unsigned signature);
		/* get cache size, this is used to cap readahead */
		unsigned    (*getsize)(struct InputStream *cc);
		/* fetch last error message */
		const char *(*geterror)(struct InputStream *cc);
		/* memory allocation */
		void	      *(*memalloc)(struct InputStream *cc,size_t size);
		void	      *(*memrealloc)(struct InputStream *cc,void *mem,size_t newsize);
		void	      (*memfree)(struct InputStream *cc,void *mem);
		// zero return causes parser to abort open
		int	      (*progress)(struct InputStream *cc,ulonglong cur,ulonglong max);

		//Picard
		int	      (*ioreadch)(struct InputStream *inf);
		int	      (*ioread)(struct InputStream *inf,void *buffer,int count);
		void	      (*ioseek)(struct InputStream *inf,longlong where,int how);
		longlong    (*iotell)(struct InputStream *inf);
		void*	      (*makeref)(struct InputStream *inf,int count);
		void	      (*releaseref)(struct InputStream *inf,void* ref);
	};

	typedef struct InputStream InputStream;

	/* matroska file */
	struct MatroskaFile; /* opaque */

	typedef struct MatroskaFile MatroskaFile;

#define	COMP_ZLIB   0
#define	COMP_BZIP   1
#define	COMP_LZO1X  2
#define	COMP_PREPEND 3

#define	TT_VIDEO    1
#define	TT_AUDIO    2
#define	TT_SUB	    17

	struct TrackInfo 
	{
		unsigned char	  Number;
		unsigned char	  Type;
		unsigned char	  TrackOverlay;
		ulonglong	  UID;
		ulonglong	  MinCache;
		ulonglong	  MaxCache;
		ulonglong	  DefaultDuration;
		MKFLOAT	  TimecodeScale;
		void		  *CodecPrivate;
		unsigned	  CodecPrivateSize;
		unsigned	  CompMethod;
		void		  *CompMethodPrivate;
		unsigned	  CompMethodPrivateSize;
		unsigned	  MaxBlockAdditionID;

		unsigned int  Enabled:1;
		unsigned int  Default:1;
		unsigned int  Lacing:1;
		unsigned int  DecodeAll:1;
		unsigned int  CompEnabled:1;

		union 
		{
			struct 
			{
				unsigned char   StereoMode;
				unsigned char   DisplayUnit;
				unsigned char   AspectRatioType;
				unsigned int    PixelWidth;
				unsigned int    PixelHeight;
				unsigned int    DisplayWidth;
				unsigned int    DisplayHeight;
				unsigned int    CropL, CropT, CropR, CropB;
				unsigned int    ColourSpace;
				MKFLOAT	      GammaValue;

				unsigned int  Interlaced:1;
			} Video;
			struct 
			{
				MKFLOAT	      SamplingFreq;
				MKFLOAT	      OutputSamplingFreq;
				unsigned char   Channels;
				unsigned char   BitDepth;
			} Audio;
		} AV;

		/* various strings */
		char			*Name;
		char			Language[4];
		char			*CodecID;
	};

	typedef struct TrackInfo  TrackInfo;

	struct SegmentInfo 
	{
		char			UID[16];
		char			PrevUID[16];
		char			NextUID[16];
		char			*Filename;
		char			*PrevFilename;
		char			*NextFilename;
		char			*Title;
		char			*MuxingApp;
		char			*WritingApp;
		ulonglong		TimecodeScale;
		ulonglong		Duration;
		longlong		DateUTC;
		char			DateUTCValid;
	};

	typedef struct SegmentInfo SegmentInfo;

	struct Attachment 
	{
		ulonglong		Position;
		ulonglong		Length;
		ulonglong		UID;
		char			*Name;
		char			*Description;
		char			*MimeType;
	};

	typedef struct Attachment Attachment;

	struct ChapterDisplay 
	{
		char			*String;
		char			Language[4];
		char			Country[4];
	};

	struct ChapterCommand 
	{
		unsigned		Time;
		unsigned		CommandLength;
		void			*Command;
	};

	struct ChapterProcess 
	{
		unsigned		CodecID;
		unsigned		CodecPrivateLength;
		void			*CodecPrivate;
		unsigned		nCommands,nCommandsSize;
		struct ChapterCommand	*Commands;
	};

	struct Chapter 
	{
		ulonglong		UID;
		ulonglong		Start;
		ulonglong		End;

		unsigned		nTracks,nTracksSize;
		ulonglong		*Tracks;
		unsigned		nDisplay,nDisplaySize;
		struct ChapterDisplay	*Display;
		unsigned		nChildren,nChildrenSize;
		struct Chapter	*Children;
		unsigned		nProcess,nProcessSize;
		struct ChapterProcess	*Process;

		char			SegmentUID[16];


		unsigned int	Hidden:1;
		unsigned int	Enabled:1;

		// Editions
		unsigned int	Default:1;
		unsigned int	Ordered:1;

	};

	typedef struct Chapter	Chapter;

#define	TARGET_TRACK	  0
#define	TARGET_CHAPTER	  1
#define	TARGET_ATTACHMENT 2
#define	TARGET_EDITION	  3
	struct Target 
	{
		ulonglong	    UID;
		unsigned	    Type;
	};

	struct SimpleTag 
	{
		char		    *Name;
		char		    *Value;
		char		    Language[4];
		unsigned	    Default:1;
	};

	struct Tag 
	{
		unsigned	    nTargets,nTargetsSize;
		struct Target	    *Targets;

		unsigned	    nSimpleTags,nSimpleTagsSize;
		struct SimpleTag  *SimpleTags;
	};

	typedef struct Tag  Tag;

	//Picard
#if !defined(__palmos__) || !defined(ARM) 
#include <setjmp.h>
#else
#ifndef EOF
#define EOF -1
#endif
	typedef int jmp_buf[16];
	int setjmp(jmp_buf jp);
	int longjmp(jmp_buf jp, int ret);
#define alloca(size) __builtin_alloca(size)
#endif

#if defined(_WIN32)&&!defined(__SYMBIAN32__)
	// MS names some functions differently
#define	alloca	  _alloca
#define	inline	  __inline

#include <tchar.h>
#endif

#if defined(__SYMBIAN32__)
#if defined(__GNUC__)
#define alloca(size) __builtin_alloca(size)
#elif defined(__ARMCC__)
#include <stdlib.h>
#else
	//#include <malloc.h>
#endif
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef EVCBUG
#define	EVCBUG
#endif

#define	EBML_VERSION	      1
#define	EBML_MAX_ID_LENGTH    4
#define	EBML_MAX_SIZE_LENGTH  8
#define	MATROSKA_VERSION      2
#define	MATROSKA_DOCTYPE      "matroska"
#define	WEBM_DOCTYPE		  "webm"

#define	MAX_STRING_LEN	      1023
#define	QSEGSIZE	      512
#define	MAX_TRACKS	      32
#define	MAX_READAHEAD	      (512*1024)

#define	MAXCLUSTER	      (64*1048576)
#define	MAXFRAME	      (4*1048576)

#if defined(WIN32) && !defined(__SYMBIAN32__)
#define	LL(x)	x##i64
#define	ULL(x)	x##ui64
#else
#define	LL(x)	x##ll
#define	ULL(x)	x##ull
#endif

#define	MAXU64		      ULL(0xffffffffffffffff)
#define	ONE		      ULL(1)

	struct Cue 
	{
		ulonglong	Time;
		ulonglong	Position;
		ulonglong	Block;
		unsigned char	Track;
	};

	struct QueueEntry 
	{
		struct QueueEntry *next;
		unsigned int	    Length;

		ulonglong	    Start;
		ulonglong	    End;
		ulonglong	    Position;
		void			*Ref; //Picard

		unsigned int	    flags;
	};

	struct Queue 
	{
		struct QueueEntry *head;
		struct QueueEntry *tail;
	};

#define	MPF_ERROR 0x10000
#define	IBSZ	  1024

#define	RBRESYNC  1

	struct MatroskaFile 
	{
		// parser config
		unsigned    flags;

		// input
		InputStream *cache;

		// internal buffering
		char	      inbuf[IBSZ];
		ulonglong   bufbase; // file offset of the first byte in buffer
		int	      bufpos; // current read position in buffer
		int	      buflen; // valid bytes in buffer

		// error reporting
		char	      errmsg[128];
		jmp_buf     jb;

		// pointers to key elements
		ulonglong   pSegment;
		ulonglong   pSeekHead;
		ulonglong   pSegmentInfo;
		ulonglong   pCluster;
		ulonglong   pTracks;
		ulonglong   pCues;
		ulonglong   pAttachments;
		ulonglong   pChapters;
		ulonglong   pTags;

		// flags for key elements
		struct 
		{
			unsigned int  SegmentInfo:1;
			unsigned int  Cluster:1;
			unsigned int  Tracks:1;
			unsigned int  Cues:1;
			unsigned int  Attachments:1;
			unsigned int  Chapters:1;
			unsigned int  Tags:1;
		} seen;

		// file info
		ulonglong   firstTimecode;

		// SegmentInfo
		struct SegmentInfo  Seg;

		// Tracks
		unsigned int	    nTracks,nTracksSize;
		struct TrackInfo  **Tracks;

		// Queues
		struct QueueEntry *QFreeList;
		unsigned int	    nQBlocks,nQBlocksSize;
		struct QueueEntry **QBlocks;
		struct Queue	    *Queues;
		ulonglong	    readPosition;
		unsigned int	    trackMask;
		ulonglong	    pSegmentTop;  // offset of next byte after the segment
		ulonglong	    tcCluster;    // current cluster timecode

		// Cues
		unsigned int	    nCues,nCuesSize;
		struct Cue	    *Cues;

		// Attachments
		unsigned int	    nAttachments,nAttachmentsSize;
		struct Attachment *Attachments;

		// Chapters
		unsigned int	    nChapters,nChaptersSize;
		struct Chapter    *Chapters;

		// Tags
		unsigned int	    nTags,nTagsSize;
		struct Tag	    *Tags;
	};
	/* Open a matroska file
	* io pointer is recorded inside MatroskaFile
	*/
	X MatroskaFile  *mkv_Open(/* in */ InputStream *io,
		/* out */ char *err_msg,
		/* in */  unsigned msgsize);

#define	MKVF_AVOID_SEEKS    1 /* use sequential reading only */

	X MatroskaFile  *mkv_OpenEx(/* in */  InputStream *io,
		/* in */  ulonglong base,
		/* in */  unsigned flags,
		/* out */ char *err_msg,
		/* in */  unsigned msgsize);

	/* Close and deallocate mf
	* NULL pointer is ok and is simply ignored
	*/
	X void		mkv_Close(/* in */ MatroskaFile *mf);

	/* Fetch the error message of the last failed operation */
	X const char    *mkv_GetLastError(/* in */ MatroskaFile *mf);

	/* Get file information */
	X SegmentInfo   *mkv_GetFileInfo(/* in */ MatroskaFile *mf);

	/* Get track information */
	X unsigned int  mkv_GetNumTracks(/* in */ MatroskaFile *mf);
	X TrackInfo     *mkv_GetTrackInfo(/* in */ MatroskaFile *mf,/* in */ unsigned track);

	/* chapters, tags and attachments */
	X void	      mkv_GetAttachments(/* in */   MatroskaFile *mf,
		/* out */  Attachment **at,
		/* out */  unsigned *count);
	X void	      mkv_GetChapters(/* in */	MatroskaFile *mf,
		/* out */	Chapter **ch,
		/* out */ unsigned *count);
	X void	      mkv_GetTags(/* in */  MatroskaFile *mf,
		/* out */ Tag **tag,
		/* out */ unsigned *count);

	X ulonglong   mkv_GetSegmentTop(MatroskaFile *mf);

	/* Seek to specified timecode,
	* if timecode is past end of file,
	* all tracks are set to return EOF
	* on next read
	*/
#define	MKVF_SEEK_TO_PREV_KEYFRAME  1
#define	MKVF_SEEK_TO_PREV_KEYFRAME_STRICT   2

	X void	      mkv_Seek(/* in */ MatroskaFile *mf,
		/* in */	ulonglong timecode /* in ns */,
		/* in */ unsigned flags);

	X void	      mkv_SkipToKeyframe(MatroskaFile *mf);

	X ulonglong   mkv_GetLowestQTimecode(MatroskaFile *mf);

	X int	      mkv_TruncFloat(MKFLOAT f);

	/*************************************************************************
	* reading data, pull model
	*/

	/* frame flags */
#define	FRAME_UNKNOWN_START   0x00000001
#define	FRAME_UNKNOWN_END     0x00000002
#define	FRAME_KF	      0x00000004
#define	FRAME_GAP	      0x00800000
#define	FRAME_STREAM_MASK     0xff000000
#define	FRAME_STREAM_SHIFT    24

	/* This sets the masking flags for the parser,
	*  masked tracks [with 1s in their bit positions]
	*  will be ignored when reading file data.
	* This call discards all parsed and queued frames
	*/
	X void	      mkv_SetTrackMask(/* in */ MatroskaFile *mf,/* in */ unsigned int mask);

	/* Read one frame from the queue.
	* mask specifies what tracks to ignore.
	* Returns -1 if there are no more frames in the specified
	* set of tracks, 0 on success
	*/
	X int	      mkv_ReadFrame(/* in */  MatroskaFile *mf,
		/* in */  unsigned int mask,
		/* out */ unsigned int *track,
		/* out */ ulonglong *StartTime /* in ns */,
		/* out */ ulonglong *EndTime /* in ns */,
		/* out */ ulonglong *FilePos /* in bytes from start of file */,
		/* out */ unsigned int *FrameSize /* in bytes */,
		/* out */ void** FrameRef, //Picard
		/* out */ unsigned int *FrameFlags);

	/* Read raw frame data, invokes underlying io methods,
	* but keeps track of the file pointer for internal buffering.
	* Returns the -1 if error occurred, or 0 on success
	*/
	X int	      mkv_ReadData(/* in */   MatroskaFile *mf,
		/* in */   ulonglong FilePos,
		/* out */  void *Buffer,
		/* in */   unsigned int Count);

#ifdef MATROSKA_COMPRESSION_SUPPORT
	/* Compressed streams support */
	struct CompressedStream;

	typedef struct CompressedStream CompressedStream;

	X CompressedStream  *cs_Create(/* in */	MatroskaFile *mf,
		/* in */	unsigned tracknum,
		/* out */	char *errormsg,
		/* in */	unsigned msgsize);
	X void		  cs_Destroy(/* in */ CompressedStream *cs);

	/* advance to the next frame in matroska stream, you need to pass values returned
	* by mkv_ReadFrame */
	X void		  cs_NextFrame(/* in */ CompressedStream *cs,
		/* in */ ulonglong pos,
		/* in */ unsigned size);

	/* read and decode more data from current frame, return number of bytes decoded,
	* 0 on end of frame, or -1 on error */
	X int		  cs_ReadData(CompressedStream *cs,char *buffer,unsigned bufsize);

	/* return error message for the last error */
	X const char	  *cs_GetLastError(CompressedStream *cs);
#endif

#ifdef __cplusplus
}
#endif

#undef X

#endif
