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
 * $Id: overlay.h 615 2006-01-26 16:57:51Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#ifndef __OVERLAY_H
#define __OVERLAY_H

// default colorkey to use
#define COLORKEY			CRGB(32,0,32)

#define OVERLAY_CLASS		FOURCC('O','V','L','A')
#define OVERLAY_SUBTITLE_ID FOURCC('O','V','L','S')
#define DDRAW_ID			FOURCC('D','R','A','W')
#define DIRECT_ID			FOURCC('D','I','R','C')
#define FLYTV_ID			FOURCC('F','L','Y','T')
#define GAPI_ID				FOURCC('G','A','P','I')
#define GDI_ID				FOURCC('G','D','I','_')
#define RAW_ID				FOURCC('R','A','W','D')
#define XSCALE_ID			FOURCC('X','S','C','2')
#define XSCALE_LOW_ID		FOURCC('X','S','L','2')
#define HIRES_ID			FOURCC('H','I','R','S')
#define S1D13806_ID			FOURCC('S','1','D','1')
#define CONSOLE_ID			FOURCC('O','C','O','N')
#define OVERLAY_SYMBIAN_ID	FOURCC('O','S','Y','M')
#define ANDROID_SURFACE_ID	FOURCC('A','S','U','R')
#define OVERLY_THUMB_ID		FOURCC('O','T','U','B')


// special flag for get/set subtitle process function (ovlblitsub)
#define OVERLAY_INPUT_SUB_PROCESS	0x88
#define ANDROID_OUTPUT_SUB_PROCESS	0x89

#define OVERLAY_THUMB_INFO			0x8A //width height pitch plane

#define SUBTITLE_GDI		0x01
#define SUBTITLE_GAPI		0x02
#define SUBTITLE_ATI3200	0x04
#define SUBTITLE_INTEL2700G	0x08

#define SUBT_FONTNAME		0x10
#define SUBT_HIRESFONTSIZE	0x11
#define SUBT_LOWRESFONTSIZE	0x12
#define SUBT_LOWRESBOUNDARY	0x13
#define SUBT_SPEED			0x14
#define SUBT_TYPE			0x15
#define SUBT_ENABLED		0x16
#define SUBT_FONTBOLD		0x17
#define SUBT_OLDSTYLE		0x18
#define SUBT_FPS			0x19
#define SUBT_NOHIDE			0x1A
#define SUBT_FONTCOLOR		0x1B

#define SUBT_TOPALIGN		0x20
#define SUBT_MARGINH		0x21
#define SUBT_MARGINV		0x22

int  DrawSubInMem(void * p,void* Overlay,video* Video,void* pSubInfo,int OverlayType,int Pitch);
void DrawSubtitle(void *p, short* dest);

#define DDRAW_COLORKEY		0x100
#define DDRAW_BLIT			0x101
#define DDRAW_BLITSTRETCH	0x102
#define DDRAW_FORMAT		0x103

#define DDRAWFORMAT_ENUM	FOURCC('D','R','A','F')
#define DDRAWFORMAT_AUTO	0x100
#define DDRAWFORMAT_YV12	0x101
#define DDRAWFORMAT_YUY2	0x102
#define DDRAWFORMAT_RGB		0x103

#define RAW_WIDTH		0x100
#define RAW_HEIGHT		0x101
#define RAW_PITCHX		0x102
#define RAW_PITCHY		0x103
#define RAW_BPP			0x104
#define RAW_FORMAT		0x105
#define RAW_POINTER		0x106

#define GAPI_WIDTH		0x100
#define GAPI_HEIGHT		0x101
#define GAPI_PITCHX		0x102
#define GAPI_PITCHY		0x103
#define GAPI_BPP		0x104
#define GAPI_FORMAT		0x105
#define GAPI_POINTER	0x106
#define GAPI_DRAM		0x107
#define GAPI_OPEN_ERROR	0x200

#define HIRES_TRIPLEBUFFER			0x100
#define HIRES_USEBORDER				0x101
#define HIRES_USEBORDER_INFO		0x102

#define XSCALEDRIVER_ID				FOURCC('X','S','C','D')
#define XSCALEDRIVER_STRETCH		0x100
#define XSCALEDRIVER_FLIP			0x101
#define XSCALEDRIVER_ALLOC_AT_START	0x102
#define XSCALEDRIVER_DETECTED		0x103


