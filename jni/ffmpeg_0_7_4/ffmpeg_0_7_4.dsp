# Microsoft Developer Studio Project File - Name="ffmpeg_0_7_4" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ffmpeg_0_7_4 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ffmpeg_0_7_4.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ffmpeg_0_7_4.mak" CFG="ffmpeg_0_7_4 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ffmpeg_0_7_4 - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ffmpeg_0_7_4 - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ffmpeg_0_7_4 - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ffmpeg_0_7_4_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /GX /Zd /O2 /I ".\\" /I "libavcodec" /I "libavutil" /D "NDEBUG" /D "TCPMP" /D "EMULATE_INTTYPES" /D inline=__inline /D "_USRDLL" /D "FFMPEG_0_7_4_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40e /d "NDEBUG"
# ADD RSC /l 0x40e /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /map /machine:I386 /out:"../release/ffmpeg_0_7_4.plg"
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "ffmpeg_0_7_4 - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "FFMPEG_0_7_4_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gm /GX /ZI /Od /I ".\\" /I "libavcodec" /I "libavutil" /D "_DEBUG" /D "TCPMP" /D "EMULATE_INTTYPES" /D inline=__inline /D "_USRDLL" /D "FFMPEG_0_7_4_EXPORTS" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40e /d "_DEBUG"
# ADD RSC /l 0x40e /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../debug/ffmpeg_0_7_4.plg" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ffmpeg_0_7_4 - Win32 Release"
# Name "ffmpeg_0_7_4 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ffmpega.c
# End Source File
# Begin Source File

SOURCE=.\ffmpegv.c
# End Source File
# Begin Source File

SOURCE=.\stdafx.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ffmpeg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "libavcodec"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libavcodec\atrac3.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\atrac3data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\avcodec.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\avs.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\bitstream.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\bitstream.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cabac.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cabac.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cavs.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cavs.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cavsdata.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cavsdec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cavsdsp.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\celp_filters.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\celp_filters.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\celp_math.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\celp_math.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cinepak.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cook.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\cookdata.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\dsputil.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\dsputil.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\error_resilience.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\faanidct.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\faanidct.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\fft.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\golomb.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h261.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h261.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h261data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h261dec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h263.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h263.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h263_parser.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h263_parser.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h263data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h263dec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264_parser.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264_parser.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264idct.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264pred.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\h264pred.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\huffman.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\huffman.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\imgconvert.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\imgconvert.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\indeo2.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\indeo2data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\indeo3.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\indeo3data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\intrax8.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\intrax8.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\intrax8dsp.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\intrax8huf.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\jrevdct.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mdct.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg12.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg12.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg12data.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg12data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg12decdata.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg4video_parser.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpeg4video_parser.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpegvideo.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpegvideo.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\mpegvideo_common.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msmpeg4.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msmpeg4.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msmpeg4data.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msmpeg4data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msrle.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msrledec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msrledec.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\msvideo1.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\nellymoser.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\nellymoser.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\nellymoserdec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\parser.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\parser.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\ra144.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\ra144.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\ra288.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\ra288.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rangecoder.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rangecoder.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv10.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv30.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv30data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv30dsp.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv34.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv34.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv34data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv34vlc.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv40.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv40data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv40dsp.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\rv40vlc2.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\simple_idct.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\simple_idct.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\svq1.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\svq1.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\svq1_cb.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\svq1_vlc.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\svq1dec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\tscc.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\utils.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vc1.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vc1.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vc1acdata.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vc1data.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vc1data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vc1dsp.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp3.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp3data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp3dsp.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp5.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp56.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp56.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp56data.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp56data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp5data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp6.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\vp6data.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wma.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wma.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wmadata.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wmadec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wmv2.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wmv2.h
# End Source File
# Begin Source File

SOURCE=.\libavcodec\wmv2dec.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\xiph.c
# End Source File
# Begin Source File

SOURCE=.\libavcodec\xiph.h
# End Source File
# End Group
# Begin Group "libavutil"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libavutil\avutil.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\common.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\log.c
# End Source File
# Begin Source File

SOURCE=.\libavutil\log.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\mathematics.c
# End Source File
# Begin Source File

SOURCE=.\libavutil\mathematics.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\mem.c
# End Source File
# Begin Source File

SOURCE=.\libavutil\mem.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\random.c
# End Source File
# Begin Source File

SOURCE=.\libavutil\random.h
# End Source File
# Begin Source File

SOURCE=.\libavutil\rational.c
# End Source File
# Begin Source File

SOURCE=.\libavutil\rational.h
# End Source File
# End Group
# End Target
# End Project
