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
 * $Id: node_win32.c 622 2006-01-31 19:02:53Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"

#if defined(TARGET_ANDROID)

#include <sys/cdefs.h>
#include <sys/types.h>
#include <machine/_types.h>
#include <unistd.h>
#include <dlfcn.h>
#include <dirent.h>
#include <sys/stat.h>
#include "../ini/libini.h"
#include "cpu-features.h"
//FIXME IMPORTANT
#ifndef _MODULE
#define _MODULE
typedef void* HMODULE;
#endif

#if defined(UNICODE)
#define TSB(a) a
#else
#define TSB(a) a
#endif

void GetResourcePath(tchar_t*Path);

extern ini_fd_t ini_file;
extern const char iniPath[MAXPATH];

void NOINLINE NodeBase(int Class, tchar_t* Base, int BaseLen)
{
	tcscpy_s(Base,BaseLen,T(""));
	if (Class)
	{
		tchar_t s[16];
		FourCCToString(s,TSIZEOF(s),Class);
		tcscat_s(Base,BaseLen,s);
	}
}

static bool_t LoadValue(ini_fd_t fd,int Id,void* Data,int Size,int Type)
{
	uint8_t Buffer[MAXDATA]={0};
	tchar_t Name[16];
	unsigned int RegType;
	unsigned int RegSize = sizeof(Buffer);

	if(fd == NULL)
		return 0;

	IntToString(Name,TSIZEOF(Name),Id,0);
	
	ini_locateKey(fd,Name);

	if (ini_queryKeyValue(fd, Name, Buffer, &RegSize,&RegType) == ERR_NONE)
	{
		switch (Type)
		{
		case TYPE_BOOL:
			if (RegType == INI_TYPE_INT || RegType == INI_TYPE_INT64
				||RegType == INI_TYPE_UINT || RegType == INI_TYPE_UINT64)
			{
				*(bool_t*)Data = *(unsigned int*)Buffer;
				return 1;
			}
			break;

		case TYPE_RGB:
			if (RegType == INI_TYPE_INT || RegType == INI_TYPE_INT64
				||RegType == INI_TYPE_UINT || RegType == INI_TYPE_UINT64)
			{
				*(rgbval_t*)Data = *(unsigned int*)Buffer;
				return 1;
			}
			break;

		case TYPE_TICK:
			if (RegType == INI_TYPE_INT || RegType == INI_TYPE_INT64
				||RegType == INI_TYPE_UINT || RegType == INI_TYPE_UINT64)
			{
				*(tick_t*)Data = *(unsigned int*)Buffer;
				return 1;
			}
			break;

		case TYPE_INT:
		case TYPE_HOTKEY:
			if (RegType == INI_TYPE_INT || RegType == INI_TYPE_INT64
				||RegType == INI_TYPE_UINT || RegType == INI_TYPE_UINT64)
			{
				*(int*)Data = *(unsigned int*)Buffer;
				return 1;
			}
			break;

		case TYPE_POINT:
		case TYPE_RECT:
		case TYPE_BINARY:
		case TYPE_FRACTION:
			if (RegType == INI_TYPE_BINARY && Size<=MAXDATA)
			{
				memcpy(Data,Buffer,Size);
				return 1;
			}
			break;

		case TYPE_STRING:
			if (RegType == INI_TYPE_STRING && Size<=MAXDATA)
			{
				memcpy(Data,Buffer,Size);
				return 1;
			}
			break;
		}
	}
	return 0;
}

