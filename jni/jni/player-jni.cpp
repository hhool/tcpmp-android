#include <jni.h>
#include <android/log.h>
#include "../common/common.h"
#include "environment.h"
#include "vsurface_wrap.h"
#include "mediainfo.h"
#include <pthread.h>

#define NAME1(CLZ, FUN) Java_##CLZ##_PlayerCore_##FUN
#define NAME2(CLZ, FUN) NAME1(CLZ, FUN)
#define FUNCTION(FUN) NAME2(PACKAGE_NAME, FUN)

extern "C"{
void OverlaySuface_Init();
void OverlaySuface_Done();

Environment g_Environment;

#define STATIC 

static jobject 		gs_object=NULL;
static int 			gs_MainThreadID = 0;
static tchar_t 		g_szExts[5120]={0};
static jmethodID  	gs_method_PlayerNotify=NULL;   	// 必须使用 global reference
static jmethodID  	gs_method_ErrorNotify=NULL;   	// 必须使用 global reference
static jmethodID	gs_method_SubNotify=NULL;
static bool_t 		gs_bInit = 0;
static void* 		gs_EventThumb = NULL;
static void* 		gs_EventSyncThumb = NULL;
static void*		gs_LockThumb = NULL;
static void*		gs_LockSyncThumb = NULL;
static bool_t		gs_ThumbOk = 0;
static bool_t		gs_bSyncOk = 0;
static jint PlayerCore_Stop(int NoFillBuf);

char g_szPackageDir[1024]=PACKAGE_DIR;


jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	JNIEnv *env = NULL;

	JavaVM*	jvm=NULL;

	jint result = JNI_ERR;

	if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK)
	{
		return result;
	}

	env->GetJavaVM(&jvm);

	SetJVM((void*)jvm);

	return JNI_VERSION_1_4;
}
void GenExts(tchar_t* Exts,int ExtsLen)
{
	int* i;
	array List;
	Exts[0]=0;
	NodeEnumClass(&List,MEDIA_CLASS);
	for (i=ARRAYBEGIN(List,int);i!=ARRAYEND(List,int);++i)
	{
		const tchar_t* s = LangStr(*i,NODE_EXTS);
		if (s[0])
		{
			if (Exts[0]) tcscat_s(Exts,ExtsLen,T(";"));
			tcscat_s(Exts,ExtsLen,s);
		}
	}
	ArrayClear(&List);
}
static int PlayerNotify(player* p,int Id,int Value)
{
	DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify gs_object 0x%x,Id 0x%x,Value %d"),gs_object,Id,Value);

	ThreadEnv* threadEnv;

	if(!(JavaVM*)GetJVM()||!gs_object||!gs_method_PlayerNotify||!gs_method_ErrorNotify)
		return ERR_NOT_SUPPORTED;

	threadEnv = GetThreadEnv(ThreadId());

	DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify::GetThreadEnv(ThreadID=0x%x)=0x%x.env 0x%x"),ThreadId(),threadEnv,threadEnv?threadEnv->Env:0);

	if(!threadEnv||!threadEnv->Env)
	{
		DEBUG_MSG2(DEBUG_SYS,T("PlayerNotify::if(!threadEnv||!threadEnv->Env) (JavaVM*)GetJVM() %x threadEnv->Env 0x%x"),(JavaVM*)GetJVM(),threadEnv->Env);
		return ERR_NOT_SUPPORTED;
	}

	DEBUG_MSG3(DEBUG_SYS,T("PlayerNotify::GetThreadEnv(ThreadID=0x%x)=0x%x.env 0x%x"),ThreadId(),threadEnv,threadEnv?threadEnv->Env:0);

	if(Id == PLAYER_PLAY)
	{
		DEBUG_MSG2(DEBUG_SYS,T("PlayerNotify::CallVoidMethod (JavaVM*)GetJVM() %x threadEnv->Env 0x%x"),(JavaVM*)GetJVM(),threadEnv->Env);

		if(Value)
		{
			((JNIEnv*)(threadEnv->Env))->CallVoidMethod(gs_object,gs_method_PlayerNotify, 1);
		}
		else
		{
			((JNIEnv*)(threadEnv->Env))->CallVoidMethod(gs_object,gs_method_PlayerNotify, 2);
		}
		DEBUG_MSG1(DEBUG_SYS,T("PlayerNotify::CallVoidMethod end (JavaVM*)GetJVM() %x"),(JavaVM*)GetJVM());
	}
	else if(Id == PLAYER_SYNCED)
	{
		((JNIEnv*)(threadEnv->Env))->CallVoidMethod(gs_object,gs_method_PlayerNotify, 3);
	}
	else if(Id == PLAYER_PLAY_COMPLETE)
	{
		//fixme
		PlayerCore_Stop(0);
		ResetSurfaceBuffer();

		((JNIEnv*)(threadEnv->Env))->CallVoidMethod(gs_object,gs_method_PlayerNotify, 4);
	}
	else if(Id == PLAYER_EXIT_AT_END)
	{
	}
	else if(Id == PLAYER_LOADMODE)
	{
	}
	else if(Id == PLAYER_FULLSCREEN)
	{
	}
	else if(Id == PLAYER_BENCHMARK)
	{

	}
	return ERR_NONE;
}

static int ErrorNotify(player* p,int Param,int Param2)
{
	DEBUG_MSG1(DEBUG_SYS,T("ErrorNotify gs_object %x"),gs_object);

	ThreadEnv* threadEnv;
	const tchar_t* Msg = (const tchar_t*)Param2;

	if(!(JavaVM*)GetJVM()||!gs_object||!gs_method_PlayerNotify||!gs_method_ErrorNotify)
		return ERR_NOT_SUPPORTED;

	Msg = (const tchar_t*)Param2;

	threadEnv = GetThreadEnv(ThreadId());

	DEBUG_MSG2(DEBUG_SYS,T("ErrorNotify CallVoidMethod(ThreadID=%d)=0x%x"),ThreadId(),threadEnv);

	if(!threadEnv||!threadEnv->Env)
	{
		DEBUG_MSG2(DEBUG_SYS,T("ErrorNotify::if(!threadEnv||!threadEnv->Env) (JavaVM*)GetJVM() %x threadEnv->Env 0x%x"),(JavaVM*)GetJVM(),threadEnv->Env);
		return ERR_NOT_SUPPORTED;
	}

	char szStr[MAXPATH];

	jstring str;

	TcsToAscii(szStr,MAXPATH,Msg);

	DEBUG_MSG(DEBUG_SYS,T("ErrorNotify::CharTojstring beging"));

	str = (jstring)CharTojstring(((JNIEnv*)(threadEnv->Env)),szStr);

	((JNIEnv*)(threadEnv->Env))->CallVoidMethod(gs_object,gs_method_ErrorNotify, str);

	((JNIEnv*)(threadEnv->Env))->DeleteLocalRef(str);

	DEBUG_MSG2(DEBUG_SYS,T("ErrorNotify CallVoidMethod End (ThreadID=%d)=0x%x"),ThreadId(),threadEnv);

	return ERR_NONE;
}
static int SubtitleNotify(node * p,int Param,int Param2)
{
	DEBUG_MSG2(DEBUG_VIDEO,T("SubtitleNotify Param 0x%x,Param2 0x%x"),Param,Param2);
	DEBUG_MSG1(DEBUG_SYS,T("SubtitleNotify gs_object %x"),gs_object);

	int ThreadID = ThreadId();;
	ThreadEnv* threadEnv;
	const tchar_t* Msg;

	if(!(JavaVM*)GetJVM()||!gs_object||!gs_method_PlayerNotify||!gs_method_ErrorNotify||!gs_method_SubNotify)
		return ERR_NOT_SUPPORTED;

	SubtitleInfo * pSubInfo = (SubtitleInfo*)Param;

	threadEnv = GetThreadEnv(ThreadID);

	DEBUG_MSG2(DEBUG_SYS,T("SubtitleNotify CallVoidMethod(ThreadID=%d)=0x%x"),ThreadID,threadEnv);

	threadEnv = GetThreadEnv(ThreadID);

	if(!threadEnv||!threadEnv->Env)
	{
		DEBUG_MSG2(DEBUG_SYS,T("SubtitleNotify::if(!threadEnv||!threadEnv->Env) (JavaVM*)GetJVM() %x threadEnv->Env 0x%x"),(JavaVM*)GetJVM(),threadEnv->Env);
		return ERR_NOT_SUPPORTED;
	}

	const tchar_t* szString = (const tchar_t*)pSubInfo->szString;

	char szStr[MAXPATH];

	jstring str;

	TcsToAscii(szStr,MAXPATH,szString);

	DEBUG_MSG1(DEBUG_SYS,T("SubtitleNotify::CharTojstring beging %s"),szStr);

	str = (jstring)CharTojstring(((JNIEnv*)(threadEnv->Env)),szStr);

	((JNIEnv*)(threadEnv->Env))->CallVoidMethod(gs_object,gs_method_SubNotify, str);

	((JNIEnv*)(threadEnv->Env))->DeleteLocalRef(str);


	return ERR_NONE;
}

