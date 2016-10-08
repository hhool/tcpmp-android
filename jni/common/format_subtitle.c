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
 * $Id: format_subtitle.c 271 2005-08-09 08:31:35Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "common.h"

#define MAXSUBTITLECOUNT		5000

typedef struct format_subtitleitem
{
	bool_t Allocated;
	tick_t StartRefTime;
	tick_t EndRefTime;
	int    x;
	int	   y;
	struct format_subtitleitem* Next;
	char* s;

}format_subtitleitem;

typedef struct format_subtitle
{
	format_stream s;
	format_subtitleitem* List;
	format_subtitleitem* Pos;//记录当前的subitem
	format_subtitleitem* Cur;//记录当前存储的subitem和用于subitem显示开关
	int32_t TypeLoaded;
	int32_t SubsCount;
	int fps;
	tchar_t CurSubStr[1024];
	int ValiableCount;
} format_subtitle;

void ReplaceStr(tchar_t* source,int slen,const tchar_t* orig,const tchar_t* dest)
{
	int srclen = tcslen(source);
	int origlen= tcslen(orig);
	int destlen= tcslen(dest);
	int i = 0;
	size_t n = 0;
	//DEBUG_MSG1(DEBUG_FORMAT,T("format_subtitle::RelpaceStr:source %s"),source);
	//DEBUG_MSG3(DEBUG_FORMAT,T("format_subtitle::RelpaceStr:source %s orig %s dest %s"),source,orig,dest);
	for( i; i < srclen ;i++)
	{
		if(!tcsncmp(&(source[i]),orig,origlen))
		{
			tcscpy_s(&(source[i])+destlen,slen-i-destlen,&(source[i])+origlen);
			n = min(tcslen(dest),slen-i);
			memcpy(&(source[i]),dest,n*sizeof(tchar_t));
		}
	}
	//DEBUG_MSG3(DEBUG_FORMAT,T("format_subtitle::RelpaceStr:after source %s orig %s dest %s"),source,orig,dest);
}
//fixme
int GetSubsTypeByName(format_base* p,format_stream* s,tchar_t* name)
{
	tchar_t * tmp;
	tmp=name;
	while (*tmp!='\0') tmp++;
	
	if (((*(tmp-3)=='a') || (*(tmp-3)=='A')) &&
		((*(tmp-2)=='s') || (*(tmp-2)=='S')) &&
		((*(tmp-1)=='s') || (*(tmp-1)=='S'))) 
		return SUBTITLE_ASS;
	if (((*(tmp-3)=='s') || (*(tmp-3)=='S')) &&
		((*(tmp-2)=='s') || (*(tmp-2)=='S')) &&
		((*(tmp-1)=='a') || (*(tmp-1)=='A')))
		return SUBTITLE_SSA;
	if (((*(tmp-3)=='s') || (*(tmp-3)=='S')) &&
		((*(tmp-2)=='r') || (*(tmp-2)=='R')) &&
		((*(tmp-1)=='t') || (*(tmp-1)=='T'))) 
		return SUBTITLE_SRT;
	if (((*(tmp-3)=='s') || (*(tmp-3)=='S')) &&
		((*(tmp-2)=='m') || (*(tmp-2)=='M')) &&
		((*(tmp-1)=='i') || (*(tmp-1)=='I')))
		return SUBTITLE_SMI;
	if (((*(tmp-3)=='s') || (*(tmp-3)=='S')) &&
		((*(tmp-2)=='u') || (*(tmp-2)=='U')) &&
		((*(tmp-1)=='b') || (*(tmp-1)=='B'))) 
		return SUBTITLE_SUB;
	if (((*(tmp-3)=='t') || (*(tmp-3)=='T')) &&
		((*(tmp-2)=='x') || (*(tmp-2)=='X')) &&
		((*(tmp-1)=='t') || (*(tmp-1)=='T'))) 
		return SUBTITLE_SUB;
	return 0;
}

