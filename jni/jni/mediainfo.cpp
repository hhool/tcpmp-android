#include <jni.h>
#include <android/log.h>
#include "../common/common.h"

extern "C"{

extern char g_szPackageDir[1024];

typedef struct MediaInfoMap
{
	char JavaFun[256];
	char NativeFun[256];
	char Parameters[256];
}MediaInfoMap;

static int CommentNo[] = { COMMENT_TITLE, COMMENT_ARTIST, COMMENT_AUTHOR, COMMENT_TRACK, COMMENT_ALBUM, COMMENT_YEAR,
						   COMMENT_GENRE, COMMENT_COPYRIGHT, COMMENT_COMMENT,COMMENT_LANGUAGE, 0 };

static MediaInfoMap MMap[]=
{
		{"mFileName","URL","Ljava/lang/String;"},
		{"mDuration","Duration","I"},
		{"mSize","Filesize","I"},

};

static MediaInfoMap CommentMap[]=
{
		{"mTitle","URL","Ljava/lang/String;"},
		{"mArtist","URL","Ljava/lang/String;"},
		{"mAuthor","URL","Ljava/lang/String;"},
		{"mTrack","URL","Ljava/lang/String;"},
		{"mAlbum","URL","Ljava/lang/String;"},
		{"mYear","URL","Ljava/lang/String;"},
		{"mGenre","URL","Ljava/lang/String;"},
		{"mCopyright","URL","Ljava/lang/String;"},
		{"mComment","URL","Ljava/lang/String;"},
		{"mLanguage","URL","Ljava/lang/String;"},
		{0}
};


jclass MediaInfo_Input_getClass(JNIEnv *env)
{
	char szInput[1024]={0};
	tcscpy_s(szInput,1024,g_szPackageDir);
	tcscat_s(szInput+tcslen(szInput),1024-tcslen(szInput),"/MediaInputInfo");

	return env->FindClass(szInput);
}
jclass MediaInfo_Comment_getClass(JNIEnv *env)
{
	char szComment[1024]={0};
	tcscpy_s(szComment,1024,g_szPackageDir);
	tcscat_s(szComment+tcslen(szComment),1024-tcslen(szComment),"/MediaCommentInfo");
	return env->FindClass(szComment);
}
jclass MediaInfo_Audio_getClass(JNIEnv *env)
{
	char szAudio[1024]={0};
	tcscpy_s(szAudio,1024,g_szPackageDir);
	tcscat_s(szAudio+tcslen(szAudio),1024-tcslen(szAudio),"/MediaAudioInfo");
	return env->FindClass(szAudio);
}
jclass MediaInfo_Video_getClass(JNIEnv *env)
{
	char szVideo[1024]={0};
	tcscpy_s(szVideo,1024,g_szPackageDir);
	tcscat_s(szVideo+tcslen(szVideo),1024-tcslen(szVideo),"/MediaVideoInfo");
	return env->FindClass(szVideo);
}
jclass MediaInfo_Render_getClass(JNIEnv *env)
{
	char szRender[1024]={0};
	tcscpy_s(szRender,1024,g_szPackageDir);
	tcscat_s(szRender+tcslen(szRender),1024-tcslen(szRender),"/MediaRenderInfo");
	return env->FindClass(szRender);
}

void MediaInfo_Comment(JNIEnv* env,jclass Class,jobject Object,player* Player,int Stream)
{
	tchar_t s[256]={0};
	int i;

	DEBUG_MSG(DEBUG_SYS,T("MediaInfo_Comment"));

	for (i=0;CommentNo[i];++i)
		if (Player->CommentByName(Player,Stream,PlayerComment(CommentNo[i]),s,TSIZEOF(s)))
		{
			if (Stream>=0 && Player->CommentByName(Player,-1,PlayerComment(CommentNo[i]),NULL,0))
				continue;

			DEBUG_MSG2(DEBUG_SYS,T("MediaInfo_Comment::SetObjectField CommentMap[i].JavaFun,%s,Value %s"),CommentMap[i].JavaFun,s);

			char szStr[MAXPATH];

			TcsToAscii(szStr,MAXPATH,s);

			DEBUG_MSG(DEBUG_SYS,T("MediaInfo_Comment::CharTojstring beging"));

			jstring str = (jstring)CharTojstring(env,szStr);

			DEBUG_MSG(DEBUG_SYS,T("MediaInfo_Comment::CharTojstring end"));

			env->SetObjectField(Object,
								env->GetFieldID(Class,CommentMap[i].JavaFun,CommentMap[i].Parameters),
								str);
			env->DeleteLocalRef(str);
			DEBUG_MSG(DEBUG_SYS,T("MediaInfo_Comment::SetObjectField end"));
		}
}

void MediaInfo_Input(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name)
{
	int No;
	datadef DataDef;

	for (No=0;NodeEnum(Node,No,&DataDef)==ERR_NONE;++No)
	{
		if (!(DataDef.Flags & (DF_OUTPUT|DF_HIDDEN)))
		{
			tchar_t s[256];
			tick_t Tick;
			int i;
			bool_t Ok = 0;

			switch (DataDef.Type)
			{
			case TYPE_STRING:
				Ok = Node->Get(Node,DataDef.No,s,sizeof(s))==ERR_NONE;
				break;
			}

			DEBUG_MSG1(DEBUG_SYS,T(" MediaInfo_Input DataDef.Name %s"),DataDef.Name);

			if (Ok)
			{
				if(DataDef.No == STREAM_URL)
				{
					DEBUG_MSG(DEBUG_SYS,T(" MediaInfo_Input URL SetObjectField"));
					char szStr[MAXPATH];

					TcsToAscii(szStr,MAXPATH,s);

					DEBUG_MSG2(DEBUG_SYS,T(" MediaInfo_Input ::CharTojstring beging szStr %s,s %s"),szStr,s);

					jstring str = env->NewStringUTF(szStr);//Fixme
				//	jstring str = (jstring)CharTojstring(env,szStr);

					env->SetObjectField(Object,
										env->GetFieldID(Class,"mFileName","Ljava/lang/String;"),
										str);

				}
			}

		}
	}
}
void MediaInfo_Reader(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name)
{
	int No;
	datadef DataDef;

	if(Name)
	{
		DEBUG_MSG1(DEBUG_SYS,T("::MediaInfo_Reader if(Name) %s"),LangStr(Node->Class,NODE_NAME));

		char szStr[MAXPATH];

		TcsToAscii(szStr,MAXPATH,LangStr(Node->Class,NODE_NAME));

		DEBUG_MSG(DEBUG_SYS,T("::MediaInfo_Reader:CharTojstring beging"));

		jstring str = (jstring)CharTojstring(env,szStr);

		env->SetObjectField(Object,
						env->GetFieldID(Class,"mFormat","Ljava/lang/String;"),
						str);
		env->DeleteLocalRef(str);
	}

	for (No=0;NodeEnum(Node,No,&DataDef)==ERR_NONE;++No)
	{
		if (!(DataDef.Flags & (DF_OUTPUT|DF_HIDDEN)))
		{
			tchar_t s[256];
			tick_t Tick;
			int i;
			bool_t Ok = 0;

			switch (DataDef.Type)
			{
			case TYPE_TICK:
				Ok = Node->Get(Node,DataDef.No,&Tick,sizeof(Tick))==ERR_NONE;
				TickToString(s,TSIZEOF(s),Tick,0,1,0);
				break;

			case TYPE_INT:
				Ok = Node->Get(Node,DataDef.No,&i,sizeof(i))==ERR_NONE;
				IntToString(s,TSIZEOF(s),i,(DataDef.Flags & DF_HEX)!=0);
				if (DataDef.Flags & DF_KBYTE)
					tcscat_s(s,TSIZEOF(s),T(" KB"));
				break;

			case TYPE_STRING:
				Ok = Node->Get(Node,DataDef.No,s,sizeof(s))==ERR_NONE;
				break;
			}

			DEBUG_MSG1(DEBUG_SYS,T(":MediaInfo_Reader DataDef.Name %s"),DataDef.Name);

			if (Ok)
			{
				if(DataDef.No == FORMAT_DURATION)
				{
					DEBUG_MSG1(DEBUG_SYS,T(":MediaInfo_Reader Duration SetIntField Tick %d"),Tick);
					env->SetIntField(Object,
										env->GetFieldID(Class,"mDuration","I"),
										Scale(Tick,1000,TICKSPERSEC));
				}
				else if(DataDef.No == FORMAT_FILESIZE)
				{
					DEBUG_MSG1(DEBUG_SYS,T(":MediaInfo_Reader Filesize SetIntField %d"),i);
					env->SetIntField(Object,
										env->GetFieldID(Class,"mSize","I"),
										i);
				}
			}

		}
	}
}
void MediaInfo_Video(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name)
{
	int No;
	datadef DataDef;

	if(Name)
	{
		DEBUG_MSG1(DEBUG_SYS,T(":MediaInfo_Video if(Name) %s"),LangStr(Node->Class,NODE_NAME));

		char szStr[MAXPATH];

		TcsToAscii(szStr,MAXPATH,LangStr(Node->Class,NODE_NAME));

		DEBUG_MSG(DEBUG_SYS,T("::MediaInfo_Video::CharTojstring beging"));

		jstring str = (jstring)CharTojstring(env,szStr);

		env->SetObjectField(Object,
						env->GetFieldID(Class,"mFormat","Ljava/lang/String;"),
						str);
		env->DeleteLocalRef(str);
	}

	for (No=0;NodeEnum(Node,No,&DataDef)==ERR_NONE;++No)
	{
		if (!(DataDef.Flags & (DF_OUTPUT|DF_HIDDEN)))
		{
			tchar_t s[256];
			tick_t Tick;
			int i;
			bool_t Ok = 0;

			switch (DataDef.Type)
			{
			case TYPE_TICK:
				Ok = Node->Get(Node,DataDef.No,&Tick,sizeof(Tick))==ERR_NONE;
				TickToString(s,TSIZEOF(s),Tick,0,1,0);
				break;

			case TYPE_INT:
				Ok = Node->Get(Node,DataDef.No,&i,sizeof(i))==ERR_NONE;
				IntToString(s,TSIZEOF(s),i,(DataDef.Flags & DF_HEX)!=0);
				if (DataDef.Flags & DF_KBYTE)
					tcscat_s(s,TSIZEOF(s),T(" KB"));
				break;

			case TYPE_STRING:
				Ok = Node->Get(Node,DataDef.No,s,sizeof(s))==ERR_NONE;
				break;
			}

			DEBUG_MSG1(DEBUG_SYS,T(":MediaInfo_Video DataDef.Name %s"),DataDef.Name);

			if (Ok)
			{
				if(tcscmp(DataDef.Name,T("Duration"))==0)
				{
					DEBUG_MSG(DEBUG_SYS,T(":MediaInfo_Video Duration SetIntField"));
					env->SetIntField(Object,
										env->GetFieldID(Class,"mDuration","I"),
										Tick);
				}
				else if(tcscmp(DataDef.Name,T("Filesize"))==0)
				{
					DEBUG_MSG(DEBUG_SYS,T(":MediaInfo_Video Filesize SetIntField"));
					env->SetIntField(Object,
										env->GetFieldID(Class,"mSize","I"),
										i);
				}
			}

		}
	}
}
void MediaInfo_Audio(JNIEnv* env,jclass Class,jobject Object,node* Node,int Name)
{
	int No;
	datadef DataDef;

	if(Name)
	{
		DEBUG_MSG1(DEBUG_SYS,T(" if(Name) %s"),LangStr(Node->Class,NODE_NAME));

		char szStr[MAXPATH];

		TcsToAscii(szStr,MAXPATH,LangStr(Node->Class,NODE_NAME));

		DEBUG_MSG(DEBUG_SYS,T("::MediaInfo_Audio::CharTojstring beging"));

		jstring str = (jstring)CharTojstring(env,szStr);

		env->SetObjectField(Object,
						env->GetFieldID(Class,"mFormat","Ljava/lang/String;"),
						str);
		env->DeleteLocalRef(str);
	}

	for (No=0;NodeEnum(Node,No,&DataDef)==ERR_NONE;++No)
	{
		if (!(DataDef.Flags & (DF_OUTPUT|DF_HIDDEN)))
		{
			tchar_t s[256];
			tick_t Tick;
			int i;
			bool_t Ok = 0;

			switch (DataDef.Type)
			{
			case TYPE_TICK:
				Ok = Node->Get(Node,DataDef.No,&Tick,sizeof(Tick))==ERR_NONE;
				TickToString(s,TSIZEOF(s),Tick,0,1,0);
				break;

			case TYPE_INT:
				Ok = Node->Get(Node,DataDef.No,&i,sizeof(i))==ERR_NONE;
				IntToString(s,TSIZEOF(s),i,(DataDef.Flags & DF_HEX)!=0);
				if (DataDef.Flags & DF_KBYTE)
					tcscat_s(s,TSIZEOF(s),T(" KB"));
				break;

			case TYPE_STRING:
				Ok = Node->Get(Node,DataDef.No,s,sizeof(s))==ERR_NONE;
				break;
			}

			DEBUG_MSG1(DEBUG_SYS,T(" DataDef.Name %s"),DataDef.Name);

			if (Ok)
			{
				if(tcscmp(DataDef.Name,T("Duration"))==0)
				{
					DEBUG_MSG(DEBUG_SYS,T(" Duration SetIntField"));
					env->SetIntField(Object,
										env->GetFieldID(Class,"mDuration","I"),
										Tick);
				}
				else if(tcscmp(DataDef.Name,T("Filesize"))==0)
				{
					DEBUG_MSG(DEBUG_SYS,T(" Filesize SetIntField"));
					env->SetIntField(Object,
										env->GetFieldID(Class,"mSize","I"),
										i);
				}
			}

		}
	}
}
}
