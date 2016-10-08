#include "../common.h"

#ifdef _WIN32

//#define BLITTEST

#define WIN32_LEAN_AND_MEAN
#ifndef STRICT
#define STRICT
#endif
#include <windows.h>

//font 
#define MAXSUBTITLECOUNT		5000
#define DEFAULTFONTNAME			T("Tahoma")
#define DEFAULTHIRESFONTSIZE	28
#define DEFAULTLOWRESFONTSIZE	16
#define DEFAULTLOWRESBOUNDARY	400

typedef struct overlay_subtitle
{
	node		VMT;
	//font
	bool_t		Enabled;
	int			marginH;
	int			marginV;
	bool_t		topAlign;
	bool_t		NoHide;

	COLORREF	fontColor;

	tchar_t		FontName[MAXPATH];
	tchar_t		ColorName[10];
	bool_t		FontBold;
	int			HiResFontSize;
	int			LowResFontSize;
	int			LowResBoundary;

	int			SrcPtr, SrcGap;
	int			DstPtr, DstGap;
	int			DstStepX, DstStepY;
	int			DstBPP;

	int			SubWidth, SubHeight;

	HFONT		hFont;
	int			FontSize;
	HFONT		hSFont;
	int			SFontSize;

	int			MaxWidth;
	int			MaxHeight;
	HBITMAP		FontDIBBitmap;
	HGDIOBJ		FontBitmap; //bitmap returned by selectobject
	HBRUSH		FontBackBrush;
	HDC			FontDC;
	short		*FontPlanes;
} overlay_subtitle;


