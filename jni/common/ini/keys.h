/***************************************************************************
                          keys.h - Key Manipulation Functions

                             -------------------
    begin                : Fri Apr 21 2000
    copyright            : (C) 2000 by Simon White
    email                : s_a_white@email.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _keys_h_
#define _keys_h_

#include <stdio.h>
#include "ini.h"

//*******************************************************************************************************************
// Data structure definitions
//*******************************************************************************************************************
struct section_tag;
struct key_tag;
struct ini_t;

//----------------------------------------------------------------
// data types

#include "inidatatype.h"

// Linked list structure for holding key information.

//keyname =S:keyval
struct key_tag
{
    char           *key;
    long            pos;//all statement start pos in file ;
	size_t			vtlength;//valtype length in file;
    size_t          length;//valtype+val length in file;
	int				valtype;//val type such as "keyname =S:keyval",1. S string 2.I interger 3.B binary 4.F float 5.D double
	char		   *val;//valtype+val
    struct key_tag *pNext;
    struct key_tag *pPrev;

#ifdef INI_USE_HASH_TABLE
    unsigned long   crc;
    struct key_tag *pNext_Acc;
    struct key_tag *pPrev_Acc;
#endif // INI_USE_HASH_TABLE
};

struct key_tag *__ini_addKey    (struct ini_t *ini, char *key);
struct key_tag *__ini_faddKey   (struct ini_t *ini, FILE *file, long pos, size_t length);
struct key_tag *__ini_createKey (struct ini_t *ini, char *key);
void            __ini_deleteKey (struct ini_t *ini);
struct key_tag *__ini_locateKey (struct ini_t *ini, const char *key);
size_t __ini_averageLengthKey   (struct section_tag *current_h);

#endif // _keys_h_
