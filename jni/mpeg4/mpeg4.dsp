# Microsoft Developer Studio Project File - Name="mpeg4" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mpeg4 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mpeg4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mpeg4.mak" CFG="mpeg4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mpeg4 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mpeg4 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mpeg4 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MPEG4_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /GX /Zd /O2 /D "NDEBUG" /D "_USRDLL" /D "MPEG4_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40e /d "NDEBUG"
# ADD RSC /l 0x40e /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /debug /machine:I386 /out:"../release/mpeg4.plg"

!ELSEIF  "$(CFG)" == "mpeg4 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MPEG4_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /Gm /GX /ZI /Od /D "_DEBUG" /D "_USRDLL" /D "MPEG4_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40e /d "_DEBUG"
# ADD RSC /l 0x40e /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../debug/mpeg4.plg" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "mpeg4 - Win32 Release"
# Name "mpeg4 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\divx3_data.c
# End Source File
# Begin Source File

SOURCE=.\divx3_header.c
# End Source File
# Begin Source File

SOURCE=.\divx3_mblock.c
# End Source File
# Begin Source File

SOURCE=.\divx3_vlc.c

!IF  "$(CFG)" == "mpeg4 - Win32 Release"

!ELSEIF  "$(CFG)" == "mpeg4 - Win32 Debug"

# SUBTRACT CPP /WX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\h263_header.c
# End Source File
# Begin Source File

SOURCE=.\h263_mblock.c
# End Source File
# Begin Source File

SOURCE=.\h263_vld.c
# End Source File
# Begin Source File

SOURCE=.\mp4_decode.c
# End Source File
# Begin Source File

SOURCE=.\mp4_header.c
# End Source File
# Begin Source File

SOURCE=.\mp4_mblock.c
# End Source File
# Begin Source File

SOURCE=.\mp4_mv.c
# End Source File
# Begin Source File

SOURCE=.\mp4_predict.c
# End Source File
# Begin Source File

SOURCE=.\mp4_stream.c
# End Source File
# Begin Source File

SOURCE=.\mp4_vld_h263.c
# End Source File
# Begin Source File

SOURCE=.\mp4_vld_mpeg.c
# End Source File
# Begin Source File

SOURCE=.\StdAfx.c
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\divx3_vlc.h
# End Source File
# Begin Source File

SOURCE=.\mp4_decode.h
# End Source File
# Begin Source File

SOURCE=.\mp4_header.h
# End Source File
# Begin Source File

SOURCE=.\mp4_mblock.h
# End Source File
# Begin Source File

SOURCE=.\mp4_mv.h
# End Source File
# Begin Source File

SOURCE=.\mp4_predict.h
# End Source File
# Begin Source File

SOURCE=.\mp4_stream.h
# End Source File
# Begin Source File

SOURCE=.\mp4_vld.h
# End Source File
# Begin Source File

SOURCE=.\mpeg4.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# End Group
# End Target
# End Project
