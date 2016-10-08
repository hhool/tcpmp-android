#ifndef __VSURFACE_WRAP_H
#define __VSURFACE_WRAP_H

#ifdef __cplusplus
extern "C" {
#endif
void CreateSurfaceLock();

void DestroySurfaceLock();

void ResetSurfaceBuffer();

void LockSurface();

void UnlockSurface();

int GetSurfaceWidth();

int GetSurfaceHeight();

int GetSurfaceBPR();

void* GetSurfaceBuffer();

jint Attach(JNIEnv* env, jobject thiz, jobject surf);

void Detach(JNIEnv* env, jobject thiz);

#ifdef __cplusplus
}
#endif
#endif
