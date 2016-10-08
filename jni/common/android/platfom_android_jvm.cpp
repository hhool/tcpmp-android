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
#include <jni.h>
#include <pthread.h>
#include <wchar.h>


//android system
static JavaVM*		gs_jvm=NULL;

#define MAX_THREAD 16

static block 	gs_ThreadEnvArray;
static int 		gs_ThreadEnvCount=0;
static pthread_mutex_t gs_sNotifyMutex =PTHREAD_MUTEX_INITIALIZER;

ThreadEnv* GetThreadEnv(int ThreadID);

int   AllocEnvArray()
{
	int size = sizeof(ThreadEnv)*MAX_THREAD;
	size = ((size+SAFETAIL-1)/SAFETAIL)*SAFETAIL;
	if (!AllocBlock(size,&gs_ThreadEnvArray,0,HEAP_ANY))
		return ERR_OUT_OF_MEMORY;
	return ERR_NONE;
}
void FreeEnvArray()
{
	FreeBlock(&gs_ThreadEnvArray);
	gs_ThreadEnvCount = 0;
}
void  SetJVM(void *jvm)
{
	gs_jvm=(JavaVM*)jvm;
}
void* GetJVM()
{
	return (void*)gs_jvm;
}
ThreadEnv* ThreadEnvArray(int n)
{
	return ((ThreadEnv*)gs_ThreadEnvArray.Ptr)+n;
}
int GetThreadEnvCount()
{
	return gs_ThreadEnvCount;
}
int SaveThreadEnv()
{
	if(GetThreadEnv(ThreadId()) == NULL)
	{
		pthread_mutex_lock(&gs_sNotifyMutex);

		ThreadEnvArray(gs_ThreadEnvCount)->ThreadID=ThreadId();

		gs_jvm->AttachCurrentThread((JNIEnv**)&(ThreadEnvArray(gs_ThreadEnvCount)->Env),NULL);

		gs_ThreadEnvCount++;

		DEBUG_MSG4(DEBUG_SYS,T("PlayerNotify::SaveThreadEnv:ThreadEnvArray(Index = %d)->Env 0x%x,ThreadId 0x%x,gs_ThreadEnvCount %d"),\
				gs_ThreadEnvCount-1,ThreadEnvArray(gs_ThreadEnvCount-1)->Env,ThreadEnvArray(gs_ThreadEnvCount-1)->ThreadID,gs_ThreadEnvCount);

		DEBUG_MSG1(DEBUG_SYS,T("PlayerNotify::SaveThreadEnv:AttachCurrentThread gs_ThreadEnvCount %d"),gs_ThreadEnvCount);

		pthread_mutex_unlock(&gs_sNotifyMutex);
	}

	return gs_ThreadEnvCount-1;
}
int RemoveThreadEnv()
{
	int nIndex= 0;
	int ThreadID;
	ThreadEnv* threadEnv=NULL;
	bool_t bFound = 0;
	pthread_mutex_lock(&gs_sNotifyMutex);

	ThreadID = ThreadId();

	DEBUG_MSG2(DEBUG_SYS,T("PlayerNotify:RemoveThreadEnv Request gs_ThreadEnvCount 0x%x,ThreadID 0x%x"),gs_ThreadEnvCount,ThreadID);

	for(nIndex;nIndex<gs_ThreadEnvCount;nIndex++)
	{
		DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify:RemoveThreadEnv ThreadEnvArray(nIndex %d)->Env 0x%x , ThreadID 0x%x"),\
				nIndex,ThreadEnvArray(nIndex)->Env,ThreadEnvArray(nIndex)->ThreadID);

		if(ThreadEnvArray(nIndex)->ThreadID==ThreadID)
		{
			DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify:RemoveThreadEnv ThreadEnvArray FOUND(nIndex %d)->Env 0x%x , ThreadID 0x%x"),\
							nIndex,ThreadEnvArray(nIndex)->Env,ThreadEnvArray(nIndex)->ThreadID);
			threadEnv = ThreadEnvArray(nIndex);

			DEBUG_MSG1(DEBUG_SYS,T("PlayerNotify::RemoveThreadEnv:DetachCurrentThread ThreadID 0x%x"),ThreadID);

			gs_jvm->DetachCurrentThread();

			bFound = 1;


		}
		else if(threadEnv)
		{
			threadEnv->ThreadID = ThreadEnvArray(nIndex)->ThreadID;
			threadEnv->Env      = ThreadEnvArray(nIndex)->Env;
			threadEnv 			= ThreadEnvArray(nIndex);
		}
	}

	if(bFound)
	{
		gs_ThreadEnvCount--;
		DEBUG_MSG2(DEBUG_SYS,T("PlayerNotify::RemoveThreadEnv:if(bFound)  ThreadID 0x%x gs_ThreadEnvCount %x"),ThreadID,gs_ThreadEnvCount);
	}
	pthread_mutex_unlock(&gs_sNotifyMutex);

	return gs_ThreadEnvCount;

}
ThreadEnv* GetThreadEnv(int ThreadID)
{
	int nIndex=0;

	ThreadEnv* threadEnv=NULL;

	pthread_mutex_lock(&gs_sNotifyMutex);

	DEBUG_MSG2(DEBUG_SYS,T("PlayerNotify:GetThreadEnv Request gs_ThreadEnvCount 0x%x,ThreadID 0x%x"),gs_ThreadEnvCount,ThreadID);

	for(nIndex;nIndex<gs_ThreadEnvCount;nIndex++)
	{
		DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify:GetThreadEnv ThreadEnvArray(nIndex %d)->Env 0x%x , ThreadID 0x%x"),\
				nIndex,ThreadEnvArray(nIndex)->Env,ThreadEnvArray(nIndex)->ThreadID);

		if(ThreadEnvArray(nIndex)->ThreadID==ThreadID)
		{
			DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify:GetThreadEnv ThreadEnvArray FOUND(nIndex %d)->Env 0x%x , ThreadID 0x%x"),\
							nIndex,ThreadEnvArray(nIndex)->Env,ThreadEnvArray(nIndex)->ThreadID);
			threadEnv = ThreadEnvArray(nIndex);
		}
	}
	pthread_mutex_unlock(&gs_sNotifyMutex);

	return threadEnv;
}