static int GetLangFourcc(int LangId)
{
	if(LangId == ENUM_LANG_EN)
		return LANG_EN;
	if(LangId == ENUM_LANG_CHS)
		return LANG_CHS;
	return LANG_DEFAULT;
}
STATIC jint FUNCTION(ContextInit)(JNIEnv* env,jobject thiz,jobject jobjectEnv)
{
	player* Player = NULL;
	node *Platform = NULL;
	int Int ;
	notify Notify;

	jclass cls = env->GetObjectClass(jobjectEnv);

	DEBUG_MSG(DEBUG_VIDEO,"GetFieldID_m_iSdkVersion_BEFORE");
	jfieldID fid= env->GetFieldID(cls, "m_iSdkVersion", "I");
	DEBUG_MSG1(DEBUG_VIDEO,"GetFieldID_m_iSdkVersion_AFTER %d",fid);
	g_Environment.m_iSdkVersion = (int)env->GetIntField(jobjectEnv, fid);

	DEBUG_MSG1(DEBUG_VIDEO,"GetFieldID_m_LangId_AFTER SdkVersion %d",g_Environment.m_LangId);
	DEBUG_MSG(DEBUG_VIDEO,"GetFieldID_m_LangId_BEFORE");
	fid= env->GetFieldID(cls, "m_LangId", "I");
	DEBUG_MSG1(DEBUG_VIDEO,"GetFieldID_m_LangId_AFTER %d",fid);
	g_Environment.m_LangId = (int)env->GetIntField(jobjectEnv, fid);
	DEBUG_MSG1(DEBUG_VIDEO,"GetFieldID_m_LangId_AFTER SdkVersion %d",g_Environment.m_LangId);

	if(gs_bInit)
		return 0;
	gs_bInit = 1;
	//alloca for thread JVM
	if(AllocEnvArray())
		return 0;

	DEBUG_MSG(DEBUG_SYS,T("Context_Init"));
	Context_Init(T("jumplayer"),T("1.0"),0,T("1.0"),(void*)0);

	Platform = (node*) Context()->Platform;
	Int = GetLangFourcc(g_Environment.m_LangId);
	Platform->Set(Platform,PLATFORM_LANG,&Int,sizeof(int));

	String_Init();
	WaveOut_Init();
	OverlaySuface_Init();

	GenExts(g_szExts,5120);
	DEBUG_MSG1(DEBUG_SYS,T("GenExts %s"),g_szExts);

	// turn on repeat
	Player = (player*)Context()->Player;
	if(Player==NULL)
		return -1;

	Int = 1;
	Player->Set(Player,PLAYER_REPEAT,&Int,sizeof(Int));

	//Register Notify for JAVA
	gs_MainThreadID = ThreadId();

	Notify.This = Player;
	Notify.Func = (notifyfunc)PlayerNotify;
	if (Player)
		Player->Set(Player,PLAYER_NOTIFY,&Notify,sizeof(Notify));

	Context()->Error.This = Player;
	Context()->Error.Func = (notifyfunc)ErrorNotify;

	DEBUG_MSG1(DEBUG_SYS,T("mainThreadID 0x%x"),gs_MainThreadID);
#if defined(TARGET_ANDROID)
	SaveThreadEnv();
#endif
	
	DEBUG_MSG(DEBUG_SYS,T("Context_Wnd"));
	Context_Wnd((void*)1); //fake window handle

	gs_EventThumb 		= EventCreate(0,0);
	gs_EventSyncThumb 	= EventCreate(0,0);
	gs_LockThumb 		= LockCreate();
	gs_LockSyncThumb 	= LockCreate();
	return 0;
}
STATIC jint FUNCTION(ContextDone)(JNIEnv* env,jobject thiz)
{
	DEBUG_MSG(DEBUG_SYS,T("ContextDone"));
#if defined(TARGET_ANDROID)
	context* p = Context();
	if(!p)
		return -1;

	player* Player = (player*)p->Player;
	if(Player==NULL)
		return -1;
#endif
	if(!gs_bInit)
		return 0;

	DEBUG_MSG(DEBUG_SYS,T("ContextDone::Context_Wnd"));

	gs_bInit = 0;

	EventClose(gs_EventThumb);
	gs_EventThumb = NULL;

	EventClose(gs_EventSyncThumb);
	gs_EventSyncThumb = NULL;

	LockDelete(gs_LockThumb);
	gs_LockThumb = NULL;

	LockDelete(gs_LockSyncThumb);
	gs_LockSyncThumb = NULL;

	Context_Wnd(NULL);
	DEBUG_MSG(DEBUG_SYS,T("ContextDone::Context_Wnd end"));
	OverlaySuface_Done();
	DEBUG_MSG(DEBUG_SYS,T("ContextDone::WaveOut_Done"));
	WaveOut_Done();
	DEBUG_MSG(DEBUG_SYS,T("ContextDone::Context_Done"));
	Context_Done();
	DEBUG_MSG(DEBUG_SYS,T("ContextDone::FreeBlock"));
	//RemoveThreadEnv();
#if defined(TARGET_ANDROID)
	FreeEnvArray();
#endif
	DEBUG_MSG1(DEBUG_SYS,T("ContextDone::FreeBlock GetThreadEnvCount() %d"),GetThreadEnvCount());
	return 0;
}
STATIC jint FUNCTION(SetPlayerListener)(JNIEnv* env,jobject thiz,jobject host)
{
	DEBUG_MSG2(DEBUG_SYS,T("SetPlayerListener thiz 0x%x,host 0x%x"),thiz,host);

	if(host)
	{
		//delete gs_object first
		if(gs_object)
		{
			DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener:if(gs_object) env->DeleteGlobalRef(gs_object) %x"),gs_object);

			env->DeleteGlobalRef(gs_object);

			gs_object = NULL;

			gs_method_PlayerNotify = NULL;

			gs_method_ErrorNotify = NULL;

			gs_method_SubNotify = NULL;


			DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener:if(gs_object) end env->DeleteGlobalRef(gs_object) %x"),gs_object);
		}

		DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener:GetJavaVM (JavaVM*)GetJVM() %x"),(JavaVM*)GetJVM());

		jclass hostClass = env->GetObjectClass(host);

		gs_method_PlayerNotify = env->GetMethodID(hostClass,"PlayerNotify","(I)V");

		DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener: PlayerNotify %x"),PlayerNotify);

		gs_method_ErrorNotify = env->GetMethodID(hostClass,"ErrorNotify","(Ljava/lang/String;)V");

		DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener: method_ErrorNotify %x"),gs_method_ErrorNotify);

		gs_method_SubNotify = env->GetMethodID(hostClass,"SubtitleNotify","(Ljava/lang/String;)V");

		DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener: method_SubNotify %x"),gs_method_SubNotify);

		if(gs_method_PlayerNotify&&gs_method_ErrorNotify&&gs_method_SubNotify)
		{
			DEBUG_MSG(DEBUG_SYS,T("SetPlayerListener:NewGlobalRef"));

			gs_object=env->NewGlobalRef(host);

			DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener:NewGlobalRef gs_object %x"),gs_object);

		}
	}
	else
	{
		if(gs_object)
		{
			DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener:if(gs_object) env->DeleteGlobalRef(gs_object) %x"),gs_object);

			env->DeleteGlobalRef(gs_object);

			gs_object = NULL;

			gs_method_PlayerNotify = NULL;

			gs_method_ErrorNotify = NULL;

			gs_method_SubNotify = NULL;

			DEBUG_MSG1(DEBUG_SYS,T("SetPlayerListener:if(gs_object) end env->DeleteGlobalRef(gs_object) %x"),gs_object);
		}

	}
	return 0;
}

