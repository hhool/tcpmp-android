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
 * $Id: stdafx.c 284 2005-10-04 08:54:26Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common/common.h"
#include "ffmpeg.h"

LIBGCC
LIBGCCFLOAT

int DLLRegister(int Version)
{
	int LocalVersion = CONTEXT_VERSION;
	if (Version != LocalVersion) 
		return ERR_NOT_COMPATIBLE;

	FFMPEGV_Origin_Init();
	FFMPEGV_Real_Init();
	FFMPEGA_Origin_Init();
	FFMPEGA_Real_Init();
	return ERR_NONE;
}

void DLLUnRegister()
{
	FFMPEGV_Origin_Done();
	FFMPEGV_Real_Done();
	FFMPEGA_Origin_Done();
	FFMPEGA_Real_Done();
}

//void DLLTest() {}

