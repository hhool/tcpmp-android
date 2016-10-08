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
 * $Id: overlay_framebuffer.c 543 2006-01-07 22:06:24Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common/common.h"

#if defined(TARGET_ANDROID)

#include <jni.h>
#include "environment.h"

void 	CreateSurfaceLock();
void 	DestroySurfaceLock();
void 	LockSurface();
void 	UnlockSurface();
int 	GetSurfaceWidth() ;
int 	GetSurfaceHeight() ;
void* 	GetSurfaceBuffer();


typedef struct suface
{
	overlay 	Overlay;
	SubtitleInfo SubInfo;
	notify_draw	 SubDrawNotify;
} suface;


static void Done(suface* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Done"));

	//ResetSurfaceBuffer();
}

static int Init(suface* p)
{
	int sw, sh, bpr;

	DEBUG_MSG(DEBUG_VIDEO,T("suface::Init"));

	DEBUG_MSG(DEBUG_VIDEO,T("suface::Init::LockSurface"));
	LockSurface();
	sw = GetSurfaceWidth();
	sh = GetSurfaceHeight();
	bpr= GetSurfaceBPR();
	UnlockSurface();
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Init::UnlockSurface"));

	DEBUG_MSG3(DEBUG_VIDEO,T("suface::w %d,h %d,bpr %d"),sw,sh,bpr);

	p->Overlay.Output.Format.Video.Width = sw;
	p->Overlay.Output.Format.Video.Height = sh;
	p->Overlay.Output.Format.Video.Pitch = (bpr);
	p->Overlay.Output.Format.Video.Aspect = ASPECT_ONE;
	p->Overlay.Output.Format.Video.Direction = 0;

	DefaultRGB(&p->Overlay.Output.Format.Video.Pixel,16,5,6,5,0,0,0);

	p->Overlay.ClearFX = BLITFX_ONLYDIFF;

	return ERR_NONE;
}

static int Reset(suface* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Reset"));
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Reset::Done"));
	Done(p);
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Reset::Init"));
	Init(p);
	return ERR_NONE;
}


static int Update(suface* p)
{
	rect OldGUI = p->Overlay.GUIAlignedRect;
	rect Old = p->Overlay.DstAlignedRect;

	DEBUG_MSG(DEBUG_VIDEO,T("suface::Update"));
	DEBUG_MSG4(DEBUG_VIDEO,T("suface::BLIT Viewport:%d %d %d %d"),p->Overlay.Viewport.x,p->Overlay.Viewport.y,p->Overlay.Viewport.Width,p->Overlay.Viewport.Height);

	Reset(p);

	VirtToPhy(&p->Overlay.Viewport,&p->Overlay.DstAlignedRect,&p->Overlay.Output.Format.Video);
	VirtToPhy(NULL,&p->Overlay.SrcAlignedRect,&p->Overlay.Input.Format.Video);

	BlitRelease(p->Overlay.Soft);
	p->Overlay.Soft = BlitCreate(&p->Overlay.Output.Format.Video,&p->Overlay.Input.Format.Video,&p->Overlay.FX,&p->Overlay.Caps);
	BlitAlign(p->Overlay.Soft, &p->Overlay.DstAlignedRect, &p->Overlay.SrcAlignedRect);

	DEBUG_MSG4(DEBUG_VIDEO,T("suface::BLIT DstRect:%d %d %d %d"),p->Overlay.DstAlignedRect.x,p->Overlay.DstAlignedRect.y,p->Overlay.DstAlignedRect.Width,p->Overlay.DstAlignedRect.Height);
	DEBUG_MSG4(DEBUG_VIDEO,T("suface::BLIT SrcRect:%d %d %d %d"),p->Overlay.SrcAlignedRect.x,p->Overlay.SrcAlignedRect.y,p->Overlay.SrcAlignedRect.Width,p->Overlay.SrcAlignedRect.Height);


	if (p->Overlay.Show && !EqRect(&Old,&p->Overlay.DstAlignedRect))
	{

	}
	return ERR_NONE;
}
static int Set(suface* p, int No, const void* Data, int Size)
{
	DEBUG_MSG2(DEBUG_VIDEO,T("suface::Set(No 0x%x Data 0x%x"),No,Data);

	int Result = OverlaySet(&p->Overlay,No,Data,Size);
	switch (No)
	{
	case ANDROID_OUTPUT_SUB_PROCESS:
		p->SubDrawNotify = *(notify_draw*)Data;
		DEBUG_MSG6(DEBUG_VIDEO,T("suface::Set:VOUT_SUB_PROCESS:p->SubDrawNotify.Func 0x%x,p->SubDrawNotify.This 0x%x,p->SubDrawNotify.This Class %c%c%c%c"),
				p->SubDrawNotify.Func,p->SubDrawNotify.This,
				(signed char)(((node*)p->SubDrawNotify.This)->Class&0xff),
				(signed char)(((node*)p->SubDrawNotify.This)->Class>>8&0xff),
				(signed char)(((node*)p->SubDrawNotify.This)->Class>>16&0xff),
				(signed char)(((node*)p->SubDrawNotify.This)->Class>>24&0xff));
		break;
	}
	return Result;
}

