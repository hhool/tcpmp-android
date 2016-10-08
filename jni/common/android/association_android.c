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
 * $Id: association_win32.c 603 2006-01-19 13:00:33Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#include "../common.h"

#if defined(TARGET_ANDROID)

static const nodedef Association =
{
	sizeof(node)|CF_GLOBAL|CF_SETTINGS,
	ASSOCIATION_ID,
	NODE_CLASS,
	PRI_MAXIMUM+500,
	NULL,
};

void Association_Init()
{
	NodeRegisterClass(&Association);
}

void Association_Done()
{
	NodeUnRegisterClass(ASSOCIATION_ID);
}

#endif