static void SaveValue(ini_fd_t fd,int Id,const void* Data,int Size,int Type)
{
	tchar_t Name[16];
	if(fd == NULL)
		return;

	IntToString(Name,TSIZEOF(Name),Id,0);
	ini_locateKey(fd,Name);
	if (!Data)
		ini_deleteKey(fd);
	else
	{
		unsigned int RegType,RegSize,v;
		RegType = INI_TYPE_INT;
		RegSize = sizeof(unsigned int);

		switch (Type)
		{
		case TYPE_BOOL:
			v = *(bool_t*)Data;
			break;

		case TYPE_RGB:
			v = *(int32_t*)Data;
			break;

		case TYPE_TICK:
			v = *(tick_t*)Data;
			break;

		case TYPE_INT:
		case TYPE_HOTKEY:
			v = *(int*)Data;
			break;

		case TYPE_POINT:
		case TYPE_RECT:
		case TYPE_BINARY:
		case TYPE_FRACTION:
			RegType = INI_TYPE_BINARY;
			RegSize = Size;
			break;

		case TYPE_STRING:
			RegType = INI_TYPE_STRING;
			RegSize = (unsigned int)((tcslen((const tchar_t*)Data)+1)*sizeof(tchar_t));
			break;

		default:
			return;
		}

		if (RegType == INI_TYPE_INT)
			Data = &v;

		ini_setKeyValue(ini_file, Name, Data, &RegSize, RegType);
	}
}

bool_t NodeRegLoadValue(int Class, int Id, void* Data, int Size, int Type)
{
	bool_t Result = 0;
	tchar_t Base[256];
	bool_t bOpenInFuc = 0;

	if (ini_file == NULL)
	{
		ini_file = ini_open(iniPath,"w",0);
		bOpenInFuc = 1;
	}
	if (ini_file == NULL)
	{
		return Result;
	}
	NodeBase(Class,Base,TSIZEOF(Base));

	ini_locateHeading (ini_file, Base);

	Result = LoadValue(ini_file,Id,Data,Size,Type);

	if(bOpenInFuc&&ini_file)
	{
		ini_close(ini_file);
		ini_file = NULL;
	}
		
	return Result;
}

void NodeRegSaveValue(int Class, int Id, const void* Data, int Size, int Type)
{
	tchar_t Base[256];
	bool_t bOpenInFuc = 0;

	if (ini_file == NULL)
	{
		ini_file = ini_open(iniPath,"w",0);
		bOpenInFuc = 1;
	}

	if (ini_file == NULL)
	{
		return;
	}

	NodeBase(Class,Base,TSIZEOF(Base));

	ini_locateHeading (ini_file, Base);
	
	SaveValue(ini_file,Id,Data,Size,Type);

	if(bOpenInFuc&&ini_file)
	{
		ini_close(ini_file);
		ini_file = NULL;
	}
}

void NodeRegLoad(node* p)
{
	if (p)
	{
		uint8_t Buffer[MAXDATA];
		tchar_t Base[256];
		datadef Type;
		int No;
		bool_t bOpenInFuc = 0;

		NodeBase(p->Class,Base,TSIZEOF(Base));
		if (ini_file == NULL)
		{
			ini_file = ini_open(iniPath,"w",0);
			bOpenInFuc = 1;
		}
		if (ini_file == NULL)
		{
			return;
		}
		ini_locateHeading(ini_file,Base);

		for (No=0;NodeEnum(p,No,&Type)==ERR_NONE;++No)
			if ((Type.Flags & DF_SETUP) && !(Type.Flags & (DF_NOSAVE|DF_RDONLY)) &&
				LoadValue(ini_file,Type.No,Buffer,Type.Size,Type.Type))
				p->Set(p,Type.No,Buffer,Type.Size);

		if(bOpenInFuc&&ini_file)
		{
			ini_close(ini_file);
			ini_file = NULL;
		}
		
	}
}

