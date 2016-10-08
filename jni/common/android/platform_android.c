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
 * $Id: platform_win32.c 622 2006-01-31 19:02:53Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"

#if defined(TARGET_ANDROID)

//file directory
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <android/log.h>
#include "../ini/libini.h"
#define LOG_TAG "JumPlayerCore"
#define LOGI(a)  __android_log_write(ANDROID_LOG_INFO,LOG_TAG,a)

#ifdef UNICODE
#define mopendir 				wopendir
#define mreaddir 				wreaddir
#define mrewinddir 				wrewinddir
#define mclosedir 				wclosedir
#define mtelldir    			wtelldir
#define mseekdir    			wseekdir
#define mstat					wstat
typedef struct wdirent 			mdirent;
typedef struct __EPOC32_WDIR 	MDIR;
#else
#define mopendir 				opendir
#define mreaddir 				readdir
#define mrewinddir 				rewinddir
#define mclosedir 				closedir
#define mtelldir    			telldir
#define mseekdir    			seekdir
#define mstat					stat
typedef struct dirent 			mdirent;
typedef DIR					 	MDIR;
#endif

ini_fd_t ini_file = NULL;
const char iniPath[MAXPATH]=INIPATH;
//FIXME IMPORTANT
int DefaultLang()
{
	return LANG_DEFAULT;
}

rgb* Palette = NULL;
bool_t GetHandedness()
{
	return 0;
}
int GetOrientation()
{
	return 0;
}
void QueryDesktop(video* p)
{
	int32_t	 nBitPerPixel=32;

	memset(p,0,sizeof(video));
	//GetScreenXY((int32_t*)&p->Width,(int32_t*)&p->Height);
	p->Aspect = ASPECT_ONE;

	//GetBitPerPixel(&nBitPerPixel);

	p->Pixel.BitCount = nBitPerPixel;

	if(nBitPerPixel==32)
	{
		DefaultRGB(&p->Pixel,32,8,8,8,0,0,0);
	}
	else if(nBitPerPixel==24)
	{
		DefaultRGB(&p->Pixel,24,8,8,8,0,0,0);
	}
	else if(nBitPerPixel==16)
	{
		if (QueryPlatform(PLATFORM_CAPS) & CAPS_ONLY12BITRGB)
			DefaultRGB(&p->Pixel,16,4,4,4,1,2,1);
		else
			DefaultRGB(&p->Pixel,16,5,6,5,0,0,0);
	}
}

bool_t HaveDPad()
{
	
	return 1;
}


void PlatformDetect(platform* p)
{
	
}

void Platform_Init()
{
	//FIXME IMPORTANT
	//if (!Context()->CodePage)
	//	Context()->CodePage = CP_ACP; //CP_OEMCP;

	NodeRegisterClass(&Platform);
	Context()->Platform = NodeEnumObject(NULL,PLATFORM_ID);

	SleepTimeout(0,0); // restore backlight settings (if last time crashed)

	File_Init();
}
void Ini_Init()
{
	if (ini_file == NULL)
	{
		ini_file = ini_open(INIPATH,"w",0);
	}
}
void Ini_Done()
{
	if(ini_file)
	{
		ini_close(ini_file);
		ini_file = NULL;
	}
}
void Log_Done()
{
}

void Platform_Done()
{
	File_Done();
	NodeUnRegisterClass(PLATFORM_ID);
	SetDisplayPower(1,0);
	SetKeyboardBacklight(1);
}


void WaitDisable(bool_t v) {}
bool_t WaitBegin() { return 0; }
void WaitEnd() {}

bool_t GetKeyboardBacklight()
{

	return 0;
}

static int _SetKeyboardBacklight(bool_t State)
{

	return ERR_NOT_SUPPORTED;
}

int SetKeyboardBacklight(bool_t State)
{
	int Result = ERR_NONE;

	return Result;
}

bool_t GetDisplayPower()
{

	return 1;
}

int SetDisplayPower(bool_t State,bool_t Force)
{


	return ERR_NOT_SUPPORTED;
}



void SleepTimerReset()
{
}

void SleepTimeout(bool_t KeepProcess,bool_t KeepDisplay)
{

}

void _Assert(const char* Exp,const char* File,int Line)
{

}

void ShowMessage(const tchar_t* Title,const tchar_t* Msg,...)
{

}