/*
static char* jstringTostring(JNIEnv* env, jstring jstr)
{
       char* rtn = NULL;
       jclass clsstring = env->FindClass("java/lang/String");
       jstring strencode = env->NewStringUTF("utf-8");
       jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
       jbyteArray barr= (jbyteArray)env->CallObjectMethod(jstr, mid, strencode);
       jsize alen = env->GetArrayLength(barr);
       jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
       if (alen > 0)
       {
		 rtn = (char*)malloc(alen + 1);
		 memcpy(rtn, ba, alen);
		 rtn[alen] = 0;
       }
       env->ReleaseByteArrayElements(barr, ba, 0);
       return rtn;
}

//char* to jstring
static jstring chartoJstring(JNIEnv* env, const char* pat)
{
       jclass strClass = env->FindClass("Ljava/lang/String;");
       jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
       jbyteArray bytes = env->NewByteArray(strlen(pat));
       env->SetByteArrayRegion(bytes, 0, strlen(pat), (jbyte*)pat);
       jstring encoding = env->NewStringUTF("utf-8");
       return (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
}
*/
#define   CURRENT_ENCODE   "GBK"
void*   CharTojstring(void*   Env,   char*   str)
{
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::CharTojstring %s"),str);
	JNIEnv* env 	= (JNIEnv*)Env;
	jstring  rtn   	=   0;
	jsize   len   	=   strlen(str);

	jclass   clsstring   =   env->FindClass("java/lang/String");

	//new   encode   string   default   "GBK"
	jstring   strencode =   env->NewStringUTF(CURRENT_ENCODE);
	jmethodID   mid   	=   env->GetMethodID(clsstring,   "<init>",   "([BLjava/lang/String;)V");
	jbyteArray   barr   =   env->NewByteArray(len);

	env->SetByteArrayRegion(barr,0,len,(jbyte*)str);

	//call   new   String(byte[]   b,String   encode)
	rtn   =   (jstring)env->NewObject(clsstring,mid,barr,strencode);

	env->DeleteLocalRef(clsstring);
	env->DeleteLocalRef(strencode);//FIX ReferenceTable overflow (max=512)
	env->DeleteLocalRef(barr);//FIX ReferenceTable overflow (max=512)

	return   (void*)rtn;
}