typedef int (*ovlfunc)(void* This);
typedef void (*ovldone)(void* This);
typedef int (*ovlblit)(void* This,const constplanes Data,const constplanes LastData);
typedef int (*ovlprocsub)(void* This,void* SubDrawInfo);
typedef int (*ovllock)(void* This,planes Planes, bool_t OnlyAligned);

typedef struct SubtitleInfo
{
	tchar_t szString[1024];
	int x;
	int y;
	tchar_t szFontName[128];
	int FontSize;
	int FontColor;
	int HiResFontSize;
	int LowResFontSize;
	tick_t Speed;
	struct subtitle_draw_info *next;

}SubtitleInfo;

typedef struct
{
	int32_t		Height;
	int32_t		Width;
	int32_t		Pitch;
	void*		Plane;
	ovldone		Done;
	int32_t		tick;
	uint8_t		RGBNum;//2,4
}thumb_info;

typedef struct overlay
{
	node Node;

	ovlfunc Init;
	ovldone Done;
	ovlblit Blit;
	ovlprocsub ProcSub;
	ovlfunc Update;
	ovlfunc UpdateShow;
	ovlfunc Reset;
	ovllock Lock;
	ovlfunc Unlock;

	idct* AccelIDCT;
	int SetFX;	
	int ClearFX;
	int Caps;
	packetformat Output;

	// these are handled by overlay
	bool_t DoPowerOff; // use backup/restore in background
	bool_t Primary;
	bool_t Overlay;
	bool_t Disabled; // created with width=0 or height=0
	bool_t Inited;
	bool_t Background;
	bool_t Updating;
	bool_t ForceUpdate; 
	bool_t FullScreenViewport;
	bool_t Dirty;
	bool_t Visible;
	bool_t Clipping;
	bool_t AutoPrerotate;
	bool_t Show;
	bool_t InputHintDir;
	bool_t Play;
	bool_t TurningOff;
	tick_t LastTime;
	tick_t CurrTime;
	int Total;
	int Dropped;
	rgbval_t ColorKey;
	void* Soft;
	rect Viewport;
	rect SrcAlignedRect;
	rect GUIAlignedRect;
	rect DstAlignedRect;
	blitfx FX;
	blitfx OrigFX;
	fraction Aspect;
	bool_t PreRotate;
	int InputDirection;
	int PrefDirection;
	bool_t UpdateInputFormat;
	void* Module;
	pin Pin;
	packetformat Input;
	idctbackup Backup;
	int ErrorTime;

	int Reserved[10];

} overlay;

DLL int OverlayEnum(overlay* p,int* No,datadef* DataType);
DLL int OverlayGet(overlay* p,int No,void* Data,int Size);
DLL int OverlaySet(overlay* p,int No,const void* Data,int Size);
DLL int OverlayUpdateAlign(overlay* p);
DLL int OverlayUpdateShow(overlay* p, bool_t Temp);
DLL int OverlayBlitImage(overlay* p, const planes Data, const planes DataLast);
DLL int OverlayUpdateFX(overlay* p, bool_t ForceUpdate);
DLL void OverlayClearBorder(overlay* p);
DLL int OverlayDefaultBlit(overlay* p, const constplanes Data, const constplanes DataLast);
DLL int OverlayDefaultProcSub(void* This,void* SubDrawInfo);
void Overlay_Init();
void Overlay_Done();

#define GETRAWFRAMEBUFFER   0x00020001

#define FORMAT_565 1
#define FORMAT_555 2
#define FORMAT_OTHER 3

typedef struct RawFrameBufferInfo
{
	uint16_t wFormat;
	uint16_t wBPP;
	char *pFramePointer;
	int cxStride;
	int cyStride;
	int cxPixels;
	int cyPixels;

} RawFrameBufferInfo;

#define ESC_QUERYESCSUPPORT 8
#define GETGXINFO			0x00020000

#define kfDirect			0x20
#define kfDirect555			0x40
#define kfDirect565			0x80
#define kfLandscape			0x08

typedef struct GXDeviceInfo 
{
	long Version;           //00
	void * pvFrameBuffer;   //04
	unsigned long cbStride; //08
	unsigned long cxWidth;  //0c
	unsigned long cyHeight; //10
	unsigned long cBPP;     //14
	unsigned long ffFormat; //18
	char Unused[0x84-7*4];
} GXDeviceInfo;

#endif