int DrawSubInMem(overlay_subtitle* p,overlay* Overlay,video* Video,SubtitleInfo* pSubInfo,int OverlayType,int Pitch)
{
	int saveHeight;
	int DstWidth, DstHeight;
	int DstX, DstY;
	int DstPitch;
	int bottomgap;
	COLORREF crColor[2] = {0x00FFFFFF,0x00000000};
	HFONT obj;
	RECT tRect;
	DstWidth = Overlay->DstAlignedRect.Width;
	DstHeight = Overlay->DstAlignedRect.Height;
	DstX = 0;
	DstY = 0;

	if(!p->Enabled)
		return ERR_NONE;

	if((boolmem_t)((Overlay->FX.Direction & DIR_SWAPXY) != 0))
		SwapInt(&DstWidth, &DstHeight);

	p->SubWidth = DstWidth;
	if(p->SubWidth > p->MaxWidth)
		p->SubWidth = p->MaxWidth;

	bottomgap = 4;
	if(p->SubWidth < p->LowResBoundary) bottomgap >>= 1;

	tRect.left = 0;
	tRect.top = 0;
	tRect.right = p->SubWidth;
	tRect.bottom = DstHeight - bottomgap; 

	if(tRect.right < p->LowResBoundary)
	{
		tRect.bottom = (int)(tRect.bottom / (p->LowResFontSize)) * (p->LowResFontSize);
		obj = SelectObject(p->FontDC, p->hSFont);
	}
	else
	{
		tRect.bottom = (int)(tRect.bottom / p->HiResFontSize) * (p->HiResFontSize);
		obj = SelectObject(p->FontDC, p->hFont);
	}

	SetBkColor(p->FontDC, crColor[1]);
	SetTextColor(p->FontDC, crColor[0]);

	FillRect(p->FontDC,&tRect, p->FontBackBrush);

	tRect.left = p->marginH;
	tRect.right = p->SubWidth-p->marginH;
	p->SubHeight = DrawText(p->FontDC, pSubInfo->szString, tcslen(pSubInfo->szString), &tRect, DT_WORDBREAK | DT_CENTER);

	if (obj != NULL) SelectObject(p->FontDC, obj);

	if(p->SubHeight > 0)
	{
		saveHeight=p->SubHeight;
		p->SubHeight++;

		if (p->topAlign) 
			p->SubHeight=DstHeight;

		if (p->topAlign) 
			p->SubHeight-=p->marginV;
		else
			p->SubHeight+=p->marginV;

		p->DstBPP = GetBPP(&Video->Pixel);
		p->SrcGap = p->MaxWidth - p->SubWidth;
		p->SrcPtr = 0;

		DstPitch = Video->Pitch / (p->DstBPP >> 3);
		
		p->DstStepX = 1;
		p->DstStepY = DstPitch;

		if (Overlay->FX.Direction & DIR_MIRRORLEFTRIGHT)
			p->DstStepX = -p->DstStepX;

		if (Overlay->FX.Direction & DIR_MIRRORUPDOWN)
			p->DstStepY = -p->DstStepY;

		if (Overlay->FX.Direction & DIR_SWAPXY)
			SwapInt(&p->DstStepX, &p->DstStepY);

		switch(Overlay->FX.Direction & (DIR_SWAPXY | DIR_MIRRORLEFTRIGHT | DIR_MIRRORUPDOWN))
		{
		case 0:
			p->DstGap = DstPitch - p->SubWidth;
			p->DstPtr = (DstY + DstHeight - p->SubHeight) * DstPitch + DstX;
			break;
		case DIR_MIRRORLEFTRIGHT:
			p->DstGap = DstPitch + p->SubWidth;
			p->DstPtr = (DstY + DstHeight - p->SubHeight) * DstPitch + DstX + DstWidth;
			break;
		case DIR_MIRRORUPDOWN:
			p->DstGap = -(DstPitch + p->SubWidth);
			p->DstPtr = (DstY + p->SubHeight) * DstPitch + DstX;
			break;
		case DIR_MIRRORLEFTRIGHT | DIR_MIRRORUPDOWN:
			p->DstGap = -(DstPitch - p->SubWidth);
			p->DstPtr = (DstY + p->SubHeight + bottomgap) * DstPitch + DstX + DstWidth;
			break;
		case DIR_SWAPXY:
			p->DstGap = (p->SubWidth * DstPitch) - 1;
			p->DstPtr = (DstY + p->SubWidth) * DstPitch + DstX + p->SubHeight + bottomgap;
			break;
		case DIR_SWAPXY | DIR_MIRRORLEFTRIGHT:
			p->DstGap = -(p->SubWidth * DstPitch) - 1;
			p->DstPtr = DstY * DstPitch + DstX + p->SubHeight + bottomgap;
			break;
		case DIR_SWAPXY | DIR_MIRRORUPDOWN:
			p->DstGap = (p->SubWidth * DstPitch) + 1;
			p->DstPtr = (DstY + DstWidth) * DstPitch + DstX + DstHeight - p->SubHeight;
			break;
		case DIR_SWAPXY | DIR_MIRRORLEFTRIGHT | DIR_MIRRORUPDOWN:
			p->DstGap = -(p->SubWidth * DstPitch) + 1;
			p->DstPtr = DstY * DstPitch + DstX + DstHeight - p->SubHeight;
			break;
		}
		p->SubHeight=saveHeight;
	}

	return ERR_NONE;
}


