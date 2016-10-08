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

#include "../common.h"

#define THUMB_WIDTH 120
#define THUMB_HEIGHT 120
#define THUMB_PIXEL_SIZE 4

typedef struct surface_thumb
{
	overlay 	Overlay;
	thumb_info  Thumb;
} surface_thumb;


static void Done(surface_thumb* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Done"));
	if(p->Thumb.Plane != NULL)
		free(p->Thumb.Plane);
	p->Thumb.Plane = NULL;
}

static int Init(surface_thumb* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Init"));

	p->Overlay.Output.Format.Video.Width = p->Thumb.Width;
	p->Overlay.Output.Format.Video.Height = p->Thumb.Height;
	p->Overlay.Output.Format.Video.Pitch = p->Thumb.Pitch;
	p->Overlay.Output.Format.Video.Aspect = ASPECT_ONE;
	p->Overlay.Output.Format.Video.Direction = 0;

	p->Thumb.Plane =  malloc(p->Thumb.Height*p->Thumb.Width*p->Thumb.RGBNum);

	DEBUG_MSG5(DEBUG_VIDEO,T("surface_thumb::Init:w %d,h %d,bpr %d,RGB %d,Ptr"),p->Thumb.Width,p->Thumb.Height,p->Thumb.Pitch,p->Thumb.RGBNum,p->Thumb.Plane);

	if(p->Thumb.RGBNum == 4)
	{
		DefaultRGB(&p->Overlay.Output.Format.Video.Pixel,32,8,8,8,0,0,0);
	}
	else if(p->Thumb.RGBNum == 2)
	{
		DefaultRGB(&p->Overlay.Output.Format.Video.Pixel,16,5,6,5,0,0,0);
	}
	else
		return ERR_INVALID_DATA;

	p->Overlay.ClearFX = BLITFX_ONLYDIFF;

	return ERR_NONE;
}

static int Reset(surface_thumb* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Reset"));
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Reset::Done"));
	Done(p);
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Reset::Init"));
	Init(p);
	return ERR_NONE;
}


static int Update(surface_thumb* p)
{
	rect OldGUI = p->Overlay.GUIAlignedRect;
	rect Old = p->Overlay.DstAlignedRect;

	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Update"));
	DEBUG_MSG4(DEBUG_VIDEO,T("surface_thumb::BLIT Viewport:%d %d %d %d"),p->Overlay.Viewport.x,p->Overlay.Viewport.y,p->Overlay.Viewport.Width,p->Overlay.Viewport.Height);

	Reset(p);

	VirtToPhy(&p->Overlay.Viewport,&p->Overlay.DstAlignedRect,&p->Overlay.Output.Format.Video);
	VirtToPhy(NULL,&p->Overlay.SrcAlignedRect,&p->Overlay.Input.Format.Video);

	BlitRelease(p->Overlay.Soft);
	p->Overlay.Soft = BlitCreate(&p->Overlay.Output.Format.Video,&p->Overlay.Input.Format.Video,&p->Overlay.FX,&p->Overlay.Caps);
	BlitAlign(p->Overlay.Soft, &p->Overlay.DstAlignedRect, &p->Overlay.SrcAlignedRect);

	DEBUG_MSG4(DEBUG_VIDEO,T("surface_thumb::BLIT DstRect:%d %d %d %d"),p->Overlay.DstAlignedRect.x,p->Overlay.DstAlignedRect.y,p->Overlay.DstAlignedRect.Width,p->Overlay.DstAlignedRect.Height);
	DEBUG_MSG4(DEBUG_VIDEO,T("surface_thumb::BLIT SrcRect:%d %d %d %d"),p->Overlay.SrcAlignedRect.x,p->Overlay.SrcAlignedRect.y,p->Overlay.SrcAlignedRect.Width,p->Overlay.SrcAlignedRect.Height);


	if (p->Overlay.Show && !EqRect(&Old,&p->Overlay.DstAlignedRect))
	{

	}
	return ERR_NONE;
}
static int Set(surface_thumb* p, int No, const void* Data, int Size)
{
	int Result = OverlaySet(&p->Overlay,No,Data,Size);
	DEBUG_MSG2(DEBUG_VIDEO,T("surface_thumb::Set(No 0x%x Data 0x%x"),No,Data);
	switch (No)
	{
	case OVERLAY_THUMB_INFO:
		{
			if(Data == NULL)
			{
				Result = ERR_INVALID_DATA;
			}
			else
			{
				thumb_info* ThumbSrc = (thumb_info*)Data;
				bool_t bUpdate = 0;

				if(p->Thumb.Plane!=NULL)
				{
					memset((void*)p->Thumb.Plane,0,p->Thumb.Height*p->Thumb.Width*p->Thumb.RGBNum);
				}
				if((p->Thumb.Height !=  ThumbSrc->Height) \
					||(p->Thumb.Height !=  ThumbSrc->Height) \
					||(p->Thumb.RGBNum !=  ThumbSrc->RGBNum))
				{
					bUpdate = 1;
				}
				p->Thumb.Height =  ThumbSrc->Height;
				p->Thumb.Width 	=  ThumbSrc->Width;
				p->Thumb.RGBNum =  ThumbSrc->RGBNum;
				p->Thumb.Pitch 	=  ThumbSrc->Width*ThumbSrc->RGBNum;
				p->Thumb.tick	=  ThumbSrc->tick;

				if(p->Thumb.Done != ThumbSrc->Done)
					p->Thumb.Done	=  ThumbSrc->Done;

				if(bUpdate)
					Update(p);
			}
		}
		break;
	}
	return Result;
}