void AddSubTitle(format_base * p,format_stream *s,format_subtitleitem* subitem)
{
	format_subtitleitem *PreCur,*Cur;
	if(s==NULL)
		return;

	PreCur = ((format_subtitle*)s)->List;
	Cur = ((format_subtitle*)s)->List;

	if(Cur==NULL)
	{
		((format_subtitle*)s)->List = subitem;
		((format_subtitle*)s)->Cur  = subitem;
	}
	else
	{
		do
		{
			if(subitem->StartRefTime>Cur->StartRefTime)
			{
				PreCur = Cur;
				Cur = Cur->Next;
			}
			else
			{
				format_subtitleitem* subtemp = PreCur->Next;
				if(PreCur == Cur && PreCur == ((format_subtitle*)s)->List)
				{
					subtemp = ((format_subtitle*)s)->List;
					((format_subtitle*)s)->List =subitem;
					subitem->Next = subtemp;
				}
				else
				{
					PreCur->Next = subitem;
					subitem->Next = subtemp;
				}
				break;
			}

		}while(Cur);

		if(!Cur&&PreCur)
		{
			PreCur->Next = subitem;
		}

	}

DEBUG_MSG3(DEBUG_FORMAT,T("format_subtitle.c:subitem->s %s,RefStart %d,RefEnd %d"),
	subitem->s,
	subitem->StartRefTime,
	subitem->EndRefTime);
}