void HotKeyToString(tchar_t* Out, size_t OutLen, int HotKey)
{

}
int GetTimeFreq()
{
	int Freq = 1000;
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::GetTimeFreq Freq %d"),Freq);
	return Freq;
}
int timeGetTime()
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::timeGetTime=%d"),((int32_t)(((int64_t)tv.tv_sec * 1000000 + tv.tv_usec)/1000)&0x7FFFFFFF));
	return ((int32_t)(((int64_t)tv.tv_sec * 1000000 + tv.tv_usec)/1000)&0x7FFFFFFF);
}
int GetTimeTick()
{
	return timeGetTime();
}

void GetTimeCycle(int* p)
{
	int n=1;
	int j;
	int i = timeGetTime();
	while ((j = timeGetTime())==i)
		++n;
	p[0] = j;
	p[1] = n;
}



void DebugMessage(const tchar_t* Msg, ...)
{
	va_list Args;
	tchar_t s[1024]={0};
	va_start(Args,Msg);
	vstprintf_s(s+tcslen(s),TSIZEOF(s)-tcslen(s), Msg, Args);
	va_end(Args);
	tcscat_s(s,TSIZEOF(s),T("\n"));

	LOGI(s);
}

void* BrushCreate(rgbval_t Color) { return (void*)Color; }
void BrushDelete(void* Handle) {}

void WinFill(void* DC,rect* Rect,rect* Exclude,void* Brush)
{

}

void WinUpdate()
{
}

void WinValidate(const rect* Rect)
{
}

void WinInvalidate(const rect* Rect, bool_t Local)
{
}

void ReleaseModule(void** Module)
{
	if (*Module)
	{
		dlclose(*Module);
		*Module = NULL;
	}
}

int64_t GetTimeDate()
{
	time_t tm;
	time(&tm);
	return tm * 1000000000;
}

bool_t SaveDocument(const tchar_t* Name, const tchar_t* Text,tchar_t* URL,int URLLen)
{
	//todo...
	return 0;
}

void FindFiles(const tchar_t* Path, const tchar_t* Mask,void(*Process)(const tchar_t*,void*),void* Param)
{
	MDIR  	*dir;
	mdirent *entry;
	struct stat statbuf;

	tchar_t FindPath[MAXPATH];
	tchar_t Directory[MAXPATH]={0};

	tcscpy_s(Directory,TSIZEOF(Directory),Path);

	dir =mopendir(Directory);

	DEBUG_MSG2(DEBUG_SYS,T("platform_android.c:FindFiles:Path %s,dir %d"),Path,dir);
	if(dir == NULL)
		return ;

	while((entry = mreaddir(dir))!=NULL)
	{
		tchar_t* pMask = tcsrchr(Mask,'.');
		tchar_t* pExt  = tcsrchr(entry->d_name,'.');
		if(pExt == NULL)
		{
			continue;
		}
		mstat(entry->d_name,&statbuf);

		if(tcsicmp(T("."),entry->d_name) == 0 || tcsicmp(T(".."),entry->d_name) == 0)
			continue;

		if(tcsicmp(pMask,pExt) == 0)
		{
			tcscpy_s(FindPath,TSIZEOF(FindPath),Path);
			tcscat_s(FindPath,TSIZEOF(FindPath),entry->d_name);
			DEBUG_MSG1(DEBUG_SYS,T("platform_android::FindFiles:FindPath %s"),FindPath);
			Process(FindPath,Param);
		}
	}

	mclosedir(dir);
}
void GetRootPath(tchar_t* Path)
{
#ifdef UNICODE
	wgetcwd(Path,MAXPATH);
#else
	getcwd(Path,MAXPATH);
#endif
	DEBUG_MSG1(DEBUG_SYS,T("platform_android.c:GetRootPath:Path %s"),Path);
	Path[3]=0;
}
//Fixme hhool
void GetModulePath(tchar_t* Path,const tchar_t* Module)
{
	GetRootPath(Path);

	tcscat_s(Path+tcslen(Path),MAXPATH,PACKAGEPATH);
	tcscat_s(Path+tcslen(Path),MAXPATH,T("/lib/"));
	
	if (Module)
	{
		tcscat_s(Path+tcslen(Path),MAXPATH,Module);
	}
}
//Fixme hhool
void GetResourcePath(tchar_t* Path)
{
	GetRootPath(Path);

	tcscat_s(Path+tcslen(Path),MAXPATH,PACKAGEPATH);
	tcscat_s(Path+tcslen(Path),MAXPATH,T("/"));
}
void GetDebugPath(tchar_t* Path, int PathLen, const tchar_t* FileName)
{
	stprintf_s(Path,PathLen,T("slot1:\\%s"),FileName);
}

void GetSystemPath(tchar_t* Path, int PathLen, const tchar_t* FileName)
{
}

double compability_with_old_aac(int i) { return i; } // needed for defining __floatsidf()


#endif
