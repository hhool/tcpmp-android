#include "../common.h"

#if defined(TARGET_ANDROID)

#include <sys/stat.h>
#include <dirent.h>
#include <unistd.H>
#include <time.h>

#define WRITE_BINARY T("wb")
#define READ_BINARY  T("rb")

typedef struct dirent dirent;

typedef struct filestream
{
	stream Stream;
	tchar_t URL[MAXPATH];
	FILE* Handle;
	filepos_t Length;
	filepos_t Pos;
	bool_t Silent;
	bool_t Create;

	DIR*	dir;
	dirent * entry;
	tchar_t URLEnumDir[MAXPATH];
	const tchar_t* Exts;
	bool_t ExtFilter;
	struct stat finfo;

} filestream;

static int Get(filestream* p, int No, void* Data, int Size)
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case STREAM_URL: GETSTRING(p->URL); break;
	case STREAM_SILENT: GETVALUE(p->Silent,bool_t); break;
	case STREAM_LENGTH: GETVALUECOND(p->Length,int,p->Length>=0); break;
	case STREAM_CREATE: GETVALUE(p->Create,bool_t); break;
	}
	return Result;
}

static int   GetFileSize(FILE* f)
{
	long curpos, length;
	curpos = ftell(f);
	fseek(f, 0L, SEEK_END);
	length = ftell(f);
	fseek(f, curpos, SEEK_SET);
	return length;
}
static int   GetFileSizeByPath(tchar_t* Path)
{
	struct stat buf;
	if (stat(Path, &buf) < 0)
	{
		return -1;
	}
	return buf.st_size;
}
static int Seek(filestream* p,int Pos,int SeekMode)
{
	int Result;

	switch (SeekMode)
	{
	default:
	case SEEK_SET: SeekMode = SEEK_SET; break;
	case SEEK_CUR: SeekMode = SEEK_CUR; break;
	case SEEK_END: SeekMode = SEEK_END; break;
	}

	//DEBUG_MSG3(-1,"FileSeek: %08x %08x %d",SetFilePointer(p->Handle,0,NULL,FILE_CURRENT),Pos,SeekMode);

	Result = fseek(p->Handle,Pos,SeekMode);

	if (Result == ERR_NONE || Result == feof(p->Handle) )
	{
		p->Pos = ftell(p->Handle);
		return p->Pos;
	}

	return Result;
}
static int Open(filestream* p, const tchar_t* URL, bool_t ReOpen)
{
	if (p->Handle)
		fclose(p->Handle);
	p->Handle = 0;
	p->Length = -1;
	if (!ReOpen)
		p->URL[0] = 0;
	else
		sleep(1);

	if (URL && URL[0])
	{
		FILE* Handle;

		Handle = fopen(URL,p->Create?WRITE_BINARY:READ_BINARY);

		if (Handle == NULL)
		{
			if (!ReOpen && !p->Silent)
				ShowError(0,ERR_ID,ERR_FILE_NOT_FOUND,URL);
			return ERR_FILE_NOT_FOUND;
		}

		tcscpy_s(p->URL,TSIZEOF(p->URL),URL);
		p->Handle = Handle;

		p->Length = GetFileSize(p->Handle);
		if (ReOpen)
			p->Pos = Seek(p,p->Pos,SEEK_SET);
		else
		{
			p->Pos = 0;
		}
	}
	return ERR_NONE;
}

static int Set(filestream* p, int No, const void* Data, int Size)
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case STREAM_SILENT: SETVALUE(p->Silent,bool_t,ERR_NONE); break;
	case STREAM_CREATE: SETVALUE(p->Create,bool_t,ERR_NONE); break;
	case STREAM_URL:
		Result = Open(p,(const tchar_t*)Data,0);
		break;
	}
	return Result;
}

static int Read(filestream* p,void* Data,int Size)
{
	size_t   Readed=0;

	//DEBUG_MSG1(DEBUG_TEST,T("filestream::Read:Time %d"),timeGetTime());

	Readed = fread(Data,1,Size,p->Handle);

	if (Readed>=0)
	{
		//DEBUG_MSG3(DEBUG_TEST,T("filestream::Read:Time %d,READ pos:%d len:%d"),timeGetTime(),p->Pos,Readed);
#ifdef _DEBUG
		if(Readed <= 0)
		{
			Readed = Readed;
		}
#endif
		p->Pos += Readed;
		return Readed;
	}

	//DEBUG_MSG2(T("READ pos:%d error:%d"),p->Pos,Error);

	if(feof(p->Handle)||ferror(p->Handle))
		Open(p,p->URL,1);

	return -1;
}

static int ReadBlock(filestream* p,block* Block,int Ofs,int Size)
{
	return Read(p,(char*)(Block->Ptr+Ofs),Size);
}

static int Write(filestream* p,const void* Data,int Size)
{
	size_t Written;
	if ((Written=fwrite(Data,1,Size,p->Handle))&&Written>0)
	{
		fflush(p->Handle);
		p->Pos += Written;
		return Written;
	}
	return -1;
}

