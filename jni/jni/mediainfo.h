#ifndef __MEDIAINFO_H
#define __MEDIAINFO_H

#ifdef __cplusplus
extern "C" {
#endif
#define MEDIAINFO_ID		FOURCC('M','E','D','I')

#define MEDIAINFO_FORMAT		0x100
#define MEDIAINFO_CODEC			0x101
#define MEDIAINFO_FPS			0x104
#define MEDIAINFO_SIZE			0x106
#define MEDIAINFO_BITRATE		0x109
#define MEDIAINFO_VIDEO_TOTAL	0x10A
#define MEDIAINFO_VIDEO_DROPPED	0x10B
#define MEDIAINFO_AVG_FPS		0x10C
#define MEDIAINFO_AUDIO_FORMAT	0x10D
#define MEDIAINFO_AUDIO_MONO	0x10E
#define MEDIAINFO_AUDIO_STEREO	0x10F

struct fields_t
{
    jmethodID constructor;
};

struct fields_t fields;

jclass MediaInfo_Input_getClass(JNIEnv *env);

jclass MediaInfo_Comment_getClass(JNIEnv *env);

jclass MediaInfo_Audio_getClass(JNIEnv *env);

jclass MediaInfo_Video_getClass(JNIEnv *env);

jclass MediaInfo_Render_getClass(JNIEnv *env);

void MediaInfo_Input(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name);

void MediaInfo_Reader(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name);

void MediaInfo_Comment(JNIEnv* env,jclass Class,jobject Object,player* Player,int Stream);

void MediaInfo_Video(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name); //no use

void MediaInfo_Audio(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name); //no use

#ifdef __cplusplus
}
#endif
#endif