// Blit subtitle in screen
void DrawSubtitle(overlay_subtitle *p, short* dest)
{
	int x,y;
	short *sour = p->FontPlanes;

	if (p->SubHeight == 0) 
		return;

	if(!p->Enabled)
		return;

	if (dest == NULL || sour == NULL || p->DstPtr<0)
	{
		return; 
	}

	switch(p->DstBPP)
	{
	case 8:
		{ // for ATI 3200(Y FontPlanes)
			char *destb = (char*)dest + p->DstPtr;
			short *sourb = sour + p->SrcPtr;
			for (y=0;y<p->SubHeight;y++) 
			{
				for (x=0;x<p->SubWidth;x++, sourb++, destb+=p->DstStepX)
				{
					if (x && y && *sourb != 0)
					{
						*(destb-p->DstStepX) = 0;
						*(destb+p->DstStepX) = 0;
						*(destb+p->DstStepY) = 0;
						*(destb-p->DstStepY) = 0;
					}
				}
				sourb += p->SrcGap;
				destb += p->DstGap;
			}
			destb = (char*)dest + p->DstPtr;
			sourb = sour + p->SrcPtr;
			for (y=0;y<p->SubHeight;y++) {
				for (x=0;x<p->SubWidth;x++, sourb++, destb+=p->DstStepX)
				{
					if (*sourb != 0)
					{
						*destb = -1;
					}
				}
				sourb += p->SrcGap;
				destb += p->DstGap;
			}
		}
		break;
	case 16:
		{ // for GDI, GAPI(RGB FontPlanes)
			short *destb = (short*)dest + p->DstPtr;
			short *sourb = sour + p->SrcPtr;
			short col=((p->fontColor&0xFF0000)>>5)|((p->fontColor&0x00FF00)>>3)|((p->fontColor&0x0000FF));
			for (y=0;y<p->SubHeight;y++)
			{
				for (x=0;x<p->SubWidth;x++, sourb++, destb+=p->DstStepX)
				{
					if (x && y && *sourb != 0)
					{
						*(destb-p->DstStepX) = 0;
						*(destb+p->DstStepX) = 0;
						*(destb+p->DstStepY) = 0;
						*(destb-p->DstStepY) = 0;
					}
				}
				sourb += p->SrcGap;
				destb += p->DstGap;
			}
			destb = (short*)dest + p->DstPtr;
			sourb = sour + p->SrcPtr;
			for (y=0;y<p->SubHeight;y++)
			{
				for (x=0;x<p->SubWidth;x++, sourb++, destb+=p->DstStepX)
				{
					if (*sourb != 0)
					{
						*destb = col;
					}
				}
				sourb += p->SrcGap;
				destb += p->DstGap;
			}
		}
		break;
	case 32:
		{ // for PC
			long *destb = (long*)dest + p->DstPtr;
			short *sourb = sour + p->SrcPtr;
			for (y=0;y<p->SubHeight;y++)
			{
				for (x=0;x<p->SubWidth;x++, sourb++, destb+=p->DstStepX)
				{
					if (x && y && *sourb != 0)
					{
						*(destb-p->DstStepX) = 0;
						*(destb+p->DstStepX) = 0;
						*(destb+p->DstStepY) = 0;
						*(destb-p->DstStepY) = 0;
					}
				}
				sourb += p->SrcGap;
				destb += p->DstGap;
			}
			destb = (long*)dest + p->DstPtr;
			sourb = sour + p->SrcPtr;
			for (y=0;y<p->SubHeight;y++)
			{
				for (x=0;x<p->SubWidth;x++, sourb++, destb+=p->DstStepX)
				{
					if (*sourb != 0)
					{
						*destb =  p->fontColor;//((*sourb & 0x7C00)<<9)+((*sourb & 0x3E0)<<6)+((*sourb & 0x1F)<<3);
					}
				}
				sourb += p->SrcGap;
				destb += p->DstGap;
			}
		}
		break;
	}
}

