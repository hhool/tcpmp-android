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
 * $Id: str_win32.c 585 2006-01-16 09:48:55Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"
#include "../gzip.h"

#define MAXTEXT		128*1024

#if defined(TARGET_ANDROID)

void GetModulePath(tchar_t* Path,const tchar_t* Module);
void GetResourcePath(tchar_t*Path);
void FindFiles(const tchar_t* Path, const tchar_t* Mask,void(*Process)(const tchar_t*,void*),void* Param);
void StringAlloc();
void StringFree();

#if !defined(NO_LANG) || defined(NO_PLUGINS)
static void LoadTGZ(stream* p,void* Buffer)
{
	gzreader Reader;
	if (GZInit(&Reader,p))
	{
		int Size;
		tchar_t StrSize[16];
		tarhead Head;

		while (GZRead(&Reader,&Head,sizeof(Head))==sizeof(Head) && Head.size[0])
		{
			GetAsciiToken(StrSize,16,Head.size,sizeof(Head.size));
			if (stscanf(StrSize,T("%o"),&Size)!=1 || Size>MAXTEXT)
				break;
			assert(Size<MAXTEXT);
			StringAddText(Buffer,GZRead(&Reader,Buffer,Size));
			if (Size & 511)
				GZRead(&Reader,Buffer,512-(Size & 511));
		}

		GZDone(&Reader);
	}
}
#endif

#ifndef NO_LANG
static void LoadTXT(const tchar_t* FileName,void* Buffer)
{
	stream* p = StreamOpen(FileName,0);
	DEBUG_MSG1(DEBUG_SYS,T("str_android::LoadTXT:FileName %s"),FileName);
	if (p)
	{
		StringAddText(Buffer,p->Read(p,Buffer,MAXTEXT));
		StreamClose(p);
	}
}
#endif
#ifdef RES_IN_COMMOM
int IsHaveResLang(int Fourcc);
int GetResLang(int Fourcc,uint8_t* Buffer);
#endif
void String_Init()
{
	//FIXME IMPORTANT
	void* Buffer;

	StringAlloc();

	Buffer = malloc(MAXTEXT);
	if (Buffer)
	{
#ifdef NO_PLUGINS
		{
			int n;
			HANDLE Module = GetModuleHandle(NULL);
			HRSRC Rsrc = FindResource(Module,MAKEINTRESOURCE(2000),T("LANGTAR"));
			if (Rsrc)
			{
				int Size = SizeofResource(Module,Rsrc);
				HGLOBAL Global = LoadResource(Module,Rsrc);
				if (Global)
				{
					void* p = LockResource(Global);
					if (p)
					{
						stream* Stream = StreamOpenMem(p,Size);
						if (Stream)
						{
							LoadTGZ(Stream,Buffer);
							StreamCloseMem(Stream);
						}
					}
				}
			}
			for (n=2000;(Rsrc = FindResource(Module,MAKEINTRESOURCE(n),T("LANG")))!=NULL;++n)
			{
				int Size = SizeofResource(Module,Rsrc);
				HGLOBAL Global = LoadResource(Module,Rsrc);
				if (Global)
				{
					void* p = LockResource(Global);
					if (p)
						StringAddText(p,Size);
				}
			}
		}

#endif
#ifndef NO_LANG
		{
#ifdef RES_IN_COMMOM
			int Len = 0;
			DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::IsHaveResLang:LANG_STD"));
			if(IsHaveResLang(LANG_STD)>=0)
			{
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::HaveResLang:LANG_STD"));
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::GetResLang:LANG_STD"));
				Len = GetResLang(LANG_STD,Buffer);
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::GetResLang:LANG_STD Over"));
				StringAddText(Buffer,Len);
			}
			DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::IsHaveResLang:LANG_EN"));
			if(IsHaveResLang(LANG_EN)>=0)
			{
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::HaveResLang:LANG_EN"));
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::GetResLang:LANG_EN"));
				Len = GetResLang(LANG_EN,Buffer);
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::GetResLang:LANG_EN Over"));
				StringAddText(Buffer,Len);
			}
			DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::IsHaveResLang:LANG_CHS"));
			if(IsHaveResLang(LANG_CHS)>=0)
			{
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::HaveResLang:LANG_CHS"));
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::GetResLang:LANG_CHS"));
				Len = GetResLang(LANG_CHS,Buffer);
				DEBUG_MSG(DEBUG_SYS,T("str_android::String_Init::GetResLang:LANG_CHS Over"));
				StringAddText(Buffer,Len);
			}
#else
			tchar_t Path[MAXPATH];
			stream* p;

			GetResourcePath(Path);
			tcscat_s(Path,TSIZEOF(Path),T("language.tgz"));
			p = StreamOpen(Path,0);
			if (p)
			{
				LoadTGZ(p,Buffer);
				StreamClose(p);
			}

			GetResourcePath(Path);
			FindFiles(Path,T("*.txt"),LoadTXT,Buffer);
#endif
		}
#endif
		free(Buffer);
	}
}

void String_Done()
{
	StringFree();
}

#ifndef CP_UTF8
#define CP_UTF8 65001
#endif
void AsciiToTcs(tchar_t* Out,size_t OutLen,const char* In)
{
	StrToTcs(Out,OutLen,In);
}

void TcsToAscii(char* Out,size_t OutLen,const tchar_t* In)
{
	TcsToStr(Out,OutLen,In);
}
void UTF7ToTcs(tchar_t* Out,size_t OutLen,const char* In)
{

}
void TcsToUTF7(char* Out,size_t OutLen,const tchar_t* In)
{

}

void TcsToStrEx(char* Out,size_t OutLen,const tchar_t* In,int CodePage)
{
#ifdef UNICODE
	size_t size = tcslen(In);
	wcstombs(Out,In,size);

	for (;OutLen>1 && *In;++In&&size>0,--OutLen,++Out,size--)
		*Out = (char)(*In>255?'?':*In);

	*Out = 0;
#else
	tcscpy_s(Out,OutLen,In);
#endif
}

void StrToTcsEx(tchar_t* Out,size_t OutLen,const char* In,int CodePage)
{
#ifdef UNICODE
	mbstowcs(Out,In,strlen(In));

	for (;OutLen>1 && *In;++In,--OutLen,++Out)
		*Out = (uint8_t)*In;

	*Out = (uint8_t)0;
#else
	tcscpy_s(Out,OutLen,In);
#endif
}

int tcsicmp(const tchar_t* a,const tchar_t* b) 
{
	for (;*a && *b;++a,++b)
		if (toupper(*a) != toupper(*b))
			break;
	if (*a == *b)
		return 0;
	return (toupper(*a)>toupper(*b)) ? 1:-1;
}

int tcsnicmp(const tchar_t* a,const tchar_t* b,size_t n) 
{
	for (;n>0 && *a && *b;++a,++b,--n)
		if (toupper(*a) != toupper(*b))
			break;
	if (n<=0 || *a == *b)
		return 0;
	return (toupper(*a)>toupper(*b)) ? 1:-1;
}

int GetCodePage(const tchar_t* ContentType)
{
	return Context()->CodePage;
}

#endif