static int Get(surface_thumb* p, int No, void* Data, int Size)
{
	int Result = OverlayGet(&p->Overlay,No,Data,Size);
	switch (No)
	{
	case OVERLAY_THUMB_INFO:
		memcpy((void*)Data,&(p->Thumb),sizeof(thumb_info));
		DEBUG_MSG5(DEBUG_VIDEO,T("surface_thumb::Get:w %d,h %d,bpr %d,Ptr 0x%x"),p->Thumb.Width,p->Thumb.Height,p->Thumb.Pitch,p->Thumb.RGBNum,p->Thumb.Plane);
		break;
	}
	return Result;
}
static int Lock(surface_thumb* p, planes Planes, bool_t OnlyAligned)
{
	DEBUG_MSG5(DEBUG_VIDEO,T("surface_thumb::Lock:w %d,h %d,bpr %d,RGB %d,Ptr 0x%x"),p->Thumb.Width,p->Thumb.Height,p->Thumb.Pitch,p->Thumb.RGBNum,p->Thumb.Plane);
	memset((void*)p->Thumb.Plane,0,p->Thumb.Height*p->Thumb.Width*p->Thumb.RGBNum);
	Planes[0] = p->Thumb.Plane;
	return ERR_NONE;
}

static int Unlock(surface_thumb* p)
{
	DEBUG_MSG3(DEBUG_VIDEO,T("surface_thumb::Unlock: p->Thumb.Done 0x%x,Cur %d,ThumbTick %d"),p->Thumb.Done,p->Overlay.CurrTime,p->Thumb.tick);
	if(p->Thumb.Done&&p->Overlay.CurrTime>=p->Thumb.tick)
		p->Thumb.Done(p);
	return ERR_NONE;
}
static void Delete(surface_thumb* p)
{
	if(p->Thumb.Plane)
		free(p->Thumb.Plane);
	p->Thumb.Plane = NULL;
	memset(&(p->Thumb),0,sizeof(thumb_info));
}

static int Create(surface_thumb* p)
{
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::Create"));
	p->Thumb.Width = THUMB_WIDTH;
	p->Thumb.Height= THUMB_HEIGHT;
	p->Thumb.RGBNum= 4;
	p->Thumb.Pitch = THUMB_WIDTH*p->Thumb.RGBNum;
	p->Thumb.Plane =  malloc(p->Thumb.Height*p->Thumb.Width*p->Thumb.RGBNum);
	p->Thumb.tick  = TICKSPERSEC*3600*4;

	p->Overlay.Init 	= (ovlfunc)Init;
	p->Overlay.Done 	= (ovldone)Done;
	p->Overlay.Reset 	= (ovlfunc)Reset;
	p->Overlay.Lock 	= (ovllock)Lock;
	p->Overlay.Unlock 	= (ovlfunc)Unlock;
	p->Overlay.Update 	= (ovlfunc)Update;
	p->Overlay.Node.Set = (nodeset)Set;
	p->Overlay.Node.Get = (nodeget)Get;
	return ERR_NONE;
}

static const nodedef SURFACE_THUMB =
{
	sizeof(surface_thumb)|CF_GLOBAL,
	OVERLY_THUMB_ID,
	OVERLAY_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
};

void OverlaySufaceThumb_Init()
{
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::OverlaySuface_Init"));
	NodeRegisterClass(&SURFACE_THUMB);
}

void OverlaySufaceThumb_Done()
{
	DEBUG_MSG(DEBUG_VIDEO,T("surface_thumb::OverlaySuface_Done"));
	NodeUnRegisterClass(OVERLY_THUMB_ID);
}