void ParseSUB(format_base * p,format_stream *s,buffer * m)
{
	tchar_t *str=m->Data;
	tchar_t *bol=str;
	long long realtime;
	long time1;
	long time2;
	int znow=0;
	float fpsx;
	long long realtime_start=-1;
	long long realtime_end=-1;
	tchar_t *substart = NULL,*subend = NULL;

	int nCount = 0;

	if (*bol=='{')
	{	
		while (*bol)
		{
			if (*bol=='{')
			{
				bol++;
				if (znow==0)
					time1=atoi(bol);
				else if (znow==1) 
					time2=atoi(bol);
				znow++;
				while (*bol!='}' && *bol!='\0')
					bol++;
				if (*bol=='\0') 
					break;
				bol++;
			}
			else
			{
				znow=0;
				if (time1<2 && time2<2)
				{
					stscanf(bol,T("%f"),&fpsx);
					fpsx*=1000;
					((format_subtitle*)s)->fps=(int)fpsx;
				}
				else
				{
					realtime=time1;
					realtime*=TICKSPERSEC;
					realtime*=1000;
					realtime/=((format_subtitle*)s)->fps;
					realtime_start = realtime;
				
					realtime=time2;
					realtime*=TICKSPERSEC;
					realtime*=1000;
					realtime/=((format_subtitle*)s)->fps;
					realtime_end = realtime;
					
					subend = substart = bol;
					while (1)
					{
						if((*(subend+0)=='\r' || *(subend+0)=='\n') &&
							(*(subend+1)=='\r' || *(subend+1)=='\n')||*(subend+0)==0)
						{
							break;
						}
						subend++;
					}
					if(substart != NULL&&substart!=subend)
					{
						format_subtitleitem * subitem = (format_subtitleitem*)malloc(sizeof(format_subtitleitem));
						subitem->s = (char*)malloc(sizeof(char)*(subend-substart+1+2));
						memset(subitem->s,0,sizeof(char)*(subend-substart+1+2));
						tcscpy_s(subitem->s,subend-substart+1,substart);
						subitem->StartRefTime = realtime_start;
						subitem->EndRefTime   = realtime_end;
						subitem->Next = NULL;
						ReplaceStr(subitem->s,1024,T("\r"),T(""));
						ReplaceStr(subitem->s,1024,T("|"),T("\n"));
						ReplaceStr(subitem->s,1024,T("{y: i}"),T(""));
						
						AddSubTitle(p,s,subitem);
						nCount++;
#if 0
						if(nCount>=462)
						{
							nCount = nCount;
						}
#endif
						bol = subend;
					}
					if (nCount>MAXSUBTITLECOUNT-2) 
						break;
				}
				while (*bol!='{' && *bol!='\0') bol++;
				if (*bol=='\0') break;
				*(bol-1)='\0';
			}
		}
	}
	((format_subtitle*)s)->SubsCount=nCount;
	((format_subtitle*)s)->Cur = NULL;
}
void ParseSMI(format_base * p,format_stream *s,buffer * m)
{
	// bol = current symbol parsed
	// bot = begin of last tag
	// eot = end of last tag
	// cot = center of tag pointer
	tchar_t *str=m->Data;
	
	tchar_t *bol=str;
	tchar_t *bot=0,*eot=0,*cot=0;
	int nCount = 0;
	long long realtime;
	
	long long realtime_start=-1;
	long long realtime_end=-1;
	tchar_t *substart = NULL,*subend = NULL;
	tchar_t * tag;
	while (*bol)
	{
		// Step 1 - replace all whitespace to spaces
		if (*bol=='\r' || *bol=='\n' || *bol=='\t') *bol=' ';
		// Step 2 - remove tags
		if (*bol=='&' && *(bol+1)=='n' && *(bol+2)=='b' && *(bol+3)=='s' && *(bol+4)=='p' && *(bol+5)==';')
		{
			*bol=*(bol+1)=*(bol+2)=*(bol+3)=*(bol+4)=*(bol+5)=' ';
		}
		if (*bol=='<') 
		{
			bot=bol;
			eot=0;
		}
		if (*bol=='>')
		{
			eot=bol;
		}
		if (bot && eot)
		{
			if 
			(
			(*(bot+1)=='b' || *(bot+1)=='B') &&
			(*(bot+2)=='r' || *(bot+2)=='R')
			)
			{
				*bot='\n';//fixme
				bot++;
			}
			else while 
			(
			(*(bot+1)=='s' || *(bot+1)=='S') &&
			(*(bot+2)=='y' || *(bot+2)=='Y') &&
			(*(bot+3)=='n' || *(bot+3)=='N') &&
			(*(bot+4)=='c' || *(bot+4)=='C') &&
			(*(bot+5)==' ' || *(bot+5)==' ')
			)
			{
				*bot='\0';
				bot++;
				cot=bot;
				while (*cot!='=' && cot<eot) cot++;
				if (*cot!='=') 
					break;
				cot++;
				if (nCount>=MAXSUBTITLECOUNT-1) 
					break;
				realtime=atoi(cot);
				realtime*=TICKSPERSEC;
				realtime/=1000;
				if(realtime_start == -1)
				{
					realtime_start = realtime;
					tag = cot;
					while(1)
					{
						if(*tag == '>')
						{
							if(*(tag+1) == '<')
							{
								tag+=2;
								continue;
							}
							else
							{
								break;
							}
						}
						tag++;
					}
					substart = tag+1;

				}
				else if(realtime_start>=0&&realtime_end == -1)
				{
					realtime_end = realtime;
					tag = cot;
					while(1)
					{
						if(*tag == 0)
						{
							break;
						}
						tag--;
					}
					subend = tag-1;
					if(substart != NULL&&substart!=subend)
					{
						format_subtitleitem * subitem = (format_subtitleitem*)malloc(sizeof(format_subtitleitem));
						subitem->s = (char*)malloc(sizeof(char)*(subend-substart+1+2));
						memset(subitem->s,0,sizeof(char)*(subend-substart+1+2));
						tcscpy_s(subitem->s,subend-substart+1,substart);
						subitem->StartRefTime = realtime_start;
						subitem->EndRefTime   = realtime_end;
						subitem->Next = NULL;
						ReplaceStr(subitem->s,1024,T("\r"),T(""));
						AddSubTitle(p,s,subitem);
						nCount++;
					}
					realtime_start = realtime_end;
					tag = cot;
					while(1)
					{
						if(*tag == '>')
						{
							if(*(tag+1) == '<')
							{
								tag+=2;
								continue;
							}
							else
							{
								break;
							}
						}
						tag++;
					}
					substart = tag+1;
					realtime_end = -1;
				}
				break;
			}
			while (bot!=eot+1) 
			{
				*bot=' ';
				bot++;
			}
			bot=eot=0;
		}
		bol++;
	}
	((format_subtitle*)s)->SubsCount=nCount;
	((format_subtitle*)s)->Cur = NULL;
}
void ParseSRT(format_base * p,format_stream *s,buffer * m)
{
	tchar_t *str=m->Data;
	tchar_t *bol=m->Data;
	tchar_t *bot=0,*eot=0,*cot=0;
	int nCount=0;
	int semicolons;
	tchar_t *xtime1,*xtext;
	int tt;
	long long realtime;
	long long realtime_start;
	long long realtime_end;
	tchar_t *substart = NULL,*subend = NULL;

	while (*bol)
	{
		if ((*(bol+0)=='\r' || *(bol+0)=='\n') &&
			(*(bol+1)=='\r' || *(bol+1)=='\n') &&
			(((*(bol+2)=='\r' || *(bol+2)=='\n') &&(*(bol+3)=='\r' || *(bol+3)=='\n'))||*(bol+0)==*(bol+1)))
		{
			*bol='\0';
		}
		if (nCount>MAXSUBTITLECOUNT-3)
			break;
		if (*bol=='<') {bot=bol; eot=0;}
		if (*bol=='>') eot=bol;
		if (bot && eot && eot-bot<50)
		{
			while (bot!=eot+1) {*bot=' '; bot++;}
			bot=eot=0;
		}
		if (bol==str || *(bol-1)=='\n' || *(bol-1)=='\r')
		{
			cot=bol;
			semicolons=0;
			while (*cot!='\n' && *cot!='\r' && *cot!='\0')
			{
				if (*cot==':') semicolons++;
				cot++;
			}
			if (*cot=='\0') {break;}
			if (semicolons!=4) {bol++;continue;}
			cot=bol;
			while ((*cot<'0' || *cot>'9') && *cot!='\0') cot++;
			if (*cot=='\0') {bol++; continue;}
			xtime1=cot;
			realtime=0;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30); xtime1++;}; xtime1++;
			realtime+=tt; realtime*=60;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
			realtime+=tt; realtime*=60;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
			realtime+=tt; realtime*=1000;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
			realtime+=tt;
			realtime_start=realtime;
			realtime_start*=TICKSPERSEC;
			realtime_start/=1000;
			
			cot=xtime1;
			while ((*cot<'0' || *cot>'9') && *cot!='\0') cot++;
			if (*cot=='\0') {bol++; continue;}
			xtime1=cot;
			realtime=0;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30); xtime1++;}; xtime1++;
			realtime+=tt; realtime*=60;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
			realtime+=tt; realtime*=60;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
			realtime+=tt; realtime*=1000;
			tt=0; while (*xtime1>='0' && *xtime1<='9') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
			realtime+=tt;
			realtime_end=realtime;
			realtime_end*=TICKSPERSEC;
			realtime_end/=1000;
			
			cot=xtime1;
			while (*cot!='\r' && *cot!='\n' && *cot!='\0') cot++;
			while (*cot=='\r' || *cot=='\n') cot++;
			if (*cot=='\0') {bol++; continue;}
			xtext=cot;
			
			subend = substart = xtext;
			while (*subend)
			{
				if ((*(subend+0)=='\r' || *(subend+0)=='\n') &&(*(subend+1)=='\r' || *(subend+1)=='\n') &&(((*(subend+2)=='\r' || *(subend+2)=='\n') &&(*(subend+3)=='\r' || *(subend+3)=='\n'))||*(subend+0)==*(subend+1)))
				{
					break;
				}
				subend++;
			}
			if(substart != NULL&&substart!=subend)
			{
				format_subtitleitem * subitem = (format_subtitleitem*)malloc(sizeof(format_subtitleitem));
				subitem->s = (char*)malloc(sizeof(char)*(subend-substart+1+2));
				memset(subitem->s,0,sizeof(char)*(subend-substart+1+2));
				tcscpy_s(subitem->s,subend-substart+1,substart);
				subitem->StartRefTime = realtime_start;
				subitem->EndRefTime   = realtime_end;
				subitem->Next = NULL;
				ReplaceStr(subitem->s,1024,T("\r"),T(""));
				AddSubTitle(p,s,subitem);
				nCount++;
			}
		}
		bol++;
	}
	((format_subtitle*)s)->SubsCount=nCount;
	((format_subtitle*)s)->Cur = NULL;
}