char*   jstringToChar(void*   Env,   void*   jStr)
{
	DEBUG_MSG(DEBUG_SYS,T("platform_android::jstringToChar"));
	JNIEnv* env = (JNIEnv*)Env;
	jstring jstr= (jstring)jStr;
	char*   rtn =   NULL;
	jclass   clsstring  =   env->FindClass("java/lang/String");

	//new   encode   string   default   "GBK"
	jstring   strencode =   env->NewStringUTF(CURRENT_ENCODE);
	jmethodID   mid   	=   env->GetMethodID(clsstring,   "getBytes",   "(Ljava/lang/String;)[B");

	//call   String.getBytes   method   to   avoid   incompatible   migrating   into   solaris
	jbyteArray   barr	=   (jbyteArray)env->CallObjectMethod(jstr,mid,strencode);

	jsize   alen  =   env->GetArrayLength(barr);
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::jstringToChar %d"),alen);
	jbyte*   ba   =   env->GetByteArrayElements(barr,JNI_FALSE);

	if(alen   >   0)
	{
	  rtn   =   (char*)malloc(alen+1);         //new   char[alen+1];
	  memcpy(rtn,ba,alen);
	  rtn[alen]=0;
	}
	env->DeleteLocalRef(clsstring);
	env->DeleteLocalRef(strencode);//FIX ReferenceTable overflow (max=512)
	env->ReleaseByteArrayElements(barr,ba,0);

	return   rtn;
}
void UTF8ToTcs(tchar_t* Out,size_t OutLen,const char* In)
{
	ThreadEnv* threadEnv;
	int ThreadID =ThreadId();
	threadEnv = GetThreadEnv(ThreadID);
	jstring jstrin;
	tchar_t* temp=NULL;
	Out[0]=0;
	JNIEnv* env = NULL;

	if(!threadEnv||!threadEnv->Env)
	{
		DEBUG_MSG(DEBUG_SYS,T("platform_android::UTF8ToTcs:if(!threadEnv||!threadEnv->Env)"));
		return ;
	}
	DEBUG_MSG(DEBUG_SYS,T("platform_android::UTF8ToTcs:NewStringUTF before"));
	env = ((JNIEnv*)(threadEnv->Env));

	jstrin = env->NewStringUTF(In);
	if(env->ExceptionCheck())
	{
		DEBUG_MSG(DEBUG_SYS,T("platform_android::UTF8ToTcs:NewStringUTF:if(!ExceptionCheck())"));
		env->ExceptionDescribe();
		env->ExceptionClear();
		tcscat_s(Out,OutLen,In);
		return;
	}
	temp = jstringToChar(env,jstrin);
	if(env->ExceptionCheck())
	{
		DEBUG_MSG(DEBUG_SYS,T("platform_android::UTF8ToTcs:jstringToChar:if(!ExceptionCheck())"));
		env->ExceptionDescribe();
		env->ExceptionClear();
		tcscat_s(Out,OutLen,In);
	}
	if(temp)
	{
		tcscat_s(Out,OutLen,temp);
		free(temp);
	}
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::UTF8ToTcs:Out %s"),Out);
	env->DeleteLocalRef(jstrin);//FIX ReferenceTable overflow (max=512)
}

void TcsToUTF8(char* Out,size_t OutLen,const tchar_t* In)
{
	TcsToStr(Out,OutLen,In); //todo: fix
}

void WcsToTcs(tchar_t* Out,size_t OutLen,const uint16_t* In)
{
	ThreadEnv* threadEnv;
	int ThreadID =ThreadId();
	threadEnv = GetThreadEnv(ThreadID);
	jstring jstrin16;
	tchar_t* temp=NULL;
	const uint16_t* In_temp=In;
	JNIEnv* env = NULL;

	Out[0]=0;
	int InLen = 0;

	if(!threadEnv||!threadEnv->Env)
	{
		DEBUG_MSG(DEBUG_SYS,T("platform_android::WcsToTcs:if(!threadEnv||!threadEnv->Env)"));
	}
	env = ((JNIEnv*)(threadEnv->Env));
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::WcsToTcs:wcslen((const wchar_t*)In) %d"),wcslen((const wchar_t*)In));

	//cal the In value lenth;
	for (;*In_temp;++In_temp)
	{
		InLen++;
	}
	if(InLen<=0)
		return ;
	//generate unicode string object
	DEBUG_MSG1(DEBUG_SYS,T("platform_android::WcsToTcs:InLen %d"),InLen);
	jstrin16 = env->NewString(In,InLen);
	if(env->ExceptionCheck())
	{
		DEBUG_MSG(DEBUG_SYS,T("platform_android::WcsToTcs::NewString:if(!ExceptionCheck())"));
		env->ExceptionDescribe();
		env->ExceptionClear();
		return;
	}
	//converter to Char
	temp = jstringToChar(env,jstrin16);
	if(env->ExceptionCheck())
	{
		DEBUG_MSG(DEBUG_SYS,T("platform_android::WcsToTcs::jstringToChar(utf8_temp):if(!ExceptionCheck())"));
		env->ExceptionDescribe();
		env->ExceptionClear();
		env->DeleteLocalRef(jstrin16);//FIX ReferenceTable overflow (max=512)
		return;
	}
	if(temp)
	{
		DEBUG_MSG1(DEBUG_SYS,T("platform_android::WcsToTcs:temp %s"),temp);
		tcscat_s(Out,OutLen,temp);
		free(temp);
	}

	env->DeleteLocalRef(jstrin16);//FIX ReferenceTable overflow (max=512)
}
#endif