void NodeRegSave(node* p)
{
	if (p)
	{
		uint8_t Buffer[MAXDATA];
		datadef Type;
		bool_t bOpenInFuc = 0;
		tchar_t Base[256];

		NodeBase(p->Class,Base,TSIZEOF(Base));
		if (ini_file == NULL)
		{
			ini_file = ini_open(iniPath,"w",0);
			bOpenInFuc = 1;
		}
		if (ini_file == NULL)
		{
			return;
		}
			
		ini_locateHeading(ini_file,Base);
		{
			int No;
			int* i;
			unsigned int RegSize;
			tchar_t Name[64];
			array List = {NULL};

			RegSize = TSIZEOF(Name);
			for (No=0;ini_enumKeyName(ini_file,No,Name,&RegSize)==ERR_NONE;++No)
			{
				int Id = StringToInt(Name,0);
				if (Id >= NODE_MAX_REGISTRY)
					ArrayAppend(&List,&Id,sizeof(Id),256);
				RegSize = TSIZEOF(Name);
			}
			
			for (No=0;NodeEnum(p,No,&Type)==ERR_NONE;++No)
				if ((Type.Flags & DF_SETUP) && !(Type.Flags & (DF_NOSAVE|DF_RDONLY)))
				{
					int Result = p->Get(p,Type.No,Buffer,Type.Size);
					if (Result == ERR_NONE || Result == ERR_NOT_SUPPORTED) // not supported settings should be still saved
					{
						for (i=ARRAYBEGIN(List,int);i!=ARRAYEND(List,int);++i)
							if (*i == Type.No)
								*i = 0;
						SaveValue(ini_file,Type.No,Buffer,Type.Size,Type.Type);
					}
				}

			for (i=ARRAYBEGIN(List,int);i!=ARRAYEND(List,int);++i)
				if (*i)
					SaveValue(ini_file,*i,NULL,0,TYPE_INT);

			ArrayClear(&List);
		}
		if(bOpenInFuc&&ini_file)
		{
			ini_close(ini_file);
			ini_file = NULL;
		}
	}
}

void NodeRegLoadGlobal() {}
void NodeRegSaveGlobal() {}

void GetModulePath(tchar_t* Path,const tchar_t* Module);
void FindFiles(const tchar_t* Path, const tchar_t* Mask,void(*Process)(const tchar_t*,void*),void* Param);

static HMODULE Load(const tchar_t* Path)
{
	HMODULE Module;
#ifdef UNICODE
	char  	OutPath[MAXPATH];
#endif

#ifdef UNICODE
	wcstombs (OutPath, Path, MAXPATH );
	Module = dlopen(OutPath,RTLD_LAZY);
#else
	Module = dlopen(Path,RTLD_LAZY);
#endif
	DEBUG_MSG2(DEBUG_SYS,T("Load %s,Module 0x%x"),Path,Module);
	if (!Module)
	{
#ifdef UNICODE
		wcstombs (OutPath, Path, MAXPATH );
		Module = dlopen(OutPath,RTLD_LAZY);
#else
		Module = dlopen(Path,RTLD_LAZY);
#endif
	}
	return Module;
}
static void PluginError(const tchar_t* FileName)
{
	tchar_t Name[MAXPATH];
	tchar_t Ext[MAXPATH];
	SplitURL(FileName,NULL,0,NULL,0,Name,TSIZEOF(Name),Ext,TSIZEOF(Ext));
	tcsupr(Name);
	ShowError(0,ERR_ID,ERR_NOT_COMPATIBLE,Name);
}

void* NodeLoadModule(const tchar_t* Path,int* Id,void** AnyFunc,void** Db)
{
	HMODULE Module;
	void* Func;
	Module = Load(Path);
	if(!Module)
	{
		PluginError(Path);
	}

	Func= dlsym(Module,TSB("DLLRegister"));

	if (Func)
	{
		int Result;
		if (AnyFunc)
			*AnyFunc = Func; // set before calling DLLRegister

		Result = ((int(*)(int))Func)(CONTEXT_VERSION);
		if (Result != ERR_NONE)
		{
			Func = NULL;
			if (AnyFunc)
				*AnyFunc = NULL;
			if (Result == ERR_NOT_COMPATIBLE)
				PluginError(Path);
		}
	}

	if (!Func)
	{
		if(Module)
		{
			DEBUG_MSG1(DEBUG_SYS,T("node_android.c::NodeLoadModule:dlclose %s"),Path);
			dlclose(Module);
			Module = NULL;
		}
	}
	return Module;
}

