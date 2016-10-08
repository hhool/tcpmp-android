#include "../common/common.h"
#include <pthread.h>
#include <jni.h>
#if PLATFORM < 8
#include <ui/Surface.h>
#elif PLATFORM >= 8 && PLATFORM < 16
#include <surfaceflinger/Surface.h>
#elif PLATFORM >= 16
#include <gui/Surface.h>
#else
#error "?"
#endif
#include "environment.h"

using namespace android;

#include "vsurface_wrap.h"

static void* mutex = 0;
static int locked = 0;
static Surface* surface = 0;
static char dummy[sizeof(Surface::SurfaceInfo)];
static Surface::SurfaceInfo* info = (Surface::SurfaceInfo*) dummy;

void CreateSurfaceLock()
{
	DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::CreateSurfaceLock mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
	mutex = (pthread_mutex_t*)LockCreate();
    locked = 0;
    DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::CreateSurfaceLock  end mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
}

void DestroySurfaceLock()
{
	DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::DestroySurfaceLock mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
    LockEnter(mutex);
    surface = 0;
    locked = 0;
    LockLeave(mutex);
    LockDelete(mutex);
    DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::DestroySurfaceLock end mutex 0x%x,ThreadId 0x%x end"),mutex,ThreadId());
}

void LockSurface()
{
	DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::LockSurface mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
    LockEnter(mutex);
    DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::LockSurface Enter mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
    if (surface)
    {
    	DEBUG_MSG3(DEBUG_VIDEO,T("vsurface_wrap::LockSurface Enter mutex 0x%x,ThreadId 0x%x,surface 0x%x"),mutex,ThreadId(),surface);
        surface->lock(info);
        DEBUG_MSG3(DEBUG_VIDEO,T("vsurface_wrap::LockSurface Enter mutex 0x%x,ThreadId 0x%x,surface locked 0x%x"),mutex,ThreadId(),surface);
        locked = -1;
    }
}

void UnlockSurface()
{
    if (surface)
    {
        surface->unlockAndPost();
        DEBUG_MSG3(DEBUG_VIDEO,T("vsurface_wrap::UnlockSurface Enter mutex 0x%x,ThreadId 0x%x surface unlocked"),mutex,ThreadId(),surface);
        locked = 0;
    }
    DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::UnlockSurface Enter mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
    LockLeave(mutex);
    DEBUG_MSG2(DEBUG_VIDEO,T("vsurface_wrap::UnlockSurface mutex 0x%x,ThreadId 0x%x"),mutex,ThreadId());
}
int GetSurfaceBPR()
{
#if PLATFORM < 8
	DEBUG_MSG6(DEBUG_VIDEO,T("GetSurfaceBPR:w %d, h %d, bpr %d,format 0x%x,bits 0x%x,base 0x%x"),\
		info->w,info->h,info->bpr,info->format,info->bits,info->base);
	if(g_Environment.m_iSdkVersion == 4)
	{
		return locked ? info->bpr : 0;
	}
	else
		return locked ? info->bpr*2 : 0;
#elif PLATFORM >= 8
	DEBUG_MSG5(DEBUG_VIDEO,T("GetSurfaceBPR:w %d, h %d, s %d,format 0x%x,bits 0x%x"),\
			info->w,info->h,info->s,info->format,info->bits);
	return locked ? info->s*2 : 0;
#else
#error "?"
#endif
}
int GetSurfaceWidth()
{
	DEBUG_MSG2(DEBUG_VIDEO,T("getSurfaceWidth locked %d,info->w %d"),locked,locked?info->w:-1);
    return locked ? info->w : 0;
}

int GetSurfaceHeight()
{
	DEBUG_MSG2(DEBUG_VIDEO,T("getSurfaceHeight locked %d,info->h %d"),locked,locked?info->h:-1);
    return locked ? info->h : 0;
}

void* GetSurfaceBuffer()
{
    // workaround for eclair
#if PLATFORM < 8

	DEBUG_MSG6(DEBUG_VIDEO,T("GetSurfaceBuffer:w %d, h %d, bpr %d,format 0x%x,bits 0x%x,base 0x%x"),\
	info->w,info->h,info->bpr,info->format,info->bits,info->base);
    return locked ? (reinterpret_cast<int>(info->bits) < 0x0200 ? info->base : info->bits) : 0;
#elif PLATFORM >= 8
    return locked ? info->bits : 0;
#else
#error "?"
#endif
}
static void ResetSurface()
{
	int sw, sh;
	uint8_t* ptr=NULL;

	LockSurface();

	ptr = (uint8_t*)GetSurfaceBuffer();
	sw = GetSurfaceWidth();
	sh = GetSurfaceHeight();

	if(ptr)
	{
		memset(ptr,0,sh*sw*2);

		DEBUG_MSG1(DEBUG_VIDEO,"ResetSurfaceBuffer::memset Surface! %p", surface);
	}

	UnlockSurface();
}
void ResetSurfaceBuffer()
{
	ResetSurface();
	ResetSurface();
}
jint Attach(JNIEnv* env, jobject thiz, jobject surf)
{
	// get java surface class
	jfieldID fid;
	jclass cls = env->GetObjectClass(surf);
	if(g_Environment.m_iSdkVersion <= 8)
	{
		DEBUG_MSG(DEBUG_VIDEO,"GetFieldID_mSurface_BEFORE");
		fid= env->GetFieldID(cls, "mSurface", "I");
		DEBUG_MSG1(DEBUG_VIDEO,"GetFieldID_mSurface_AFTER %d",fid);
	}
	else
	{
		DEBUG_MSG(DEBUG_VIDEO,"GetFieldID_mNativeSurface_BEFORE");
		fid = env->GetFieldID(cls, "mNativeSurface", "I");
		DEBUG_MSG1(DEBUG_VIDEO,"GetFieldID_mNativeSurface_AFTER %d",fid);
	}
    Surface* ptr = (Surface*)env->GetIntField(surf, fid);
    if (ptr)
    {
        LockEnter(mutex);
        surface = ptr;
        locked = 0;
		ResetSurfaceBuffer();
        LockLeave(mutex);
        DEBUG_MSG1(DEBUG_VIDEO,"Attach::Got Surface! %p", surface);
        return 0;
    }
    return -1;
}

void Detach(JNIEnv* env, jobject thiz)
{
    if (surface)
    {
        LockEnter(mutex);
		ResetSurfaceBuffer();
        surface = 0;
        locked = 0;
        memset(dummy,0,sizeof(Surface::SurfaceInfo));
        LockLeave(mutex);
    }
}

