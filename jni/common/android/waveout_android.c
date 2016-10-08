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
 * $Id: waveout_win32.c 543 2006-01-07 22:06:24Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"
#include <math.h>

#if defined(TARGET_ANDROID)

typedef struct waveout
{
	node Node;

} waveout;

static int Create(waveout* p)
{
	return ERR_NONE;
}

static void Delete(waveout* p)
{

}

static const nodedef WaveOut =
{
	sizeof(waveout)|CF_GLOBAL,
	WAVEOUT_ID,
	AOUT_CLASS,
	PRI_DEFAULT,
	(nodecreate)Create,
	(nodedelete)Delete,
};

void WaveOut_Init()
{
	NodeRegisterClass(&WaveOut);
}

void WaveOut_Done()
{
	NodeUnRegisterClass(WAVEOUT_ID);
}

#endif