void NodeFreeModule(void* Module,void* Db)
{
	if (Module)
	{
		void* UnRegister= dlsym(Module,TSB("DLLUnRegister"));

		if (UnRegister)
			((void(*)())UnRegister)();

		dlclose(Module);
	}
}
/*static int CheckModuleName(const tchar_t* URL, const tchar_t* Section)
{
	tchar_t Name[MAXPATH];
	tchar_t* Tail;
	tchar_t* pName = Name;

	SplitURL(URL,NULL,0,NULL,0,Name,TSIZEOF(Name),NULL,0);
	Tail = tcschr(Name,'?');
	if (Tail) *Tail = 0;

	DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:CheckModuleName %s"),URL);

	while(pName)
	{
		const tchar_t* p = tcschr(pName,'_');

		DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:CheckModuleName while pName %s"),pName!=NULL?pName:T(""));

		DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:CheckModuleName while  tcschr %s"),p!=NULL?p:T(""));

		if(p)p++;

		if (p && tcsnicmp(Section,p,tcslen(Section))==0)
		{
			DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:CheckModuleName while  tcschr return %s"),p!=NULL?p:T(""));

			return p[1]; // return type char
		}

		pName = p;
	}
	DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:CheckModuleName bad "));

	return 0;
}*/

#define NEON 	T("neon")
#define VFP	 	T("vfp")
#define VFP3 	T("v3fp")
#define V7		T("v7")
#define V6		T("v6")
#define V5		T("v5")