static int Get(suface* p, int No, void* Data, int Size)
{
	int Result = OverlayGet(&p->Overlay,No,Data,Size);
	switch (No)
	{
	case ANDROID_OUTPUT_SUB_PROCESS:
		//GETVALUE(p->isUpdate,bool_t); break;
		break;
	}
	return Result;
}
static int Lock(suface* p, planes Planes, bool_t OnlyAligned)
{
	int sw, sh;
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Lock"));
	LockSurface();
	Planes[0] = GetSurfaceBuffer();

	sw = GetSurfaceWidth();
	sh = GetSurfaceHeight();

	memset((uint8_t*)Planes[0],0,sh*sw*2);

	DEBUG_MSG1(DEBUG_VIDEO,T("suface::Lock Ptr %x"),Planes[0]);
	return ERR_NONE;
}

static int Unlock(suface* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Unlock"));
	UnlockSurface();
	return ERR_NONE;
}

int ProcSub(suface* p,void* SubDrawInfo)
{
	if(p)
	{
		memcpy(&p->SubInfo,SubDrawInfo,sizeof(SubtitleInfo));
		DEBUG_MSG4(DEBUG_VIDEO,T("suface::p->SubDrawNotify.Func 0x%x,p->SubDrawNotify.This 0x%x,Procsub adr 0x%x,p->SubInfo: %s"),p->SubDrawNotify.Func,p->SubDrawNotify.This,ProcSub,p->SubInfo.szString);
		if(p->SubDrawNotify.Func)
		{
			p->SubDrawNotify.Func(p,&p->SubInfo,0);
		}
		DEBUG_MSG4(DEBUG_VIDEO,T("suface::p->SubDrawNotify.Func 0x%x,p->SubDrawNotify.This 0x%x,Procsub adr 0x%x,p->SubInfo: %s end"),p->SubDrawNotify.Func,p->SubDrawNotify.This,ProcSub,p->SubInfo.szString);
	}
	return ERR_NONE;
}
static int Create(suface* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("suface::Create"));
	p->Overlay.Init 	= (ovlfunc)Init;
	p->Overlay.Done 	= (ovldone)Done;
	p->Overlay.Reset 	= (ovlfunc)Reset;
	p->Overlay.Lock 	= (ovllock)Lock;
	p->Overlay.Unlock 	= (ovlfunc)Unlock;
	p->Overlay.Update 	= (ovlfunc)Update;
	p->Overlay.ProcSub 	= (ovlprocsub)ProcSub;
	p->Overlay.Node.Set = (nodeset)Set;
	p->Overlay.Node.Get = (nodeget)Get;
	DEBUG_MSG2(DEBUG_VIDEO,T("overlay_vsurface::Create::ProcSub adr 0x%x,p->SubInfo: %s"),ProcSub,p->SubInfo.szString);
	return ERR_NONE;
}

static const nodedef SURFACE =
{
	sizeof(suface)|CF_GLOBAL,
	ANDROID_SURFACE_ID,
	OVERLAY_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
};

void OverlaySuface_Init()
{
	DEBUG_MSG(DEBUG_VIDEO,T("suface::OverlaySuface_Init"));
	NodeRegisterClass(&SURFACE);
	CreateSurfaceLock();
}

void OverlaySuface_Done()
{
	DEBUG_MSG(DEBUG_VIDEO,T("suface::OverlaySuface_Done"));
	DestroySurfaceLock();
	NodeUnRegisterClass(ANDROID_SURFACE_ID);
}
#else
void OverlaySuface_Init(){DEBUG_MSG(DEBUG_VIDEO,T("NOT suface::OverlaySuface_Init"));}
void OverlaySuface_Done(){}
#endif