STATIC jint FUNCTION(OpenURL)(JNIEnv* env,jobject thiz,jstring jurl)
{
	rect Viewport = {0,0,0,0};
	int Int = 0;
	bool_t b = 0;
	jboolean iscopy;
	const char *URL;

	DEBUG_MSG(DEBUG_SYS,T("OpenURL"));

	DEBUG_MSG2(DEBUG_SYS,T("OpenURL Width %d,Height %d"),Viewport.Width,Viewport.Height);

	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return -1;

	Int = NULLAUDIO_ID;
	if(Player->Get(Player,PLAYER_AOUTPUTID,&Int,sizeof(Int))==ERR_NONE&&Int!=WAVEOUT_ID)
	{
		Int = WAVEOUT_ID;
		Player->Set(Player,PLAYER_AOUTPUTID,&Int,sizeof(Int));
	}
	Int = NULLVIDEO_ID;
	if(Player->Get(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int))==ERR_NONE&&Int!=ANDROID_SURFACE_ID)
	{
		Int = ANDROID_SURFACE_ID;
		Player->Set(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int));
	}
	// empty saved playlist
	Int = 0;
	Player->Set(Player,PLAYER_LIST_COUNT,&Int,sizeof(Int));

	b  = 1;
	Player->Set(Player,PLAYER_PLAYATOPEN_FULL,&b,sizeof(b));

	URL = env->GetStringUTFChars(jurl, &iscopy);

	DEBUG_MSG1(DEBUG_SYS,T("GetStringUTFChars %s"),URL);
	Player->Set(Player,PLAYER_LIST_URL+0,URL,sizeof(URL));

	env->ReleaseStringUTFChars(jurl, URL);

	Player->Set(Player,PLAYER_LIST_CURRIDX,&Int,sizeof(int));

	return 0;
}
STATIC jint FUNCTION(Play)(JNIEnv* env,jobject thiz)
{
	int Int;
	bool_t Bool;
	node* Format;

	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return -1;

	((player*)Player)->Paint(Player,NULL,0,0);

	if( Player->Get(Player,PLAYER_FORMAT,&Format,sizeof(Format)) == ERR_NONE && Format)
	{
		bool_t b = 1;
		Player->Set(Player,PLAYER_PLAY,&b,sizeof(b));
	}

	return 0;
}
STATIC jint FUNCTION(Pause)(JNIEnv* env,jobject thiz)
{
	bool_t State = 0;
	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return -1;

	DEBUG_MSG(DEBUG_SYS,T("Pause"));

	if (Player->Get(Player,PLAYER_PLAY,&State,sizeof(State))==ERR_NONE)
	{
		State = 0;
		Player->Set(Player,PLAYER_PLAY,&State,sizeof(State));
	}

	return 0;
}
static jint PlayerCore_Stop(int NoFillBuf)
{
	fraction f;
	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return -1;

	DEBUG_MSG(DEBUG_SYS,T("Stop"));

	Player->Set(Player,PLAYER_STOP,NULL,NoFillBuf);
	f.Num = 0;
	f.Den = 1;
	Player->Set(Player,PLAYER_PERCENT,&f,sizeof(f));
	return 0;
}
STATIC jint FUNCTION(Stop)(JNIEnv* env,jobject thiz,int NoFillBuf)
{
	PlayerCore_Stop(NoFillBuf);
	return 0;
}
STATIC void FUNCTION(SetVideoMode)(JNIEnv *env, jobject thiz, int VideoMode)
{
	fraction f;
	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return ;
	DEBUG_MSG1(DEBUG_SYS,T("SetVideoMode VideoMode %d"),VideoMode);
	if(VideoMode==0) //HALF
	{
		f.Num = 1;
		f.Den = 2;

	}
	else if(VideoMode == 1)// FIT
	{
		f.Num = 0;
		f.Den = 1;
	}
	else // STREATCH
	{
		f.Num = -3;
		f.Den = SCALE_ONE;
	}
	Player->Set(Player,PLAYER_FULL_ZOOM,&f,sizeof(f));
	Player->Set(Player,PLAYER_SKIN_ZOOM,&f,sizeof(f));
}
STATIC void FUNCTION(Rotate)(JNIEnv *env, jobject thiz, int type)
{
    return;
}
STATIC void FUNCTION(VideoSurfaceChanged)(JNIEnv* env,jobject thiz,jobject jsurface)
{
	int Int = 0;
	rect Viewport = {0,0,0,0};
	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return ;
	
	DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged"));

	node* iOverlay;
	iOverlay = NodeEnumObject(NULL,ANDROID_SURFACE_ID);

	DEBUG_MSG5(DEBUG_SYS,T("SetVideoSurface::NodeEnumObject(NULL,ANDROID_SURFACE_ID) 0x%x,Class %c%c%c%c"),\
					iOverlay,
					(signed char)(iOverlay->Class&0xff),
					(signed char)(iOverlay->Class>>8&0xff),
					(signed char)(iOverlay->Class>>16&0xff),
					(signed char)(iOverlay->Class>>24&0xff)
					);
	notify_draw NotifyDraw;
	NotifyDraw.Func = (notify_draw_func)NULL;
	NotifyDraw.This= iOverlay;
	DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::SubDrawNotify.Func 0x%x,SubDrawNotify.This 0x%x"),NotifyDraw.Func,NotifyDraw.This);
	iOverlay->Set(iOverlay,ANDROID_OUTPUT_SUB_PROCESS,&NotifyDraw,sizeof(NotifyDraw));
	DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::NotifyDraw.Func 0x%x,NotifyDraw.This 0x%x end"),NotifyDraw.Func,NotifyDraw.This);


	if(Player->Get(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int))==ERR_NONE&&Int != NULLVIDEO_ID)
	{
		Int = NULLVIDEO_ID;
		Player->Set(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int));
	}
	DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged:PLAYER_UPDATEVIDEO:NULLVIDEO_ID end then Detach"));
	Detach(env, thiz);
	DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged:PLAYER_UPDATEVIDEO:NULLVIDEO_ID end"));

	if(jsurface)
	{
		Attach(env, thiz, jsurface);

		DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged:LockSurface"));
		LockSurface();
		Viewport.Width = GetSurfaceWidth();
		Viewport.Height= GetSurfaceHeight();
		DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged:UnlockSurface"));
		UnlockSurface();

		Int = ANDROID_SURFACE_ID;
		Player->Set(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int));

		Player->Set(Player,PLAYER_SKIN_VIEWPORT,&Viewport,sizeof(rect));
		DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged:PLAYER_UPDATEVIDEO"));

		node* iOverlay;
		iOverlay = NodeEnumObject(NULL,ANDROID_SURFACE_ID);

		DEBUG_MSG5(DEBUG_SYS,T("SetVideoSurface::NodeEnumObject(NULL,ANDROID_SURFACE_ID) 0x%x,Class %c%c%c%c"),\
				iOverlay,
				(signed char)(iOverlay->Class&0xff),
				(signed char)(iOverlay->Class>>8&0xff),
				(signed char)(iOverlay->Class>>16&0xff),
				(signed char)(iOverlay->Class>>24&0xff)
				);
		notify_draw NotifyDraw;
		NotifyDraw.Func = (notify_draw_func)SubtitleNotify;
		NotifyDraw.This= iOverlay;
		DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::SubDrawNotify.Func 0x%x,SubDrawNotify.This 0x%x"),NotifyDraw.Func,NotifyDraw.This);
		iOverlay->Set(iOverlay,ANDROID_OUTPUT_SUB_PROCESS,&NotifyDraw,sizeof(NotifyDraw));
		DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::NotifyDraw.Func 0x%x,NotifyDraw.This 0x%x end"),NotifyDraw.Func,NotifyDraw.This);

		Player->Set(Player,PLAYER_UPDATEVIDEO,NULL,0);
		DEBUG_MSG(DEBUG_SYS,T("VideoSurfaceChanged:PLAYER_UPDATEVIDEO:end"));
	}
}
STATIC void FUNCTION(SetVideoSurface)(JNIEnv* env,jobject thiz,jobject jsurface)
{
	int Int = 0;
	rect Viewport = {0,0,0,0};
	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return ;

	DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface"));
	if(jsurface)
	{

		Attach(env, thiz, jsurface);

		DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface:LockSurface"));
		LockSurface();
		Viewport.Width = GetSurfaceWidth();
		Viewport.Height= GetSurfaceHeight();
		DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface:UnlockSurface"));
		UnlockSurface();

		Int = ANDROID_SURFACE_ID;
		Player->Set(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int));

		Player->Set(Player,PLAYER_SKIN_VIEWPORT,&Viewport,sizeof(rect));
		DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface:PLAYER_UPDATEVIDEO"));

		node* iOverlay;
		iOverlay = NodeEnumObject(NULL,ANDROID_SURFACE_ID);

		DEBUG_MSG5(DEBUG_SYS,T("SetVideoSurface::NodeEnumObject(NULL,ANDROID_SURFACE_ID) 0x%x,Class %c%c%c%c"),\
				iOverlay,
				(signed char)(iOverlay->Class&0xff),
				(signed char)(iOverlay->Class>>8&0xff),
				(signed char)(iOverlay->Class>>16&0xff),
				(signed char)(iOverlay->Class>>24&0xff)
				);
		notify_draw NotifyDraw;
		NotifyDraw.Func = (notify_draw_func)SubtitleNotify;
		NotifyDraw.This= iOverlay;
		DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::SubDrawNotify.Func 0x%x,SubDrawNotify.This 0x%x"),NotifyDraw.Func,NotifyDraw.This);
		iOverlay->Set(iOverlay,ANDROID_OUTPUT_SUB_PROCESS,&NotifyDraw,sizeof(NotifyDraw));
		DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::NotifyDraw.Func 0x%x,NotifyDraw.This 0x%x end"),NotifyDraw.Func,NotifyDraw.This);

		Player->Set(Player,PLAYER_UPDATEVIDEO,NULL,0);
		DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface:PLAYER_UPDATEVIDEO:end"));
	}
	else
	{
		node* iOverlay;
		iOverlay = NodeEnumObject(NULL,ANDROID_SURFACE_ID);

		DEBUG_MSG5(DEBUG_SYS,T("SetVideoSurface::NodeEnumObject(NULL,ANDROID_SURFACE_ID) 0x%x,Class %c%c%c%c"),\
						iOverlay,
						(signed char)(iOverlay->Class&0xff),
						(signed char)(iOverlay->Class>>8&0xff),
						(signed char)(iOverlay->Class>>16&0xff),
						(signed char)(iOverlay->Class>>24&0xff)
						);
		notify_draw NotifyDraw;
		NotifyDraw.Func = (notify_draw_func)NULL;
		NotifyDraw.This= iOverlay;
		DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::SubDrawNotify.Func 0x%x,SubDrawNotify.This 0x%x"),NotifyDraw.Func,NotifyDraw.This);
		iOverlay->Set(iOverlay,ANDROID_OUTPUT_SUB_PROCESS,&NotifyDraw,sizeof(NotifyDraw));
		DEBUG_MSG2(DEBUG_SYS,T("SetVideoSurface::NotifyDraw.Func 0x%x,NotifyDraw.This 0x%x end"),NotifyDraw.Func,NotifyDraw.This);


		if(Player->Get(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int))==ERR_NONE&&Int != NULLVIDEO_ID)
		{
			Int = NULLVIDEO_ID;
			Player->Set(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int));
		}
		DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface:PLAYER_UPDATEVIDEO:NULLVIDEO_ID end then Detach"));
		Detach(env, thiz);
		DEBUG_MSG(DEBUG_SYS,T("SetVideoSurface:PLAYER_UPDATEVIDEO:NULLVIDEO_ID end"));
	}
}
STATIC int FUNCTION(GetVideoWidth)(JNIEnv *env, jobject thiz)
{
	DEBUG_MSG(DEBUG_SYS,T("GetVideoWidth "));
    return 0;
}
static void PlayerCore_SeekTo(int msec)
{
	tick_t Tick;
	fraction Percent;
	bool_t InSeek = 1;
	player* Player = (player*)Context()->Player;

	if(Player==NULL)
		return ;

	if(Player->Get(Player,PLAYER_DURATION,&Tick,sizeof(Tick)) == ERR_NONE && Tick>=0)
	{
		Percent.Num = msec;
		Percent.Den = Scale(Tick,1000,TICKSPERSEC);
		DEBUG_MSG2(DEBUG_SYS,T("SeekTo %d Duration %d"),msec,Percent.Den);
		Player->Set(Player,PLAYER_INSEEK,&InSeek,sizeof(InSeek));
		Player->Set(Player,PLAYER_PERCENT,&Percent,sizeof(Percent));
		InSeek = 0;
		Player->Set(Player,PLAYER_INSEEK,&InSeek,sizeof(InSeek));
	}
}
STATIC int FUNCTION(SeekTo)(JNIEnv *env, jobject thiz, int msec)
{
	DEBUG_MSG1(DEBUG_SYS,T("SeekTo %d "),msec);

	player* Player = (player*)Context()->Player;

	if(Player==NULL || msec < 0)
		return -1;

	PlayerCore_SeekTo(msec);

    return 0;
}
STATIC jboolean FUNCTION(IsPlaying)(JNIEnv *env, jobject thiz)
{
    bool_t playing = 0;
    player* Player = (player*)Context()->Player;
    if(Player)
    {
    	Player->Get(Player,PLAYER_PLAY,&playing,sizeof(playing));
    }
    return playing;
}