static tchar_t* ModuleName(tchar_t * ModuleName)
{
	tchar_t* p = tcsstr(ModuleName,V5);
	if(p!=NULL)
	{
		memcpy(ModuleName,ModuleName,p-ModuleName);
		tcscpy_s(p-1,MAXPATH,p+tcslen(V5));
	}
	p = tcsstr(ModuleName,V6);
	if(p!=NULL)
	{
		memcpy(ModuleName,ModuleName,p-ModuleName);
		tcscpy_s(p-1,MAXPATH,p+tcslen(V6));
	}
	p = tcsstr(ModuleName,V7);
	if(p!=NULL)
	{
		memcpy(ModuleName,ModuleName,p-ModuleName);
		tcscpy_s(p-1,MAXPATH,p+tcslen(V7));
	}
	p = tcsstr(ModuleName,VFP3);
	if(p!=NULL)
	{
		memcpy(ModuleName,ModuleName,p-ModuleName);
		tcscpy_s(p-1,MAXPATH,p+tcslen(VFP3));
	}
	p = tcsstr(ModuleName,VFP);
	if(p!=NULL)
	{
		memcpy(ModuleName,ModuleName,p-ModuleName);
		tcscpy_s(p-1,MAXPATH,p+tcslen(VFP));
	}
	p = tcsstr(ModuleName,NEON);
	if(p!=NULL)
	{
		memcpy(ModuleName,ModuleName,p-ModuleName);
		tcscpy_s(p-1,MAXPATH,p+tcslen(NEON));
	}

	return ModuleName;
}
//Fixme hhool
static void AddModule(const tchar_t* Path,char* szHead)
{
	AndroidCpuFamily  CpuFamily=ANDROID_CPU_FAMILY_UNKNOWN;
	uint64_t features;
	tchar_t PathOptimization[MAXPATH]={0};
	tchar_t Base[MAXPATH];
	tchar_t Name[MAXPATH];
	tchar_t Ext[MAXPATH];
	int32_t Date;
	tchar_t *bExsit=0;

	DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:Begin Path %s"),Path);
	SplitURL(Path,Base,TSIZEOF(Base),Base,TSIZEOF(Base),Name,TSIZEOF(Name),Ext,TSIZEOF(Ext));
	CpuFamily = android_getCpuFamily();
	DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:android_getCpuFamily %d"),CpuFamily);
	features = android_getCpuFeatures();
	DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:android_getCpuFeatures %d"),(int)features);
	if (CpuFamily == ANDROID_CPU_FAMILY_ARM&&(features & ANDROID_CPU_ARM_FEATURE_ARMv7))
	{
		DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:IS armv7"));

		if((features & ANDROID_CPU_ARM_FEATURE_NEON))
		{
			tchar_t Optimization[MAXPATH]=T("");

			if(tcsstr(Path,V6))
				return ;

			DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:IS armv7 with feature ANDROID_CPU_ARM_FEATURE_NEON"));
			tcscpy_s(Optimization,MAXPATH,T(""));
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,V7);
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,NEON);

			bExsit = tcsstr(Name,Optimization);

			if(!bExsit)
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and This module Path is %s no Optimization"),Path);
				tcscpy_s(PathOptimization,MAXPATH,Base);
				tcscat_s(PathOptimization,MAXPATH,T("/"));
				tcscat_s(PathOptimization,MAXPATH,ModuleName(Name));
				tcscat_s(PathOptimization,MAXPATH,Optimization);
				tcscat_s(PathOptimization,MAXPATH,T("."));
				tcscat_s(PathOptimization,MAXPATH,Ext);
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module PathOptimization Maybe is %s"),PathOptimization);
				Date = (int32_t)FileDate(PathOptimization);//fixme
				if(Date != -1)
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and The  module PathOptimization  %s Exits"),PathOptimization);
					//del file of path
					return ;
				}
				else
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module PathOptimization  %s Not Exits"),PathOptimization);
				}
			}
			else
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module Path Must be with  is %s"),Path);
			}
		}
		else if((features & ANDROID_CPU_ARM_FEATURE_VFPv3))
		{
			tchar_t Optimization[MAXPATH]=T("");

			if(tcsstr(Path,V6))
				return ;

			DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:IS armv7 with feature ANDROID_CPU_ARM_FEATURE_VFPv3"));

			tcscpy_s(Optimization,MAXPATH,T(""));
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,V6);
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,VFP3);

			bExsit = tcsstr(Name,Optimization);

			if(!bExsit)
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and This module Path is %s no Optimization"),Path);
				tcscpy_s(PathOptimization,MAXPATH,Base);
				tcscat_s(PathOptimization,MAXPATH,T("/"));
				tcscat_s(PathOptimization,MAXPATH,ModuleName(Name));
				tcscat_s(PathOptimization,MAXPATH,Optimization);
				tcscat_s(PathOptimization,MAXPATH,T("."));
				tcscat_s(PathOptimization,MAXPATH,Ext);
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module PathOptimization Maybe is %s"),PathOptimization);
				Date = (int32_t)FileDate(PathOptimization);//fixme
				if(Date != -1)
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and The  module PathOptimization  %s Exits"),PathOptimization);
					//del file of path
					return ;
				}
				else
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module PathOptimization  %s Not Exits"),PathOptimization);
				}
			}
			else
			{
				//exsit vfp3 and neon is also for armv7,return when neon exsit
				memset(PathOptimization,0,MAXPATH);
				tcscpy_s(PathOptimization,MAXPATH,Base);
				tcscat_s(PathOptimization,MAXPATH,T("/"));
				tcscat_s(PathOptimization,MAXPATH,ModuleName(Name));

				tcscat_s(PathOptimization,MAXPATH,T("_"));
				tcscat_s(PathOptimization,MAXPATH,V7);

				tcscat_s(PathOptimization,MAXPATH,T("_"));
				tcscat_s(PathOptimization,MAXPATH,NEON);
				tcscat_s(PathOptimization,MAXPATH,T("."));
				tcscat_s(PathOptimization,MAXPATH,Ext);
				Date = (int32_t)FileDate(PathOptimization);//fixme
				if(Date != -1)
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and The  module PathOptimization  %s Exits"),PathOptimization);
					//del file of path
					return ;
				}
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module Path Must be with  is %s"),Path);
			}
		}
		else if((features & ANDROID_CPU_ARM_FEATURE_VFP))
		{
			tchar_t Optimization[MAXPATH]=T("");

			DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:IS armv7 with feature ANDROID_CPU_ARM_FEATURE_VFP"));

			tcscpy_s(Optimization,MAXPATH,T(""));
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,V6);
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,VFP);

			bExsit = tcsstr(Name,Optimization);

			if(!bExsit)
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and This module Path is %s no Optimization"),Path);
				tcscpy_s(PathOptimization,MAXPATH,Base);
				tcscat_s(PathOptimization,MAXPATH,T("/"));
				tcscat_s(PathOptimization,MAXPATH,ModuleName(Name));
				tcscat_s(PathOptimization,MAXPATH,Optimization);
				tcscat_s(PathOptimization,MAXPATH,T("."));
				tcscat_s(PathOptimization,MAXPATH,Ext);
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module PathOptimization Maybe is %s"),PathOptimization);
				Date = (int32_t)FileDate(PathOptimization);//fixme
				if(Date != -1)
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and The  module PathOptimization  %s Exits"),PathOptimization);
					//del file of path
					return ;
				}
				else
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module PathOptimization  %s Not Exits"),PathOptimization);
				}
			}
			else
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module Path Must be with  is %s"),Path);
			}
		}
		else
		{
			//fixme
			DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module Path Must be with  is %s"),Path);
		}
	}
	else if (CpuFamily == ANDROID_CPU_FAMILY_ARM&&(features & ANDROID_CPU_ARM_FEATURE_ARMv6))
	{
		DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:IS armv6"));

		if(tcsstr(Path,V7))
			return ;

		if((features & ANDROID_CPU_ARM_FEATURE_VFP))
		{
			tchar_t Optimization[MAXPATH]=T("");
			tcscpy_s(Optimization,MAXPATH,T(""));
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,V6);
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,VFP);

			bExsit = tcsstr(Name,Optimization);

			if(!bExsit)
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv6 and This module Path is %s no Optimization"),Path);
				tcscpy_s(PathOptimization,MAXPATH,Base);
				tcscat_s(PathOptimization,MAXPATH,T("/"));
				tcscat_s(PathOptimization,MAXPATH,ModuleName(Name));
				tcscat_s(PathOptimization,MAXPATH,Optimization);
				tcscat_s(PathOptimization,MAXPATH,T("."));
				tcscat_s(PathOptimization,MAXPATH,Ext);
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv6 and  module PathOptimization Maybe is %s"),PathOptimization);

				Date = (int32_t)FileDate(PathOptimization);//fixme
				if(Date != -1)
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv6 and The  module PathOptimization  %s Exits"),PathOptimization);
					//del file of path
					return ;
				}
				else
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv6 and  module PathOptimization  %s Not Exits"),PathOptimization);
				}
			}
			else
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv6 and  module Path Must be  Optimization with  is %s"),Path);
			}
		}
		else
		{
			//fixme
			DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv6 and  module Path Must be with  is %s"),Path);
		}
	}
	else if (CpuFamily == ANDROID_CPU_FAMILY_ARM&&(features & ANDROID_CPU_ARM_FEATURE_ARMv5TE))
	{
		if(tcsstr(Path,V7)||tcsstr(Path,V6))
			return ;

		if((features & ANDROID_CPU_ARM_FEATURE_VFP))
		{
			tchar_t Optimization[MAXPATH]=T("");
			DEBUG_MSG(DEBUG_TEST,T("node_android::AddModule:IS arm5"));

			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,V5);
			tcscat_s(Optimization,MAXPATH,T("_"));
			tcscat_s(Optimization,MAXPATH,VFP);

			bExsit = tcsstr(Name,Optimization);
			if(!bExsit)
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS arm5 and  module Path Maybe is %s"),PathOptimization);
				tcscpy_s(PathOptimization,MAXPATH,Base);
				tcscat_s(PathOptimization,MAXPATH,T("/"));
				tcscat_s(PathOptimization,MAXPATH,Name);
				tcscat_s(PathOptimization,MAXPATH,Optimization);
				tcscat_s(PathOptimization,MAXPATH,T("."));
				tcscat_s(PathOptimization,MAXPATH,Ext);
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS arm5 and This module Path is %s"),Path);
				Date = (int32_t)FileDate(PathOptimization);//fixme
				if(Date != -1)
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS arm5 and The  module Path  %s Exits"),PathOptimization);
					//del file of path
					return ;
				}
				else
				{
					DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS arm5 and  module Path  %s Not Exits"),PathOptimization);
				}
			}
			else
			{
				DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS arm5 and  module Path Must be with  is %s"),Path);
			}
		}
		else
		{
			//fixme
			DEBUG_MSG1(DEBUG_TEST,T("node_android::AddModule:IS armv7 and  module Path Must be with  is %s"),Path);
		}
	}

	Date = (int32_t)FileDate(Path);
	DEBUG_MSG3(DEBUG_TEST,T("node_android::AddModule:Path ADD MODULE %s szHead %s Date %d"),Path,szHead,Date);
	if (Date != -1)
	{
		int32_t Value=0;
		unsigned int RegSize = sizeof(Value);
		bool_t Changed = 0;
		bool_t bOpenInFuc = 0;

		if (ini_file == NULL)
		{
			ini_file = ini_open(iniPath,"w",0);
			bOpenInFuc = 1;
		}
		
		if (ini_file == NULL)
		{
			return;
		}
		
		ini_locateHeading(ini_file,szHead);
		ini_locateKey (ini_file,Path);

		Changed = ini_queryKeyValue(ini_file, Path, (unsigned char *)&Value, (unsigned int*)&RegSize,NULL) != ERR_NONE || Value != Date;

		DEBUG_MSG5(DEBUG_SYS,T("node_android::AddModule:Path %s szHead %s Changed %d Value %d Date %d"),Path,szHead,Changed,Value,Date);
		NodeAddModule(Path,0,Date,Changed,0);

		if (Changed && ini_file) // save file stamp to registry
		{
			ini_locateHeading(ini_file,szHead);
			ini_locateKey (ini_file,Path);
			ini_writeInt(ini_file,Date);
		}
		if(bOpenInFuc&&ini_file)
		{
			ini_close(ini_file);
			ini_file = NULL;
		}
	}
}
//Fixme hhool
void ParesIni(const char* p, int n,array* List)
{
	tchar_t PathModule[MAXPATH];
	tchar_t* s = (tchar_t*)malloc(MAXLINE*sizeof(tchar_t));
	char* s8 = (char*)malloc(MAXLINE*sizeof(char));
	tchar_t* q;
	int i;

	while (n>0)
	{
		for (;n>0 && (*p==9 || *p==32);++p,--n);
		for (i=0;n>0 && *p!=13 && *p!=10;++p,--n)
			if (i<MAXLINE-1)
				s8[i++] = *p;
		for (;n>0 && (*p==13 || *p==10);++p,--n)
		s8[i]=0;

		AsciiToTcs(s,MAXLINE,s8);

		for (i=0;IsSpace(s[i]);++i);
			if (s[i]==0) continue;

		if (s[i]=='[')
		{
			++i;
			q = tcschr(s+i,']');
			if (!q) break; // invalid language file
			*q = 0;

			if(tcsncmp(s+i,T("MODULES"),tcslen(T("MODULES"))))
				break;
		}
		else
		{
			GetModulePath(PathModule,s);
			//tcscat_s(PathModule, MAXPATH, s);	//use relative path
			DEBUG_MSG1(DEBUG_SYS,T("ParesIni %s"),PathModule);
			ArrayAppend(List,PathModule,(tcslen(PathModule)+1)*sizeof(tchar_t),256);
			AddModule(PathModule,0);
		}
	}

	free(s);
	free(s8);
}

