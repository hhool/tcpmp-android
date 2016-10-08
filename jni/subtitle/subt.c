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
 * $Id: subt.c 271 2005-08-09 08:31:35Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/
#include "../common/common.h"
#include "subt.h"

#define SUBT_INPUT		0x100
#define SUBT_OUTPUT		0x101

typedef struct subt
{
	node VMT;
	packetformat Format;
	node* Output;
	packetprocess InProcess;
	ovlprocsub OutProcess;
	SubtitleInfo SubInfo;

} subt;

static int SetFormat( subt* p, const packetformat* Format )
{
	if (Format)
	{
		p->Format = *Format;
	}
	return ERR_NONE;
}

static int SetOutput( subt* p, node* Node )
{
	p->Output = Node;
	if(p->Output)
	{
		DEBUG_MSG4(DEBUG_VIDEO,T("subt::SetOutput:Node.Class %c%c%c%c"),
					(char)(Node->Class&0xff),
					(char)(Node->Class>>8&0xff),
					(char)(Node->Class>>16&0xff),
					(char)(Node->Class>>24&0xff));
		if(p->Output->Class == GDI_ID || p->Output->Class == ANDROID_SURFACE_ID)
		{
			p->Output->Get(p->Output,OVERLAY_INPUT_SUB_PROCESS,&p->OutProcess,sizeof(ovlprocsub));
		}
		else
		{
			p->OutProcess = NULL;
		}
		DEBUG_MSG2(DEBUG_VIDEO,T("subt::SetOutput p->Output 0x%x p->OutProcess 0x%x"),p->Output,p->OutProcess);
	}
	return ERR_NONE;
}

static void Discontinuity( subt* p )
{
	memset(&p->SubInfo,0,sizeof(SubtitleInfo));
	DEBUG_MSG2(DEBUG_VIDEO,T("subt::Discontinuity p->Output 0x%x p->OutProcess 0x%x"),p->Output,p->OutProcess);
	if(p->Output&&p->OutProcess)
	{	
		p->OutProcess(p->Output,&p->SubInfo);
	}
	//...
}

static int Process( subt* p, const packet* Packet,const flowstate* State)
{
	int Len;
	
	DEBUG_MSG1(DEBUG_VIDEO,T("subt::Process adr 0x%x"),p->OutProcess);

	if (State->CurrTime >= 0 && Packet->RefTime >= (State->CurrTime + SHOWAHEAD))
		return ERR_BUFFER_FULL;

	//...
	Len = Packet->Length;
	if (Len>255)
		Len=255;
	memset(&p->SubInfo,0,sizeof(SubtitleInfo));
	memcpy(p->SubInfo.szString,Packet->Data[0],Len);
	p->SubInfo.szString[Len] = 0;

	//if (p->Format == STRING_UTF8)
		//UTF8ToTcs(s,256,a);
	//else
		//StrToTcs(s,256,a);
//
	DEBUG_MSG4(DEBUG_VIDEO,T("subt::Process:p->OutProcess 0x%x Packet->Data %d %d: %s"),p->OutProcess,Packet->RefTime,State->CurrTime,p->SubInfo.szString);
	if(p->OutProcess)
	{	
		p->OutProcess(p->Output,&p->SubInfo);
	}
	DEBUG_MSG1(DEBUG_VIDEO,T("subt::Process adr 0x%x end"),p->OutProcess);
	return ERR_NONE;
}

static const datatable SubtitleParams[] = 
{
	{ SUBT_ID,			TYPE_NODE },
	{ SUBT_INPUT,		TYPE_PACKET, DF_INPUT, PACKET_SUBTITLE },
	{ SUBT_OUTPUT,		TYPE_NODE, DF_OUTPUT, VOUT_CLASS },

	DATATABLE_END(SUBT_ID)
};

static int Enum( subt* p, int* No, datadef* Param )
{
	return NodeEnumTable(No,Param,SubtitleParams);
}

static int Get( subt* p, int No, void* Data, int Size )
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case TYPE_NODE: GETVALUE(SUBT_ID,int); break;
	case SUBT_INPUT|PIN_FORMAT: GETVALUE(p->Format,packetformat); break;
	case SUBT_OUTPUT: GETVALUE(p->Output,node*); break;
	case SUBT_INPUT|PIN_PROCESS: GETVALUE(p->InProcess,packetprocess);
		DEBUG_MSG1(DEBUG_VIDEO,T("subt::Get adr p->InProcess 0x%x"),p->InProcess);
		break;
	}
	return Result;
}

static int Set( subt* p, int No, const void* Data, int Size )
{
	int Result = ERR_INVALID_PARAM;
	switch (No)
	{
	case SUBT_INPUT | PIN_FORMAT: 
		if (!Size || Size == sizeof(packetformat))
		{
			if (!Size) Data = NULL;
			Result = SetFormat(p,(const packetformat*)Data);
		}
		break;

	case SUBT_INPUT:
		break;

	case SUBT_OUTPUT:
		DEBUG_MSG1(DEBUG_VIDEO,T("subt::Set:SUBT_OUTPUT node 0x%x"),*(node**)Data);
		Discontinuity(p);
		if (Size == sizeof(node*))
		{
			Result = SetOutput(p,*(node**)Data);
		}
		DEBUG_MSG1(DEBUG_VIDEO,T("subt::Set:SUBT_OUTPUT node 0x%x end"),*(node**)Data);
		break;

	case FLOW_FLUSH:
		Discontinuity(p);
		break;
	}

	return Result;
}
static int Create(subt* p)
{
	p->VMT.Enum = (nodeenum)Enum;
	p->VMT.Get  = (nodeget)Get;
	p->VMT.Set  = (nodeset)Set;
	p->InProcess  = (packetprocess)Process;
	return ERR_NONE;
}
static const nodedef SubT =
{
	sizeof(subt),
	SUBT_ID,
	FLOW_CLASS,
	PRI_DEFAULT,		// lower priority so others can override
	(nodecreate)Create,
	NULL,
};

void SubTitle_Init()
{
	StringAdd(1, SubT.Class, NODE_NAME, T("SUBTITLE(ASS,SSA,SRT,SMI,SUB,TXT)"));
	StringAdd(1, SubT.Class, NODE_CONTENTTYPE, T("subtitle/ASS ,subtitle/SSA ,subtitle/SRT ,subtitle/SMI ,subtitle/SUB ,subtitle/TXT"));
	StringAdd(1, SubT.Class, NODE_EXTS, T("ass:S;ssa:S;srt:S;smi:S;sub:S;txt:S"));
	NodeRegisterClass(&SubT); 
}

void SubTitle_Done()
{
	NodeUnRegisterClass(SUBT_ID);
}