// Create DIB Bitmap for Draw Subtitle
static int SubtitleCreateBitmap(overlay_subtitle* p) {
	struct {
		BITMAPINFOHEADER bmiHeader;
		int RBitMask;
		int GBitMask;
		int BBitMask;
	} Info;

	if(p->FontPlanes != NULL) 
	{
		return 1;
	}

	memset(&Info,0,sizeof(Info));
	Info.bmiHeader.biSize	= sizeof(Info.bmiHeader);
	Info.bmiHeader.biWidth	= p->MaxWidth;
	Info.bmiHeader.biHeight	= -(p->MaxHeight);
	Info.bmiHeader.biPlanes	= 1;
	Info.bmiHeader.biBitCount = 16;
	Info.bmiHeader.biCompression = BI_BITFIELDS;
	Info.RBitMask = 0x000000FF;
	Info.GBitMask = 0x0000FF00;
	Info.BBitMask = 0x00FF0000;

	p->FontDIBBitmap = CreateDIBSection(NULL, (BITMAPINFO*)&Info, DIB_RGB_COLORS, &p->FontPlanes, NULL, 0);

	if (p->FontDIBBitmap) 
	{
		if(p->FontPlanes)
		{
			p->FontBitmap = SelectObject(p->FontDC, p->FontDIBBitmap);
			p->FontBackBrush = CreateSolidBrush(RGB(0,0,0));
		}
		else 
		{
			DeleteObject(p->FontDIBBitmap);
			p->FontDIBBitmap = NULL;
		}
	}

	return p->FontPlanes != NULL;
}
//font end
static int FontCreate(overlay_subtitle* p) {
	LOGFONT LogFont;

	memset(&(LogFont), 0, sizeof(LogFont));

	tcscpy_s(LogFont.lfFaceName,MAXPATH, p->FontName);

	if (p->FontBold) LogFont.lfWeight = 700; else LogFont.lfWeight=400;
	LogFont.lfCharSet = DEFAULT_CHARSET;
	LogFont.lfQuality = DEFAULT_QUALITY; 
	LogFont.lfPitchAndFamily = FF_DONTCARE;


	if (p->hFont) 
		DeleteObject(p->hFont);
	LogFont.lfHeight = p->HiResFontSize;
	p->FontSize = p->HiResFontSize;
	p->hFont = CreateFontIndirect(&LogFont);

	if (p->hSFont)
		DeleteObject(p->hSFont);
	LogFont.lfHeight = p->LowResFontSize;
	p->SFontSize = p->LowResFontSize;
	p->hSFont = CreateFontIndirect(&LogFont);	

	return ERR_NONE;
}
static const datatable SubtitleParams[] = 
{
	{ SUBT_ENABLED,			TYPE_BOOL, DF_SETUP },
	{ SUBT_FONTNAME,		TYPE_STRING, DF_SETUP|DF_GAP},
	{ SUBT_FONTCOLOR,		TYPE_STRING, DF_SETUP },
	{ SUBT_HIRESFONTSIZE,	TYPE_INT, DF_SETUP },
	{ SUBT_LOWRESFONTSIZE,	TYPE_INT, DF_SETUP },
	{ SUBT_LOWRESBOUNDARY,	TYPE_INT, DF_SETUP },
	{ SUBT_FONTBOLD,		TYPE_BOOL, DF_SETUP },

	{ SUBT_TOPALIGN,		TYPE_BOOL, DF_SETUP|DF_GAP },
	{ SUBT_MARGINH,			TYPE_INT, DF_SETUP|DF_NEG },
	{ SUBT_MARGINV,			TYPE_INT, DF_SETUP|DF_NEG },

	DATATABLE_END(OVERLAY_SUBTITLE_ID)
};
static int Enum( overlay_subtitle* p, int* No, datadef* Param )
{
	return NodeEnumTable(No,Param,SubtitleParams);
	//return NodeEnumType(&No,Param,NodeParams,FlowParams,SubtitleParams,NULL);
}

static int Get( overlay_subtitle* p, int No, void* Data, int Size )
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case SUBT_FONTNAME: GETSTRING(p->FontName); break;
	case SUBT_FONTCOLOR: GETSTRING(p->ColorName); break;
	case SUBT_HIRESFONTSIZE: GETVALUE(p->HiResFontSize,int); break;
	case SUBT_LOWRESFONTSIZE: GETVALUE(p->LowResFontSize,int); break;
	case SUBT_LOWRESBOUNDARY: GETVALUE(p->LowResBoundary,int); break;
	case SUBT_FONTBOLD: GETVALUE(p->FontBold,bool_t); break;
	case SUBT_TOPALIGN: GETVALUE(p->topAlign,bool_t); break;
	case SUBT_MARGINH: GETVALUE(p->marginH,int); break;
	case SUBT_MARGINV: GETVALUE(p->marginV,int); break;
	case SUBT_ENABLED: GETVALUE(p->Enabled,bool_t); break;
	}
	return Result;
}

static int Set( overlay_subtitle* p, int No, const void* Data, int Size )
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case SUBT_FONTNAME: SETSTRING(p->FontName);Result = FontCreate(p); break;
	case SUBT_FONTCOLOR: SETSTRING(p->ColorName); p->fontColor=0xFFFFFF; stscanf(p->ColorName,T("%6x"),&(p->fontColor));if (!p->fontColor) p->fontColor=0xFFFFFF;Result = ERR_NONE; break;
	case SUBT_HIRESFONTSIZE: SETVALUE(p->HiResFontSize,int,Result = FontCreate(p)); break;
	case SUBT_LOWRESFONTSIZE: SETVALUE(p->LowResFontSize,int,Result = FontCreate(p)); break;
	case SUBT_LOWRESBOUNDARY: SETVALUE(p->LowResBoundary,int,Result = FontCreate(p)); break;
	case SUBT_FONTBOLD: SETVALUE(p->FontBold,bool_t,Result = FontCreate(p)); break;
	case SUBT_TOPALIGN: SETVALUE(p->topAlign,bool_t,Result = FontCreate(p)); break;
	case SUBT_MARGINH: SETVALUE(p->marginH,int,Result = FontCreate(p)); break;
	case SUBT_MARGINV: SETVALUE(p->marginV,int,Result = FontCreate(p)); break;
	case SUBT_ENABLED: SETVALUE(p->Enabled,bool_t,Result = ERR_NONE);break;
	}
	return Result;
}