//static void AddPlugins(array* List)
//{
//	tchar_t FileName[MAXPATH];
//	stream* p=NULL;
//	// add plugins in exe directory
//	GetResourcePath(FileName);
//	tcscat_s(FileName,TSIZEOF(FileName),T("player.ini"));
//	p = StreamOpen(FileName,0);
//	if (p)
//	{
//		void* Buffer = malloc(65535);
//		memset(Buffer,0,65535);
//		ParesIni(Buffer,p->Read(p,Buffer,65535),List);
//		StreamClose(p);
//		free(Buffer);
//	}
//}
/*
static void AddPlugins(array* List)
{
	tchar_t Path[MAXPATH]={0};
	ini_fd_t ini_file;
	// add plugins in exe directory
	GetModulePath(Path,NULL);
	DEBUG_MSG1(DEBUG_SYS,T("node_android::AddPlugins:Path %s"),Path);
	ini_file = ini_open(g_szIniFile,"r+w",0);
	if(ini_file)
	{
		ini_locateHeading (ini_file, "DLLStamp");
	}
	FindFiles(Path,T("*.so"),AddModule,(void*)&ini_file);
}
*/
static void FindPlugins(array* List)
{
	tchar_t* s;
	tchar_t Name[64];
	tchar_t Path[MAXPATH];
	unsigned int RegSize;
	int No,Class;
	bool_t bOpenInFuc = 0; 
	
	char szHead[256] ={"DLLStamp"};
	if(ini_file == NULL)
	{
		ini_file = ini_open(iniPath,"w",0);
		bOpenInFuc = 1;
	}
	if(ini_file == NULL)
	{
		return;
	}

	ini_locateHeading (ini_file, "DLLStamp");
	
	// add plugins in exe directory
	GetModulePath(Path,NULL);
	FindFiles(Path,T("*.so"),AddModule,szHead);

	// delete unused stamps
	ArrayClear(List);
	RegSize = TSIZEOF(Path);
	for (No=0;ini_enumKeyName(ini_file,No,Path,&RegSize)==ERR_NONE;++No)
	{
		if (!NodeFindModule(Path,0))
			ArrayAppend(List,Path,(tcslen(Path)+1)*sizeof(tchar_t),256);
		RegSize = TSIZEOF(Path);
	}

	for (s=ARRAYBEGIN(*List,tchar_t);s!=ARRAYEND(*List,tchar_t);s+=tcslen(s)+1)
	{
		ini_locateKey(ini_file,s);
		ini_deleteKey(ini_file);
	}

	ArrayClear(List);
		
	RegSize = TSIZEOF(Name);
	for (No=0;ini_enumHead(ini_file,No,Name,&RegSize)==ERR_NONE;++No)
	{
		if (tcslen(Name)==4)
		{
			Class = StringToFourCC(Name,0);
			ArrayAppend(List,&Class,sizeof(Class),128);
		}
		RegSize = TSIZEOF(Name);
	}
	if(bOpenInFuc&&ini_file)
	{
		ini_close(ini_file);
		ini_file = NULL;
	}
	
}