STATIC int FUNCTION(GetVideoHeight)(JNIEnv *env, jobject thiz)
{

    return 0;
}

STATIC int FUNCTION(GetCurrentPosition)(JNIEnv *env, jobject thiz)
{
	int tms = 0;
	tick_t Tick;
	player* Player = (player*)Context()->Player;

	if(Player==NULL)
		return -1;

	if (Player->Get(Player,PLAYER_POSITION,&Tick,sizeof(tick_t)) == ERR_NONE)
	{
		tms = Scale(Tick,1000,TICKSPERSEC);

		DEBUG_MSG1(DEBUG_SYS,T("GetCurrentPosition tms %d "),tms);
	}
    return tms;
}

STATIC int FUNCTION(GetDuration)(JNIEnv *env, jobject thiz)
{
	tick_t Tick;
	player* Player = (player*)Context()->Player;

	if(Player==NULL)
		return -1;

	if(Player->Get(Player,PLAYER_DURATION,&Tick,sizeof(Tick)) == ERR_NONE && Tick>=0)
	{
		DEBUG_MSG1(DEBUG_SYS,T("GetDuration Scale(Tick,1000,TICKSPERSEC) %d "),Scale(Tick,1000,TICKSPERSEC));
		return Scale(Tick,1000,TICKSPERSEC);
	}
    return 0;
}
STATIC void FUNCTION(SetVolume)(JNIEnv *env, jobject thiz, int nVol)
{
    return;
}
STATIC int FUNCTION(GetVolume)(JNIEnv *env, jobject thiz)
{
    return 0;
}

