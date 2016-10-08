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
 * $Id: ffmpeg.h 271 2005-08-09 08:31:35Z picard $
 *
 * The Core Pocket Media Player
 * Copyright (c) 2004-2005 Gabor Kovacs
 *
 ****************************************************************************/

#ifndef __FFMPEG_H
#define __FFMPEG_H

void FFMPEGA_Origin_Init();
void FFMPEGA_Origin_Done();

void FFMPEGV_Origin_Init();
void FFMPEGV_Origin_Done();

void FFMPEGV_Real_Init();
void FFMPEGV_Real_Done();

void FFMPEGA_Real_Init();
void FFMPEGA_Real_Done();

#define RBMP_ID FOURCC('R','B','M','P')
#endif