static int EnumDir(filestream* p,const tchar_t* URL,const tchar_t* Exts,bool_t ExtFilter,streamdir* Item)
{

	if (URL)
	{
		tchar_t Path[MAXPATH];

		p->Exts = Exts;
		p->ExtFilter = ExtFilter;

		if (p->entry != NULL)
		{
			if(p->dir)
			{
				closedir(p->dir);
				p->dir = NULL;
			}
			p->entry = NULL;
		}

		stprintf_s(Path,TSIZEOF(Path),T("%s"),URL);
		stprintf_s(p->URLEnumDir,TSIZEOF(p->URLEnumDir),T("%s"),URL);

		DEBUG_MSG1(DEBUG_FORMAT,T("format_base::EnumDir:Path %s"),Path);

		if((p->dir=opendir(Path))==NULL)
		{
			return ERR_FILE_NOT_FOUND;
		}
		DEBUG_MSG(DEBUG_SYS,T("file_android::EnumDir:readdir first"));

		if((p->entry = readdir(p->dir))==NULL)
		{
			if(p->dir)
			{
				closedir(p->dir);
				p->dir = NULL;
			}
		}
	}

	Item->FileName[0] = 0;
	Item->DisplayName[0] = 0;

	while(!Item->FileName[0] && p->entry != NULL)
	{
		DEBUG_MSG1(DEBUG_SYS,T("file_android::EnumDir:d_name %s"),p->entry->d_name);

		if (p->entry->d_name[0]!='.') // skip unix/mac hidden files and . .. directory entries
		{
			tchar_t FindPath[MAXPATH];
			stprintf_s(FindPath,TSIZEOF(FindPath),T("%s/%s"),p->URLEnumDir,p->entry->d_name);
			DEBUG_MSG1(DEBUG_SYS,T("file_android::EnumDir:FindPath %s"),FindPath);

			tcscpy_s(Item->FileName,TSIZEOF(Item->FileName),p->entry->d_name);

			Item->Date = FileDate(FindPath);
			DEBUG_MSG1(DEBUG_SYS,T("file_android::EnumDir:FileDate %d"),Item->Date);

			if (p->entry->d_type & 0x4)
				Item->Size = -1; // -1 for directory;
			else
			{
				Item->Size = GetFileSizeByPath(FindPath);
				Item->Type = CheckExts(Item->FileName,p->Exts);
				DEBUG_MSG2(DEBUG_SYS,T("file_android::EnumDir:Size %d Type %c"),Item->Size,Item->Type);
				if (!Item->Type && p->ExtFilter)
					Item->FileName[0] = 0; // skip
			}

		}
		DEBUG_MSG(DEBUG_SYS,T("file_android::EnumDir:readdir second"));
		if((p->entry = readdir(p->dir))==NULL)
		{
			if(p->dir)
			{
				closedir(p->dir);
				p->dir = NULL;
			}
		}
	}
	if (!Item->FileName[0])
	{
		if (p->entry != NULL)
		{
			if(p->dir)
			{
				closedir(p->dir);
				p->dir = NULL;
			}
			p->entry = NULL;
		}
		return ERR_END_OF_FILE;
	}
	return ERR_NONE;
}

static int Create(filestream* p)
{
	p->Stream.Get = (nodeget)Get,
	p->Stream.Set = (nodeset)Set,
	p->Stream.Read = (streamread)Read;
	p->Stream.ReadBlock = (streamreadblock)ReadBlock;
	p->Stream.Write = (streamwrite)Write;
	p->Stream.Seek = (streamseek)Seek;
	p->Stream.EnumDir = (streamenumdir)EnumDir;
	p->entry = NULL;
	return ERR_NONE;
}

static void Delete(filestream* p)
{
	Open(p,NULL,0);
	if (p->dir != NULL)
		closedir(p->dir);
	p->entry = NULL;
}

static const nodedef File =
{
	sizeof(filestream),
	FILE_ID,
	STREAM_CLASS,
	PRI_MINIMUM,
	(nodecreate)Create,
	(nodedelete)Delete,
};

void File_Init()
{
	NodeRegisterClass(&File);
}

void File_Done()
{
	NodeUnRegisterClass(FILE_ID);
}

stream* FileCreate(const tchar_t* Path)
{
	// create fake stream
	filestream* p = (filestream*)malloc(sizeof(filestream));
	memset(p,0,sizeof(filestream));
	p->Stream.Class = FILE_ID;
	p->Stream.Enum = (nodeenum)StreamEnum;
	Create(p);
	return &p->Stream;
}

void FileRelease(stream* p)
{
	if (p)
	{
		Delete((filestream*)p);
		free(p);
	}
}

bool_t FileExits(const tchar_t* Path)
{
	int 	n 	 	= tcslen(Path)*2+1;
	char* 	path 	= malloc(n);

	if(access(path, X_OK|W_OK|R_OK)==-1)
	{
		free(path);
		return 0;
	}

	return 1;
}

int64_t FileDate(const tchar_t* Path)
{
	int64_t Date = -1;
	struct tm*  ptm;
	struct stat buf;
	char szPath[MAXPATH];

	TcsToAscii(szPath,MAXPATH,Path);

	if(stat(szPath,&buf)==0)
	{
		ptm = gmtime(&buf.st_mtime);

		Date = ((((((int64_t)ptm->tm_mon)*100+(int64_t)ptm->tm_mday)*100+
					       (int64_t)ptm->tm_hour)*100+(int64_t)ptm->tm_min)*100+(int64_t)ptm->tm_sec);
	}
	return Date;
}

#endif