STATIC jboolean FUNCTION(IsFileSupport)(JNIEnv* env,jobject thiz,jstring jurl)
{
	jboolean iscopy;
	const char *URL = env->GetStringUTFChars(jurl, &iscopy);

	DEBUG_MSG(DEBUG_SYS,T("IsFileSupport"));

	if(g_szExts[0]&&URL)
	{
		DEBUG_MSG1(DEBUG_SYS,T("%s"),g_szExts);
		if(CheckExts(URL,g_szExts))
		{
			env->ReleaseStringUTFChars(jurl, URL);
			return true;
		}
	}
	env->ReleaseStringUTFChars(jurl, URL);

	return false;
}
STATIC jstring  FUNCTION(GetSupportFileSuffix)(JNIEnv* env,jobject thiz)
{
	return env->NewStringUTF(g_szExts);
}
STATIC jint FUNCTION(GetAllStreamCount)(JNIEnv* env,jobject thiz)
{
	int Count=0,No=0;
	packetformat PacketFormat;
	player* Player = (player*)Context()->Player;

	if(Player==NULL)
		return -1;

	for (No=0;PlayerGetStream(Player,No,&PacketFormat,NULL,0,NULL);++No)
	{
		if (PacketFormat.Type>PACKET_NONE && PacketFormat.Type < PACKET_MAX)
		{
			Count++;
		}
	}
	return Count;
}
STATIC jboolean FUNCTION(HasStream)(JNIEnv* env,jobject thiz,int StreamType)
{
	jboolean bFound=0;
	int No=0;
	packetformat Format;
	player* Player = (player*)Context()->Player;

	if(Player==NULL)
		return -1;

	for (No=0;PlayerGetStream(Player,No,&Format,NULL,0,NULL);++No)
	{
		if (Format.Type>PACKET_NONE && Format.Type < PACKET_MAX)
		{
			if(Format.Type == StreamType)
			{
				bFound = 1;
			}
		}
	}
	return bFound;
}
STATIC jint FUNCTION(GetStreamCount)(JNIEnv* env,jobject thiz,int StreamType)
{
	int Count=0,No=0;
	packetformat Format;
	player* Player = (player*)Context()->Player;

	if(Player==NULL)
		return -1;

	for (No=0;PlayerGetStream(Player,No,&Format,NULL,0,NULL);++No)
	{
		if (Format.Type>PACKET_NONE && Format.Type < PACKET_MAX)
		{
			if(Format.Type == StreamType)
			{
				Count++;
			}
		}
	}
	return Count;
}

STATIC jobject FUNCTION(GetMediaInputInfo)(JNIEnv* env,jobject thiz)
{
	jclass 	Class;
	jobject Object;
	node* Input = NULL;
	node* Reader = NULL;
	node* VOutput = NULL;
	packetformat Format;
	int No;
    node* Player = Context()->Player;
    if(!Player)
    	return thiz;

    DEBUG_MSG(DEBUG_SYS,T("GetMediaInfo MediaInfo_Input_getClass"));
    Class = MediaInfo_Input_getClass(env);
    DEBUG_MSG(DEBUG_SYS,T("GetMediaInfo MediaInfo_getClass fields.constructor"));
    fields.constructor = env->GetMethodID(Class, "<init>", "()V");
    DEBUG_MSG1(DEBUG_SYS,T("GetMediaInfo NewObject fields.constructor %x"),fields.constructor);
    Object = env->NewObject(Class, fields.constructor);
    DEBUG_MSG2(DEBUG_SYS,T("GetMediaInfo Class %x,Object %x"),Class,Object);

    Player->Get(Player,PLAYER_FORMAT,&Reader,sizeof(Reader));
	Player->Get(Player,PLAYER_INPUT,&Input,sizeof(Input));
	Player->Get(Player,PLAYER_VOUTPUT,&VOutput,sizeof(VOutput));

	if (Input)
	{
		DEBUG_MSG(DEBUG_SYS,T("GetMediaInfo:MediaInfo_Input"));
		MediaInfo_Input(env,Class,Object,Input,0);
	}

	if (Reader)
	{
		pin Pin;
		DEBUG_MSG(DEBUG_SYS,T("GetMediaInfo:MediaInfo_Reader"));
		MediaInfo_Reader(env,Class,Object,Reader,MEDIAINFO_FORMAT);
	}

    DEBUG_MSG1(DEBUG_SYS,T("GetMediaInfo return Object %x"),Object);

	return Object;
}
STATIC jobject FUNCTION(GetMediaCommentInfo)(JNIEnv* env,jobject thiz)
{
	jclass 	Class;
	jobject Object;
	node* Reader = NULL;
	node* VOutput = NULL;
	node* Player = Context()->Player;
	if(!Player)
		return thiz;

	DEBUG_MSG(DEBUG_SYS,T("GetMediaCommentInfo MediaInfo_Comment_getClass"));
	Class = MediaInfo_Comment_getClass(env);
	DEBUG_MSG(DEBUG_SYS,T("GetMediaCommentInfo MediaInfo_getClass fields.constructor"));
	fields.constructor = env->GetMethodID(Class, "<init>", "()V");
	DEBUG_MSG1(DEBUG_SYS,T("GetMediaCommentInfo NewObject fields.constructor %x"),fields.constructor);
	Object = env->NewObject(Class, fields.constructor);
	DEBUG_MSG2(DEBUG_SYS,T("GetMediaCommentInfo Class %x,Object %x"),Class,Object);

	Player->Get(Player,PLAYER_FORMAT,&Reader,sizeof(Reader));

	if (Reader)
	{
		pin Pin;
		DEBUG_MSG(DEBUG_SYS,T("GetMediaCommentInfo:MediaInfo_Reader"));
		MediaInfo_Comment(env,Class,Object,(player*)Player,-1);
	}

	DEBUG_MSG1(DEBUG_SYS,T("GetMediaCommentInfo return Object %x"),Object);

	return Object;
}