void ParseASS(format_base *p,format_stream *s,buffer * m)
{
	tchar_t *bol=m->Data;
	tchar_t *bot=0,*eot=0,*cot=0;
	tchar_t *sub = NULL;
	tchar_t *substart = NULL,*subend = NULL;
	int nCount=0;
	int ztime1=-1;
	int ztime2=-1;
	int ztext=-1;
	tchar_t *xtime1,*xtime2,*xtext;
	int znow;
	int tt;
	long long realtime;
	long long realtime_start;
	long long realtime_end;

	while (*bol)
	{
		if ((*bol=='\\' && *(bol+1)=='N') || (*bol=='\\' && *(bol+1)=='n'))
		{
			*bol='|'; *(bol+1)=' ';
		}
		if (*bol=='\r'|| *bol=='\n') *bol='\0';
		if (*bol=='{') {bot=bol; eot=0;}
		if (*bol=='}') eot=bol;
		if (bot && eot)
		{
			while (bot!=eot+1) { bot++;}
			bot=eot=0;
		}
		if ((*bol=='\r' || *bol=='\n' || *bol=='\0') && *(bol+1)!='\r' && *(bol+1)!='\n')
		{
			if (tcsncmp(bol+1,"Format:",7)==0)
			{
				cot=bol+1;
				while (*cot!=':' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
				if (*cot==':') cot++;
				while (*cot==' ' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
				if (*cot=='\0' || *cot=='\n' || *cot=='\r') break;
				znow=0;
				while (1)
				{
					if (tcsncmp(cot,"Start",5)==0) ztime1=znow;
					if (tcsncmp(cot,"End",3)==0) ztime2=znow;
					if (tcsncmp(cot,"Text",4)==0) ztext=znow;
					while (*cot!=',' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
					if (*cot==',') cot++;
					while (*cot==' ' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
					if (*cot=='\0' || *cot=='\n' || *cot=='\r') break;
					znow++;
				}

			}
			if (tcsncmp(bol+1,"Dialogue:",9)==0)
			{
				xtime1=xtime2=xtext=0;
				cot=bol+1;
				while (*cot!=':' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
				if (*cot==':') cot++;
				while (*cot==' ' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
				if (*cot=='\0' || *cot=='\n' || *cot=='\r') break;
				znow=0;
				while (1)
				{
					if (znow==ztime1) xtime1=cot;
					if (znow==ztime2) xtime2=cot;
					if (znow==ztext)  xtext=cot;
					while (*cot!=',' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
					if (*cot==',') cot++;
					while (*cot==' ' && *cot!='\0' && *cot!='\n' && *cot!='\r') cot++;
					if (*cot=='\0' || *cot=='\n' || *cot=='\r') break;
					znow++;
				}
				if (xtime1 && xtime2 && xtext && nCount<=MAXSUBTITLECOUNT-3)
				{
					char SubItem[1024]={0};
					int In = 0;

					realtime=0;
					tt=0; while (*xtime1!=':') {tt*=10; tt+=(*xtime1-0x30); xtime1++;}; xtime1++;
					realtime+=tt; realtime*=60;
					tt=0; while (*xtime1!=':') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
					realtime+=tt; realtime*=60;
					tt=0; while (*xtime1!='.') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
					realtime+=tt; realtime*=100;
					tt=0; while (*xtime1!=',') {tt*=10; tt+=(*xtime1-0x30);xtime1++;}; xtime1++;
					realtime+=tt;
					realtime_start=realtime;
					realtime_start*=TICKSPERSEC;
					realtime_start/=100;
					
					sub =xtext;
					cot=xtext;
					while (*cot!='\0' && *cot!='\n' && *cot!='\r') 
					{
						cot++;
					}
					
					
					realtime=0;		
					tt=0; while (*xtime2!=':') {tt*=10; tt+=(*xtime2-0x30);xtime2++;}; xtime2++;
					realtime+=tt; realtime*=60;
					tt=0; while (*xtime2!=':') {tt*=10; tt+=(*xtime2-0x30);xtime2++;}; xtime2++;
					realtime+=tt; realtime*=60;
					tt=0; while (*xtime2!='.') {tt*=10; tt+=(*xtime2-0x30);xtime2++;}; xtime2++;
					realtime+=tt; realtime*=100;
					tt=0; while (*xtime2!=',') {tt*=10; tt+=(*xtime2-0x30);xtime2++;}; xtime2++;
					realtime+=tt;
					realtime_end=realtime;
					realtime_end*=TICKSPERSEC;
					realtime_end/=100;
										
					while (sub !=0) 
					{
						if(*sub == '}'&&!((*sub=='\r' || *sub=='\n' || *sub=='\0')) )
						{
							substart = sub+1;
							In = 0;
						}
						else if(*sub == '{')
						{
							subend = sub-1;
							In = 1;
						}
						else if ((*sub=='\r' || *sub=='\n' || *sub=='\0'))
						{
							break;
						}
						else if(In ==0 && !substart)
						{
							substart = sub;
						}
						else if(substart!=NULL&&subend!=NULL&&substart<subend)
						{
							tcsncpy_s(SubItem+tcslen(SubItem),1024,substart,subend-substart+1);
							/*if(tcslen(SubItem)>2)//还有在字符串中间出现的，不解。
							{
								int Len = tcslen(SubItem);
								if(SubItem[Len-1] == 'N'&&SubItem[Len-2] == '\\')
								{
									SubItem[Len-2] = '\n';
									SubItem[Len-1] = 0;
								}
							}*/
							substart = NULL;
							subend = NULL;
						}
						sub++;
					}
					
					subend = sub;
					
					if(substart!=NULL&&subend!=NULL&&substart<subend)
					{
						tcsncpy_s(SubItem+tcslen(SubItem),1024,substart,subend-substart+1);
					}

					if(tcslen(SubItem)>0)
					{
						format_subtitleitem * subitem = (format_subtitleitem*)malloc(sizeof(format_subtitleitem));
						subitem->s = (char*)malloc(sizeof(char)*(1024));
						memset(subitem->s,0,sizeof(char)*1024);
						ReplaceStr(SubItem,1024,T("\\N"),T("\n"));
						ReplaceStr(SubItem,1024,T("\r"),T(""));
						tcscpy_s(subitem->s,1024,SubItem);
						memset(SubItem,0,sizeof(SubItem));
						subitem->StartRefTime = realtime_start;
						subitem->EndRefTime   = realtime_end;
						subitem->Next = NULL;

						substart = subend = NULL;

						AddSubTitle(p,s,subitem);
						nCount++;
					}					
				}
			}
		}
		bol++;
	}
	((format_subtitle*)s)->SubsCount=nCount;
	((format_subtitle*)s)->Cur = NULL;
}

format_stream* Format_LoadSubTitle(format_base* p, stream* Input)
{
	int No;

	format_stream* s;
	tchar_t FilePath[MAXPATH];
	int Length;
	int ReadLen;
	buffer m = {NULL};
	buffer m1 = {NULL};
	int fps=0;
	
	for (No=0;No<p->StreamCount;++No)
	{
		packetformat Format;
		p->Format.Get(p,(FORMAT_STREAM|PIN_FORMAT)+No,&Format,sizeof(Format));
		if (Format.Type == PACKET_VIDEO && Format.PacketRate.Num>0)
		{
			fps=Format.PacketRate.Num*1000/Format.PacketRate.Den;
			break;
		}
	}
	
	if(Input&&Input->Get(Input,STREAM_LENGTH,&Length,sizeof(Length)) != ERR_NONE)
		return NULL;

	if (Input && Input->Get(Input,STREAM_URL,FilePath,sizeof(FilePath))!=ERR_NONE)
		return NULL;


	BufferAlloc(&m,(Length+4096-1)/4096*4096,1);
	memset(m.Data,0,m.Allocated);
	BufferStream(&m,Input);
	ReadLen = m.WritePos - m.ReadPos;
	DEBUG_MSG1(DEBUG_FORMAT,T("format_base::Format_LoadSubTitle:Input.Len %d"),ReadLen);
	if(ReadLen <= 0)
		return NULL;
	//!SUBTITLE
	if (p->StreamCount >= MAXSTREAM-1-1) //subtitle,coverart
		return NULL;

	s = (format_stream*) malloc(sizeof(format_subtitle));
	if (!s)
		return NULL;

	memset(((format_subtitle*)s),0,sizeof(format_subtitle));

	if(fps>0)
	{
		((format_subtitle*)s)->fps = fps;
	}
	else
	{
		((format_subtitle*)s)->fps = 25000;
	}

	((format_subtitle*)s)->List = NULL;

	if((m.Data[0] == 0xFF&& m.Data[1]== 0xFE)||(m.Data[0]==0xFE && m.Data[1]==0xFF))//UNICODE
	{
		BufferAlloc(&m1,(Length+4096-1)/4096*4096,1);
		memset(m1.Data,0,m1.Allocated);
		if(m.Data[0]==0xFE && m.Data[1]==0xFF)
		{
			int i = 0;
			int temp = 0;
			while(i<ReadLen)
			{
				temp = m.Data[i];
				m.Data[i]=m.Data[i+1];
				m.Data[i+1] = temp;
				i+=2;
			}
		}
		WcsToTcs(m1.Data,(Length+4096-1)/4096*4096,m.Data+2);
	}
	else if(m.Data[0]==0xEF && m.Data[1]==0xBB && m.Data[2]==0xBF)//UTF8
	{  
		BufferAlloc(&m1,(Length+4096-1)/4096*4096,1);
		memset(m1.Data,0,m1.Allocated);
		UTF8ToTcs(m1.Data,(Length+4096-1)/4096*4096,m.Data+3);
	}
	else if(m.Data[0]==0x2B && m.Data[1]==0x2F && m.Data[2]==0x76 && (m.Data[3]==0x38 || m.Data[3]==0x39 || m.Data[3]==0x2B || m.Data[3]==0x2F)) //UTF7
	{
		BufferAlloc(&m1,(Length+4096-1)/4096*4096,1);
		memset(m1.Data,0,m1.Allocated);
		UTF7ToTcs(m1.Data,(Length+4096-1)/4096*4096,m.Data+4);
	}
	else//ASCII
	{
		BufferAlloc(&m1,(Length+4096-1)/4096*4096,1);
		memset(m1.Data,0,m1.Allocated);
		memcpy(m1.Data,m.Data,ReadLen);
	}
	((format_subtitle*)s)->TypeLoaded = GetSubsTypeByName(p,s,FilePath);
	if (((format_subtitle*)s)->TypeLoaded==SUBTITLE_ASS || ((format_subtitle*)s)->TypeLoaded==SUBTITLE_SSA) 
	{
		DEBUG_MSG(DEBUG_FORMAT,T("format_base::Format_LoadSubTitle:ParseASS"));
		ParseASS(p,s,&m1);
	}
	else if (((format_subtitle*)s)->TypeLoaded==SUBTITLE_SRT) 
	{
		DEBUG_MSG(DEBUG_FORMAT,T("format_base::Format_LoadSubTitle:ParseSRT"));
		ParseSRT(p,s,&m1);
	}
	else if (((format_subtitle*)s)->TypeLoaded==SUBTITLE_SMI) 
	{
		DEBUG_MSG(DEBUG_FORMAT,T("format_base::Format_LoadSubTitle:ParseSMI"));
		ParseSMI(p,s,&m1);
	}
	else if (((format_subtitle*)s)->TypeLoaded==SUBTITLE_SUB) 
	{
		DEBUG_MSG(DEBUG_FORMAT,T("format_base::Format_LoadSubTitle:ParseSUB"));
		ParseSUB(p,s,&m1);
	}

	if(((format_subtitle*)s)->List==NULL)
	{	
		free(s);
		s= NULL;
	
	}
	else if (s)
	{
		s->Format.Type = PACKET_SUBTITLE;
		s->Format.Format.Subtitle.FourCC = ((format_subtitle*)s)->TypeLoaded;
		s->LastTime = -1;
	}

	BufferClear(&m);
	BufferClear(&m1);
	return s;
}

void Format_FreeSubTitle(format_base* p,format_stream* s)
{
	format_subtitleitem* i;
	format_subtitleitem* Last=NULL;

	for (i=((format_subtitle*)s)->List;i;i=i->Next)
		if (i->Allocated)
		{
			free(Last);
			Last = i; 
			if(Last&&Last->s!=NULL)
			{
				free(Last->s);
				Last->s = NULL;
			}
		}

	free(Last);

	PacketFormatClear(&s->Format);
	
	free(s);
}

void Format_ProcessSubTitle(format_base* p,format_stream* s)
{
	if (!s->Pending)
	{
		format_subtitleitem* Last;
		format_subtitle* Stream = (format_subtitle*)s;
		format_subtitleitem* Pos=NULL;
		tick_t Time = p->ProcessTime;
		int ValiableCount = 0;
		if (Time<0)
		{
			int No;
			// benchmark: find first video stream with valid LastTime
			for (No=0;No<p->StreamCount;++No)
				if (p->Streams[No]->Format.Type == PACKET_VIDEO &&
					p->Streams[No]->LastTime >= 0)
				{
					Time = p->Streams[No]->LastTime;
					break;
				}

			if (Time<0)
				return;
		}

		if (Stream->s.LastTime<0)
			Stream->Pos = Stream->List;
		if (Stream->Pos  == NULL)
			Stream->Pos = Stream->List;

		Stream->s.LastTime = Time;

		Pos = Stream->Pos;
		Last = NULL;
		memset(Stream->CurSubStr,0,1024);
		while (Pos)
		{
			if(Time>=Pos->StartRefTime&&Time<Pos->EndRefTime)
			{
				int sizeSub = tcslen(Stream->CurSubStr);
				if(sizeSub)
				{
					tchar_t szRet[2] = {T("\n")};
					tcscat_s(Stream->CurSubStr+tcslen(Stream->CurSubStr),1024-tcslen(Stream->CurSubStr),szRet);
					tcscat_s(Stream->CurSubStr+tcslen(Stream->CurSubStr),1024-tcslen(Stream->CurSubStr),Pos->s);
				}
				else
					tcscpy_s(Stream->CurSubStr,1024-sizeSub,Pos->s);

				if(Last==NULL)
				{
					Last=Pos;
				}
				ValiableCount++;
			}
			Pos = Pos->Next;
		}

		//if selected sub equal dispaly now return
		if(Last == Stream->Cur && Last !=NULL && Stream->ValiableCount == ValiableCount)
			return;

		if (!Last)
		{
			if(Stream->Cur&&Stream->Cur->EndRefTime<Time)
			{
				Stream->s.Packet.RefTime = Stream->Cur->StartRefTime;
				Stream->s.Packet.Key = 1;
				Stream->s.Packet.Data[0] = NULL;
				Stream->s.Packet.Length = 0;
				Stream->s.Pending = 1;
				Stream->Pos = Stream->Cur;
				Stream->Cur = NULL;
			}
			else
				return;
		}
		else
		{
			Stream->s.Packet.RefTime = Last->StartRefTime;
			Stream->s.Packet.Key = 1;
			Stream->s.Packet.Data[0] = Stream->CurSubStr;
			Stream->s.Packet.Length = tcslen(Stream->CurSubStr);
			DEBUG_MSG1(DEBUG_FORMAT,T("format_subtitle::Process:Stream->CurSubStr %s"),Stream->CurSubStr);
			Stream->s.Pending = 1;
			Stream->Cur = Last;
			Stream->ValiableCount = ValiableCount;
		}
	}

	s->State.CurrTime = p->ProcessTime;
	if (s->Process)
	{
		if(s->Process(s->Pin.Node,&s->Packet,&s->State) != ERR_BUFFER_FULL)
		s->Pending = 0;
	}
}


