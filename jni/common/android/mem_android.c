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
 * $Id: mem_win32.c 430 2005-12-28 14:50:08Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"

#if defined(TARGET_ANDROID)
#include<fcntl.h>

//#define BLOCKGROUP	8

typedef struct blockgroup
{
	block Block;
	int Mask;
	int Unused; //align to 16bytes

} blockgroup;

//static int PageSize = 4096;
//static array BlockGroup = { NULL };

#ifndef NDEBUG
static int BlockAllocated = 0;
#endif
void* 	PhyMemAlloc(int Length,phymemblock* Blocks,int* BlockCount) { return NULL; }
void 	PhyMemFree(void* p,phymemblock* Blocks,int BlockCount) {}
void* 	PhyMemBeginEx(phymemblock* Blocks,int BlockCount,bool_t Cached) { return NULL; }
void* 	PhyMemBegin(uint32_t Phy,uint32_t Length,bool_t Cached) { return NULL; }
void 	PhyMemEnd(void* Virt) {}
bool_t MemGetInfo(memoryinfo* p)
{
	return 0;
}

void* CodeAlloc(int Size)
{
	return malloc(Size);
}

void CodeFree(void* Code,int Size)
{
	free(Code);
}
void CodeLock(void* Code,int Size) {}

void CodeUnlock(void* Code,int Size)
{
	// instruction cache flush needed
}


void CodeFindPages(void* Ptr,uint8_t** PMin,uint8_t** PMax,size_t* PPageSize)
{
	if (PMin)
		*PMin = (uint8_t*)Ptr;
	if (PMax)
		*PMax = (uint8_t*)Ptr;
	if (PPageSize)
		*PPageSize = 4096;
}

void CheckHeap()
{

}

//#define FIND 0xAD1A30

size_t AvailMemory()
{
	int64_t freeSize=0xffffffff;
	//GetFreeMem(&freeSize);
	return freeSize; //(16M-1)
}

void WriteBlock(block* Block,int Ofs,const void* Src,int Length)
{
	memcpy((uint8_t*)Block->Ptr+Ofs,Src,Length);
}

void FreeBlock(block* Block)
{
	if (Block)
	{
		free((char*)Block->Ptr);
		Block->Id = 0;
		Block->Ptr = NULL;
	}
}

bool_t SetHeapBlock(int n,block* Block,int Heap)
{
	return 0;
}

#ifdef BLOCKGROUP
static bool_t AllocBlockGroup(block* Block, bool_t Optional)
{
	int n;
	blockgroup* g;
	for (g=ARRAYBEGIN(BlockGroup,blockgroup);g!=ARRAYEND(BlockGroup,blockgroup);++g)
		if (g->Mask && g->Mask != (1<<BLOCKGROUP)-1)
			break;

	if (g==ARRAYEND(BlockGroup,blockgroup))
	{
		if (!Optional)
			return 0;

		for (g=ARRAYBEGIN(BlockGroup,blockgroup);g!=ARRAYEND(BlockGroup,blockgroup);++g)
			if (!g->Mask)
				break;

		if (g==ARRAYEND(BlockGroup,blockgroup))
		{
			if (!ArrayAppend(&BlockGroup,NULL,sizeof(blockgroup),64))
				return 0;
			g=ARRAYEND(BlockGroup,blockgroup)-1;
			g->Mask = 0;
		}

		if (!AllocBlock(BLOCKSIZE*BLOCKGROUP,&g->Block,1,HEAP_ANY))
			return 0;
	}

	for (n=0;n<BLOCKGROUP;++n)
		if (!(g->Mask & (1<<n)))
		{
			g->Mask |= (1<<n);
			Block->Id = ((g-ARRAYBEGIN(BlockGroup,blockgroup))<<8)+(n+1);
			Block->Ptr = g->Block.Ptr + n*BLOCKSIZE;
			return 1;
		}

	return 0;
}

#endif

bool_t AllocBlock(size_t n,block* Block,bool_t Optional,int Heap)
{
	void* p;

	if (Optional && AvailMemory() < (256+64)*1024+n) // we want to avoid OS low memory warning
		return 0;
#ifdef BLOCKGROUP
	if (n == BLOCKSIZE && AllocBlockGroup(Block,Optional))
		return 1;
#endif
	do
	{
		p = malloc(n);
		if (p)
		{
			Block->Ptr = p;
			Block->Id = 0;
			return 1;
		}
	}
	while (!p && !Optional && n && NodeHibernate());

	Block->Ptr = p;
	Block->Id = 0;

#ifndef NDEBUG
	if (p) ++BlockAllocated;
#endif

	return p!=NULL;
}

#undef malloc
#undef realloc
#undef free

			
#ifdef SAFEGUARD
void* safeset(uint8_t* p,int n,bool_t clear)
{
	if (p)
	{
		*(int*)p = n;
		p += sizeof(int);
		memset(p,0xDA,SAFEGUARD);
		p += SAFEGUARD;
		if (clear)
			memset(p,0xCC,n);
		p += n;
		memset(p,0xDE,SAFEGUARD);
		p -= n;
	}
	return p;
}
void* safeget(uint8_t* p,bool_t clear)
{
	if (p)
	{
		int n = *(int*)(p-SAFEGUARD-sizeof(int));
		int i;
		for (i=0;i<SAFEGUARD;++i)
			assert(p[-i-1] == 0xDA);
		for (i=0;i<SAFEGUARD;++i)
			assert(p[n+i] == 0xDE);
		p -= SAFEGUARD+sizeof(int);
		if (clear)
			memset(p,0xEE,n+SAFEGUARD*2+sizeof(int));
	}
	return p;
}
#endif

void* malloc_android(size_t n)
{
	void* p = NULL;
	if (n)
	{
		do
		{
#ifdef SAFEGUARD
			p = safeset(malloc(n+SAFEGUARD*2+sizeof(int)),n,1);
#else
			p = malloc(n);
#endif

#ifdef FIND
			if ((int)p == FIND)
				DebugBreak();
#endif
		} while (!p && NodeHibernate());
	}
	return p;
}

void* realloc_android(void* p,size_t n)
{
	do
	{
#ifdef SAFEGUARD
		p = safeset(realloc(safeget(p,0),n+SAFEGUARD*2+sizeof(int)),n,p==NULL);
#else
		p = realloc(p,n);
#endif

#ifdef FIND
		if ((int)p == FIND)
			DebugBreak();
#endif
	} while (!p && n && NodeHibernate());
	return p;
}

void free_android(void* p)
{
#ifdef SAFEGUARD
	free(safeget(p,1));
#else
	free(p);
#endif
}

void ShowOutOfMemory()
{
}

void EnableOutOfMemory()
{
}

void DisableOutOfMemory()
{
}

void Mem_Init()
{
	//FIXME IMPORTANT
}

void Mem_Done()
{

}

#endif