STATIC jobjectArray  FUNCTION(GetMediaVideoInfo)(JNIEnv* env,jobject thiz)
{
	node* Player = Context()->Player;
	node* Reader = NULL;
	jclass objClass;
	jobjectArray VideoInfoArray;
	int	No;
	tchar_t s[256];
	packetformat Format;
	int arrayIndex = 0;

	char szClass[1024]={0};
	tcscpy_s(szClass,1024,g_szPackageDir);
	tcscat_s(szClass+tcslen(szClass),1024-tcslen(szClass),"/MediaVideoInfo");

	if(!Player)
		return VideoInfoArray;

	objClass = env->FindClass(szClass);
	VideoInfoArray= env->NewObjectArray((jsize)MAXSTREAM, objClass, 0);

	Player->Get(Player,PLAYER_FORMAT,&Reader,sizeof(Reader));

	if (Reader)
	{
		pin Pin;
		DEBUG_MSG(DEBUG_SYS,T("GetMediaVideoInfo"));
		for (No=0;Reader->Get(Reader,FORMAT_STREAM+No,&Pin,sizeof(Pin))==ERR_NONE;++No)
		{
			if (PlayerGetStream((player*)Player,No,&Format,NULL,0,NULL))

			{
				if (Format.Type != PACKET_VIDEO)
				{
					continue;

				}
				DEBUG_MSG(DEBUG_SYS,T("GetMediaVideoInfo MediaInfo_Comment_getClass"));
				jclass 	Class = MediaInfo_Video_getClass(env);
				DEBUG_MSG(DEBUG_SYS,T("GetMediaVideoInfo MediaInfo_getClass fields.constructor"));
				fields.constructor = env->GetMethodID(Class, "<init>", "()V");
				DEBUG_MSG1(DEBUG_SYS,T("GetMediaVideoInfo NewObject fields.constructor %x"),fields.constructor);
				jobject ObjectVideoInfo = env->NewObject(Class, fields.constructor);
				DEBUG_MSG2(DEBUG_SYS,T("GetMediaVideoInfo Class %x,ObjectVideoInfo %x"),Class,ObjectVideoInfo);

				env->SetIntField(ObjectVideoInfo,
								env->GetFieldID(Class,"mType","I"),
								Format.Type);

				env->SetIntField(ObjectVideoInfo,
								env->GetFieldID(Class,"mStreamID","I"),
								No);

				if (!PacketFormatName(&Format,s,TSIZEOF(s))) s[0] =0;
				{
					char szStr[MAXPATH];

					TcsToAscii(szStr,MAXPATH,s);

					jstring str = (jstring)CharTojstring(env,szStr);

					env->SetObjectField(ObjectVideoInfo,
										env->GetFieldID(Class,"mFormat","Ljava/lang/String;"),
										str);

					env->DeleteLocalRef(str);

					DEBUG_MSG1(DEBUG_SYS,T("GetMediaVideoInfo: mFormat %s"),s);
				}

				if (Pin.Node && Compressed(&Format.Format.Video.Pixel))
				{
					char szStr[MAXPATH];

					TcsToAscii(szStr,MAXPATH,LangStr(Pin.Node->Class,NODE_NAME));

					jstring str = (jstring)CharTojstring(env,szStr);

					env->SetObjectField(ObjectVideoInfo,
									env->GetFieldID(Class,"mCodec","Ljava/lang/String;"),
									str);

					env->DeleteLocalRef(str);
				}

				if (Format.Format.Video.Width && Format.Format.Video.Height)
				{
					env->SetIntField(ObjectVideoInfo,
									env->GetFieldID(Class,"mWidth","I"),
									Format.Format.Video.Width);
					env->SetIntField(ObjectVideoInfo,
									env->GetFieldID(Class,"mHeight","I"),
									Format.Format.Video.Height);
				}
				if (Format.PacketRate.Num)
				{
					FractionToString(s,TSIZEOF(s),&Format.PacketRate,0,3);
					env->SetIntField(ObjectVideoInfo,
									env->GetFieldID(Class,"mFps","I"),
									atoi(s));
				}
				DEBUG_MSG1(DEBUG_SYS,T("GetMediaVideoInfo: ByteRate d %d"),Format.ByteRate);

				if (Format.ByteRate)
				{
					IntToString(s,TSIZEOF(s),(Format.ByteRate+62)/125,0);// kbit/s
					env->SetIntField(ObjectVideoInfo,
									env->GetFieldID(Class,"mByteRate","I"),
									atoi(s));
					DEBUG_MSG1(DEBUG_SYS,T("GetMediaVideoInfo: ByteRate %s"),s);

				}

				env->SetObjectArrayElement(VideoInfoArray, arrayIndex++, ObjectVideoInfo);

				//Comment(p,(player*)Player,&y,No);
			}
		}
	}

	DEBUG_MSG2(DEBUG_SYS,T("GetMediaVideoInfo return VideoInfoArray %x,count %d"),VideoInfoArray,arrayIndex);

	return VideoInfoArray;
}

STATIC jobjectArray  FUNCTION(GetMediaAudioInfo)(JNIEnv* env,jobject thiz)
{
	node* Player = Context()->Player;
	node* Reader = NULL;
	int	No;
	tchar_t s[256];
	packetformat Format;
	int arrayIndex = 0;
	jclass objClass;
	jobjectArray AudioInfoArray;

	char szClass[1024]={0};
	tcscpy_s(szClass,1024,g_szPackageDir);
	tcscat_s(szClass+tcslen(szClass),1024-tcslen(szClass),"/MediaAudioInfo");

	if(!Player)
		return AudioInfoArray;

	objClass = env->FindClass(szClass);
	AudioInfoArray= env->NewObjectArray((jsize)MAXSTREAM, objClass, 0);

	Player->Get(Player,PLAYER_FORMAT,&Reader,sizeof(Reader));

	if (Reader)
	{
		pin Pin;
		DEBUG_MSG(DEBUG_SYS,T("GetMediaAudioInfo:MediaInfo_Reader"));
		for (No=0;Reader->Get(Reader,FORMAT_STREAM+No,&Pin,sizeof(Pin))==ERR_NONE;++No)
		{
			if (PlayerGetStream((player*)Player,No,&Format,NULL,0,NULL))

			{
				if (Format.Type != PACKET_AUDIO)
				{
					continue;
				}

				DEBUG_MSG(DEBUG_SYS,T("GetMediaAudioInfo MediaInfo_Comment_getClass"));
				jclass 	Class = MediaInfo_Audio_getClass(env);
				DEBUG_MSG(DEBUG_SYS,T("GetMediaAudioInfo MediaInfo_getClass fields.constructor"));
				fields.constructor = env->GetMethodID(Class, "<init>", "()V");
				DEBUG_MSG1(DEBUG_SYS,T("GetMediaAudioInfo NewObject fields.constructor %x"),fields.constructor);
				jobject ObjectAudioInfo = env->NewObject(Class, fields.constructor);
				DEBUG_MSG2(DEBUG_SYS,T("GetMediaAudioInfo Class %x,ObjectAudioInfo %x"),Class,ObjectAudioInfo);

				env->SetIntField(ObjectAudioInfo,
								env->GetFieldID(Class,"mType","I"),
								Format.Type);

				env->SetIntField(ObjectAudioInfo,
								env->GetFieldID(Class,"mStreamID","I"),
								No);

				if (!PacketFormatName(&Format,s,TSIZEOF(s))) s[0] =0;
				{
					char szStr[MAXPATH];

					TcsToAscii(szStr,MAXPATH,s);

					jstring str = (jstring)CharTojstring(env,szStr);

					env->SetObjectField(ObjectAudioInfo,
										env->GetFieldID(Class,"mFormat","Ljava/lang/String;"),
										str);

					env->DeleteLocalRef(str);

					DEBUG_MSG1(DEBUG_SYS,T("GetMediaAudioInfo: mFormat %s"),s);
				}


				if (Pin.Node && Format.Format.Audio.Format != AUDIOFMT_PCM)
				{
					char szStr[MAXPATH];

					TcsToAscii(szStr,MAXPATH,LangStr(Pin.Node->Class,NODE_NAME));

					jstring str = (jstring)CharTojstring(env,szStr);

					env->SetObjectField(ObjectAudioInfo,
										env->GetFieldID(Class,"mCodec","Ljava/lang/String;"),
										str);
					env->DeleteLocalRef(str);
				}

				s[0] = 0;
				if (Format.Format.Audio.SampleRate)
				{
					IntToString(s+tcslen(s),TSIZEOF(s)-tcslen(s),Format.Format.Audio.SampleRate,0);
					env->SetIntField(ObjectAudioInfo,
									env->GetFieldID(Class,"mSampleRate","I"),
									atoi(s));

				}
				switch (Format.Format.Audio.Channels)
				{
				case 0: break;
				case 1: tcscat_s(s,TSIZEOF(s),LangStr(MEDIAINFO_ID,MEDIAINFO_AUDIO_MONO)); break;
				case 2: tcscat_s(s,TSIZEOF(s),LangStr(MEDIAINFO_ID,MEDIAINFO_AUDIO_STEREO)); break;
				default: stcatprintf_s(s,TSIZEOF(s),T("%d Ch"),Format.Format.Audio.Channels); break;
				}
				if (s[0])
				{
					env->SetIntField(ObjectAudioInfo,
									env->GetFieldID(Class,"mChannels","I"),
									Format.Format.Audio.Channels);
				}

				if (Format.ByteRate)
				{
					IntToString(s,TSIZEOF(s),(Format.ByteRate+62)/125,0); //kbits/s
					env->SetIntField(ObjectAudioInfo,
									env->GetFieldID(Class,"mByteRate","I"),
									atoi(s));
				}

				env->SetObjectArrayElement(AudioInfoArray, arrayIndex++, ObjectAudioInfo);

				//Comment(p,(player*)Player,&y,No);
			}
		}
	}

	DEBUG_MSG2(DEBUG_SYS,T("GetMediaAudioInfo return AudioInfoArray %x Count %d"),AudioInfoArray,arrayIndex);

	return AudioInfoArray;
}
STATIC jobject FUNCTION(GetMediaRenderInfo)(JNIEnv* env,jobject thiz)
{
	jclass 	Class;
	jobject Object;
	node* VOutput = NULL;
	node* Reader = NULL;
	int	No;
	tchar_t s[256];
	packetformat Format;
	bool_t bFound = 0;

	node* Player = Context()->Player;
	if(!Player)
		return thiz;

	DEBUG_MSG(DEBUG_SYS,T("GetMediaRenderInfo MediaInfo_Comment_getClass"));
	Class = MediaInfo_Render_getClass(env);
	DEBUG_MSG(DEBUG_SYS,T("GetMediaRenderInfo MediaInfo_getClass fields.constructor"));
	fields.constructor = env->GetMethodID(Class, "<init>", "()V");
	DEBUG_MSG1(DEBUG_SYS,T("GetMediaRenderInfo NewObject fields.constructor %x"),fields.constructor);
	Object = env->NewObject(Class, fields.constructor);
	DEBUG_MSG2(DEBUG_SYS,T("GetMediaRenderInfo Class %x,Object %x"),Class,Object);

	Player->Get(Player,PLAYER_FORMAT,&Reader,sizeof(Reader));
	Player->Get(Player,PLAYER_VOUTPUT,&VOutput,sizeof(VOutput));

	if (Reader&&VOutput)
	{
		int Total = 0;
		int Dropped = 0;

		VOutput->Get(VOutput,OUT_TOTAL,&Total,sizeof(int));
		VOutput->Get(VOutput,OUT_DROPPED,&Dropped,sizeof(int));

		Total += Dropped;
		if (Total)
		{
			env->SetIntField(Object,
							env->GetFieldID(Class,"mTotal","I"),
							Total);

			env->SetIntField(Object,
							env->GetFieldID(Class,"mDropped","I"),
							Total);

			Player->Get(Player,PLAYER_VSTREAM,&No,sizeof(No));
			if (No>=0 && Reader->Get(Reader,(FORMAT_STREAM+No)|PIN_FORMAT,&Format,sizeof(Format))==ERR_NONE &&
				Format.Type == PACKET_VIDEO && Format.PacketRate.Num)
			{
				Simplify(&Format.PacketRate,(1<<30)/Total,(1<<30)/Total);
				Format.PacketRate.Num *= Total - Dropped;
				Format.PacketRate.Den *= Total;
				FractionToString(s,TSIZEOF(s),&Format.PacketRate,0,3);
				env->SetFloatField(Object,
								env->GetFieldID(Class,"mPlayFPS","F"),
								atof(s));
			}
		}
	}

	DEBUG_MSG1(DEBUG_SYS,T("GetMediaRenderInfo return Object %x"),Object);

	return Object;
}