static int Create(overlay_subtitle* p)
{
	StringAdd(1,OVERLAY_SUBTITLE_ID,NODE_NAME,T("Subtitles"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_FONTNAME,T("Font"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_HIRESFONTSIZE,T("Hi res size"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_LOWRESFONTSIZE,T("Low res size"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_LOWRESBOUNDARY,T("Low res boundary"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_FONTBOLD,T("Bold font"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_ENABLED,T("Enabled"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_TOPALIGN,T("Top align"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_MARGINH,T("Horiz. margin"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_MARGINV,T("Vert. margin"));
	StringAdd(1,OVERLAY_SUBTITLE_ID,SUBT_FONTCOLOR,T("Font color"));

	p->VMT.Get = Get;
	p->VMT.Set = Set;
	p->VMT.Enum = Enum;

	//font
	p->Enabled=1;
	tcscpy_s(p->FontName,MAXPATH, DEFAULTFONTNAME);
	p->fontColor=0xFFFFFF;
	*(p->ColorName)=0;
	p->HiResFontSize = DEFAULTHIRESFONTSIZE;
	p->LowResFontSize = DEFAULTLOWRESFONTSIZE;
	p->LowResBoundary = DEFAULTLOWRESBOUNDARY;
	p->FontBold=1;

	p->marginH=0;
	p->marginV=0;
	p->topAlign=0;
	p->NoHide=0;

	tcscpy_s(p->FontName,MAXPATH, DEFAULTFONTNAME);
	p->HiResFontSize = DEFAULTHIRESFONTSIZE;
	p->LowResFontSize = DEFAULTLOWRESFONTSIZE;
	p->LowResBoundary = DEFAULTLOWRESBOUNDARY;

	p->FontPlanes = NULL;
	p->FontBitmap = NULL;

	p->MaxWidth = GetSystemMetrics(SM_CXSCREEN);
	p->MaxHeight = GetSystemMetrics(SM_CYSCREEN);
	if(p->MaxWidth > p->MaxHeight)
		p->MaxHeight = p->MaxWidth;
	else
		p->MaxWidth = p->MaxHeight;

	p->FontDC = CreateCompatibleDC(NULL);
	p->hFont = NULL;
	p->hSFont = NULL;
	p->FontSize = 0;
	p->SFontSize = 0;
	p->SubHeight = 0;

	SubtitleCreateBitmap(p);
	FontCreate(p);
	return ERR_NONE;
}

static void Delete(overlay_subtitle* p)
{
	if (p->FontBitmap)
	{
		SelectObject(p->FontDC, p->FontBitmap);
		DeleteObject(p->FontDIBBitmap);
		p->FontDIBBitmap = NULL;

		DeleteObject(p->FontBackBrush);
		p->FontBackBrush = NULL;
	}
	p->FontPlanes = NULL;
	DeleteDC(p->FontDC);
	p->FontDC = NULL;

	if (p->hFont)
		DeleteObject(p->hFont);
	p->hFont = NULL;

	if (p->hSFont) 
		DeleteObject(p->hSFont);
	p->hSFont = NULL;

	p->SubHeight = 0;
}
static const nodedef OVERLAY_SUBTITLE = 
{
	sizeof(overlay_subtitle)|CF_GLOBAL|CF_SETTINGS,
	OVERLAY_SUBTITLE_ID,
	NODE_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
	(nodedelete)Delete,
};

void OverlaySubtitle_Init() 
{ 
	NodeRegisterClass(&OVERLAY_SUBTITLE);
}

void OverlaySubtitle_Done() 
{ 
	NodeUnRegisterClass(OVERLAY_SUBTITLE_ID);
}

#endif