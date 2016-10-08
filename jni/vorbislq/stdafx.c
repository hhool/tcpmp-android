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
 * $Id: stdafx.c 271 2005-08-09 08:31:35Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common/common.h"
#include "vorbis.h"

LIBGCC

int DLLRegister(int Version)
{
	int LocalVersion = CONTEXT_VERSION;
	if (Version != LocalVersion) 
		return ERR_NOT_COMPATIBLE;

	OGG_Init();
	OGGEmbedded_Init();
	OGGPacket_Init();
	Vorbis_Init();
	return ERR_NONE;
}

void DLLUnRegister()
{
	OGG_Done();
	OGGEmbedded_Done();
	OGGPacket_Done();
	Vorbis_Done();
}

//void DLLTest() {}