void DoneThumbnail(void* This)
{
	DEBUG_MSG1(DEBUG_SYS,T("GetThumbnail  0x%x"),gs_EventThumb);

	LockEnter(gs_LockThumb);
	DEBUG_MSG1(DEBUG_SYS,T("GetThumbnail EventSet(gs_EventThumb) 0x%x"),gs_EventThumb);
	gs_ThumbOk = 1;

	EventSet(gs_EventThumb);
	LockLeave(gs_LockThumb);
}
static bool_t HasVideoStreamByName(const tchar_t* URL)
{
	tchar_t Ext[MAXPATH];
	tchar_t Ext_V[MAXPATH];
	SplitURL(URL,NULL,0,NULL,0,NULL,0,Ext,TSIZEOF(Ext));
	tcslwr(Ext);
	tcscpy_s(Ext_V,MAXPATH,Ext);
	tcscat_s(Ext_V,MAXPATH,":V");
	if(tcsstr(g_szExts,Ext_V)==0)
	{
		DEBUG_MSG(DEBUG_SYS,T("NO VIDEO"));
		return 0;
	}
	else
	{
		DEBUG_MSG(DEBUG_SYS,T("HAS VIDEO"));
		return 1;
	}
}
static int ThumbPlayerNotify(player* p,int Id,int Value)
{
	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::ThumbPlayerNotify"));
	if(Id == PLAYER_SYNCED)
	{
		LockEnter(gs_LockSyncThumb);
		DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::ThumbPlayerNotify PLAYER_SYNCED"));
		gs_bSyncOk = 1;
		EventSet(gs_EventSyncThumb);
		LockLeave(gs_LockSyncThumb);
	}
}
STATIC jbyteArray FUNCTION(GetThumbnail)(JNIEnv *env, jobject thiz,jstring filePath,jint i_width, jint i_height,jint RGBFlag,jint msec)
{
	jboolean isCopy;
	jbyteArray byteArray = NULL;
	rect Viewport = {0,0,0,0},OldViewport = {0,0,0,0};
	int Int = 0;
	bool_t b = 0;
	jboolean iscopy;
	const char *URL;
	notify* OldNotify,Notify;

	player* Player = (player*)Context()->Player;
	if(Player==NULL)
		return NULL;

	if(RGBFlag != 4 && RGBFlag !=2)
		return NULL;

	gs_ThumbOk = 0;
	gs_bSyncOk = 0;

#if defined(TARGET_ANDROID)
	SaveThreadEnv();
#endif
	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::GetStringUTFChars"));
	URL = env->GetStringUTFChars(filePath, &iscopy);

	DEBUG_MSG2(DEBUG_SYS,T("GetThumbnail Width %d,Height %d"),i_width,i_height);
	Int = NULLAUDIO_ID;
	Player->Set(Player,PLAYER_AOUTPUTID,&Int,sizeof(Int));

	node* iOverlayThumb;
	thumb_info  Thumb;
	iOverlayThumb = NodeEnumObject(NULL,OVERLY_THUMB_ID);
	DEBUG_MSG5(DEBUG_SYS,T("GetThumbnail::NodeEnumObject(NULL,OVERLY_THUMB_ID) 0x%x,Class %c%c%c%c"),\
			iOverlayThumb,
			(signed char)(iOverlayThumb->Class&0xff),
			(signed char)(iOverlayThumb->Class>>8&0xff),
			(signed char)(iOverlayThumb->Class>>16&0xff),
			(signed char)(iOverlayThumb->Class>>24&0xff));

	Thumb.Height 	=  i_height;
	Thumb.Width 	=  i_width;
	Thumb.Done		=  DoneThumbnail;
	Thumb.RGBNum	=  RGBFlag;
	Thumb.tick 		= -1;

	iOverlayThumb->Set(iOverlayThumb,OVERLAY_THUMB_INFO,&Thumb,sizeof(thumb_info));
	iOverlayThumb->Get(iOverlayThumb,OVERLAY_THUMB_INFO,&Thumb,sizeof(thumb_info));

	DEBUG_MSG4(DEBUG_SYS,T("GetThumbnail OVERLAY_THUMB_INFO Width %d,Height %d Pitch %d Buffer 0x%x"),\
			Thumb.Height,Thumb.Width,Thumb.Pitch,Thumb.Plane);

	fraction oldf,newf;
	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail VideoMode"));
	newf.Num = 0;
	newf.Den = 1;

	Player->Get(Player,PLAYER_FULL_ZOOM,&oldf,sizeof(oldf));
	Player->Set(Player,PLAYER_FULL_ZOOM,&newf,sizeof(newf));
	Player->Set(Player,PLAYER_SKIN_ZOOM,&newf,sizeof(newf));

	Int = OVERLY_THUMB_ID;
	Player->Set(Player,PLAYER_VOUTPUTID,&Int,sizeof(Int));

	Notify.This = Player;
	Notify.Func = (notifyfunc)ThumbPlayerNotify;
	Player->Get(Player,PLAYER_NOTIFY,&OldNotify,sizeof(Notify));
	Player->Set(Player,PLAYER_NOTIFY,&Notify,sizeof(Notify));

	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::PLAYER_SKIN_VIEWPORT"));
	Player->Get(Player,PLAYER_SKIN_VIEWPORT,&Viewport,sizeof(rect));
	DEBUG_MSG4(DEBUG_SYS,T("GetThumbnail::PLAYER_SKIN_VIEWPORT Get x %d,y %d,Width %d,Height %d"),Viewport.x,Viewport.y,Viewport.Width,Viewport.Height);
	if(Viewport.Height != i_height || Viewport.Width != i_width)
	{
		Viewport.Height = i_height;
		Viewport.Width  = i_width;
		DEBUG_MSG4(DEBUG_SYS,T("GetThumbnail::PLAYER_SKIN_VIEWPORT Set x %d,y %d,Width %d,Height %d"),Viewport.x,Viewport.y,Viewport.Width,Viewport.Height);
		Player->Set(Player,PLAYER_SKIN_VIEWPORT,&Viewport,sizeof(rect));
	}
	// empty saved playlist
	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::empty saved playlist"));
	Int = 0;
	Player->Set(Player,PLAYER_LIST_COUNT,&Int,sizeof(Int));

	DEBUG_MSG1(DEBUG_SYS,T("GetStringUTFChars %s"),URL);
	Player->Set(Player,PLAYER_LIST_URL+0,URL,sizeof(URL));

	env->ReleaseStringUTFChars(filePath, URL);

	b  = 1;
	Player->Set(Player,PLAYER_PLAYATOPEN_FULL,&b,sizeof(b));

	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::PLAYER_LIST_CURRIDX"));
	Int = 0;
	Player->Set(Player,PLAYER_LIST_CURRIDX,&Int,sizeof(int));

	node* PlayerFormat;
	if( Player->Get(Player,PLAYER_FORMAT,&PlayerFormat,sizeof(PlayerFormat)) == ERR_NONE && PlayerFormat)
	{
		tick_t Tick;

		DEBUG_MSG(DEBUG_SYS,T("GetThumbnail while(!gs_bSyncOk ) "));
		LockEnter(gs_LockSyncThumb);
		while(!gs_bSyncOk)
		{
			EventWait(gs_EventSyncThumb,5000,gs_LockSyncThumb);
			if(!gs_bSyncOk)
			{
				LockLeave(gs_LockSyncThumb);
				goto thumb_failed;
			}
		}
		LockLeave(gs_LockSyncThumb);

		packetformat Format;
		int No;
		bool_t HasVideo = 0;
		for (No=0;PlayerGetStream(Player,No,&Format,NULL,0,NULL);++No)
			if (Format.Type == PACKET_VIDEO)
			{
				HasVideo = 1;
			}

		if(!HasVideo)
		{
			DEBUG_MSG(DEBUG_SYS,T("GetThumbnail if(!HasVideo)"));
			goto thumb_failed;
		}
		if(Player->Get(Player,PLAYER_DURATION,&Tick,sizeof(Tick)) == ERR_NONE && Tick>=0)
		{
			if(Scale(Tick,1000,TICKSPERSEC)>msec && msec>0)
			{
				PlayerCore_SeekTo(msec);
				Thumb.tick = Scale(msec,TICKSPERSEC,1000);
			}
			else
			{
				Thumb.tick		= 0;
			}
		}
		else
		{
			Thumb.tick		= 0;
		}

		iOverlayThumb->Set(iOverlayThumb,OVERLAY_THUMB_INFO,&Thumb,sizeof(thumb_info));
		iOverlayThumb->Get(iOverlayThumb,OVERLAY_THUMB_INFO,&Thumb,sizeof(thumb_info));

		DEBUG_MSG4(DEBUG_SYS,T("GetThumbnail OVERLAY_THUMB_INFO Width %d,Height %d Pitch %d Buffer 0x%x"),\
				Thumb.Height,Thumb.Width,Thumb.Pitch,Thumb.Plane);
	}
	else
	{
		DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::PLAYER_PLAY NO !!"));
		goto thumb_failed;
	}

	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail EventWait(gs_EventThumb,5000,NULL)"));
	LockEnter(gs_LockThumb);
	while(!gs_ThumbOk)
	{
		DEBUG_MSG1(DEBUG_SYS,T("GetThumbnail EventWait(gs_EventThumb,5000,NULL) gs_ThumbOk %d"),gs_ThumbOk);
		EventWait(gs_EventThumb,2000,gs_LockThumb);
		if(!gs_ThumbOk)
		{
			LockLeave(gs_LockThumb);
			goto thumb_failed;
		}
	}
	LockLeave(gs_LockThumb);

	b = 0;
	Player->Set(Player,PLAYER_PLAY,&b,sizeof(b));

	DEBUG_MSG1(DEBUG_SYS,T("GetThumbnail gs_ThumbOk  %d"),gs_ThumbOk);

	if(gs_ThumbOk)
	{
		byteArray = (env)->NewByteArray(i_height*i_width*RGBFlag);
		if (byteArray == NULL)
		{
			DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::if (byteArray == NULL)"));
			goto thumb_failed;
		}

		iOverlayThumb->Get(iOverlayThumb,OVERLAY_THUMB_INFO,&Thumb,sizeof(thumb_info));

		DEBUG_MSG4(DEBUG_SYS,T("GetThumbnail OVERLAY_THUMB_INFO Width %d,Height %d Pitch %d Buffer 0x%x"),\
						Thumb.Height,Thumb.Width,Thumb.Pitch,Thumb.Plane);
		(env)->SetByteArrayRegion(byteArray, 0, i_height*i_width*RGBFlag,(jbyte *)Thumb.Plane);

		(env)->DeleteLocalRef(byteArray);

		Player->Set(Player,PLAYER_FULL_ZOOM,&oldf,sizeof(oldf));
		Player->Set(Player,PLAYER_SKIN_ZOOM,&oldf,sizeof(oldf));
		Int=0;
		Player->Set(Player,PLAYER_LIST_COUNT,&Int,sizeof(Int));
		//Player->Set(Player,PLAYER_STOP,NULL,0);
		Player->Set(Player,PLAYER_NOTIFY,&OldNotify,sizeof(Notify));
#if defined(TARGET_ANDROID)
		//RemoveThreadEnv();
#endif
		return byteArray;

	}
thumb_failed:
	DEBUG_MSG(DEBUG_SYS,T("GetThumbnail::thumb_failed"));
	Player->Set(Player,PLAYER_FULL_ZOOM,&oldf,sizeof(oldf));
	Player->Set(Player,PLAYER_SKIN_ZOOM,&oldf,sizeof(oldf));
	Int=0;
	Player->Set(Player,PLAYER_LIST_COUNT,&Int,sizeof(Int));
	//Player->Set(Player,PLAYER_STOP,NULL,0);
	Player->Set(Player,PLAYER_NOTIFY,&OldNotify,sizeof(Notify));
#if defined(TARGET_ANDROID)
	//RemoveThreadEnv();
#endif
	return 0;
}
}
