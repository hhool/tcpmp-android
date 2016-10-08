/***************************************************************************
                          ini.h  -  Ini database definition

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

#ifndef _ini_h_
#define _ini_h_
#if !defined(INI_USE_HASH_TABLE)
#define INI_USE_HASH_TABLE
#endif

#include "libini.h"
#include "keys.h"
#include "headings.h"
#ifdef INI_ADD_LIST_SUPPORT
#   include "list.h"
#endif

#define  SAFE_TAIL 256

typedef enum {INI_NEW, INI_EXIST, INI_READ} ini_mode_t;

enum
{
    INI_NONE     = 0,
    INI_MODIFIED = 1 << 0,
    INI_NEWFILE  = 1 << 1,
    INI_BACKUP   = 1 << 2, // Create backup file
    INI_CASE     = 1 << 3  // Case sensitive
};
struct section_tag;
struct key_tag;
// Database containing all information about an ini file.
typedef struct ini_t
{
    char      *filename;
    FILE      *ftmp;   // Temporary work file
    ini_mode_t mode;   // Access mode
    int        flags;

    struct section_tag *first;
    struct section_tag *last;
    struct section_tag *selected;
    char  *heading; // Last written section in tmp file.
    struct section_tag *tmpSection;
    struct key_tag*     tmpKey;

#ifdef INI_USE_HASH_TABLE
    struct section_tag *sections[256];
#endif

#ifdef INI_ADD_LIST_SUPPORT
    // Rev 1.1 Added - New for list accessing
    char        *list;         // Accelerator for accessing same list (When all list read, will be freed)
    char        *listDelims;   // list sperators
    char        *listIndexPtr; // current element we wish to access   (will auto increment)
    unsigned int listLength;
    unsigned int listIndex;
#endif // INI_ADD_LIST_SUPPORT
} ini_t;

void                __ini_strtrim         (char *str);
#ifdef INI_USE_HASH_TABLE
unsigned long       __ini_createCrc32     (const char *str, bool_t strcase);
#endif

#endif // _ini_h_