static void LoadValueString(ini_fd_t fd,int Class,int Id)
{
	uint8_t Buffer[MAXDATA];
	if (LoadValue(fd,Id,Buffer,sizeof(Buffer),TYPE_STRING))
		StringAdd(1,Class,Id,(tchar_t*)Buffer);
}

void Plugins_Init()
{
	tchar_t Path[MAXPATH];
	array List = {NULL};
	nodedef Def;
	int* i;
	bool_t bOpenInFuc = 0;

	DEBUG_MSG(DEBUG_SYS,T("Plugins_Init"));
	FindPlugins(&List);

	if(!ini_file)
	{
		ini_file = ini_open(iniPath,"w",0);
		bOpenInFuc = 1;
	}
	
	for (i=ARRAYBEGIN(List,int);i!=ARRAYEND(List,int);++i)
	{
		int Class = *i;
		tchar_t Base[256];

		NodeBase(Class,Base,TSIZEOF(Base));
		if(ini_file)
		{
			ini_locateHeading (ini_file, Base);
		}

		if (LoadValue(ini_file,NODE_MODULE_PATH,Path,sizeof(Path),TYPE_STRING) && NodeFindModule(Path,0))
		{
			memset(&Def,0,sizeof(Def));
			Def.Class = Class;
			Def.Flags = 0;
			Def.ParentClass = 0;
			Def.Priority = PRI_DEFAULT;
			LoadValue(ini_file,NODE_PARENT,&Def.ParentClass,sizeof(int),TYPE_INT);
			LoadValue(ini_file,NODE_PRIORITY,&Def.Priority,sizeof(int),TYPE_INT);
			LoadValue(ini_file,NODE_FLAGS,&Def.Flags,sizeof(int),TYPE_INT);
			LoadValueString(ini_file,Class,NODE_NAME);
			LoadValueString(ini_file,Class,NODE_CONTENTTYPE);
			LoadValueString(ini_file,Class,NODE_EXTS);
			LoadValueString(ini_file,Class,NODE_PROBE);
			NodeLoadClass(&Def,Path,0);

		}
		
	}
	if(bOpenInFuc&&ini_file)
	{
		ini_close(ini_file);
		ini_file = NULL;
	}

	ArrayClear(&List);
}

void Plugins_Done()
{
}

#endif
