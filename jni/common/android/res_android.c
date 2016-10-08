#include "../common.h"

#if defined(TARGET_ANDROID) && defined(RES_IN_COMMOM)

typedef struct Language
{
	int Fourcc;
	tchar_t szText[4096];
}Language;

static const Language Lang[]=
{
		{LANG_STD,T(";CODEPAGE=1252\r\n\
					\r\n\
					[DEFAULT]\r\n\
					\r\n\
					; language names\r\n\
					LANGAR=Arabic\r\n\
					LANGBG=Bulgarian\r\n\
					LANGCA=Catal?\r\n\
					LANGCHS=Chinese S.\r\n\
					LANGCHT=Chinese T.\r\n\
					LANGCS=Cesky\r\n\
					LANGDA=Danish\r\n\
					LANGDE=Deutsch\r\n\
					LANGEN=English\r\n\
					LANGES=Espa駉l\r\n\
					LANGFR=Fran鏰is\r\n")},
		{LANG_STD,T("\
					LANGHU=Magyar\r\n\
					LANGIT=Italiano\r\n\
					LANGJA=Japanese\r\n\
					LANGKO=Korean\r\n\
					LANGNL=Dutch\r\n\
					LANGPTB=Portugu阺 (Brazil)\r\n\
					LANGPL=Polish\r\n\
					LANGRU=Russian\r\n\
					LANGSV=Swedish\r\n\
					LANGTR=T黵k鏴\r\n\
					LANGHE=Hebrew\r\n\
					LANGUA=Ukrainian\r\n")},
		{LANG_STD,T("\
					\r\n\
					; strings without language (or only internally used)\r\n\
					; no translation needed\r\n\
					\r\n\
					ABOU00CE= - Ogg Vorbis Tremor\\n - ffmpeg (parts)\\n - liba52\\n - Matroska Library Parser\\n - zlib\\n - Peal\\n - libmusepack\\n - libmikmod\\n - Mobile Stream Rot. headers\\n - Intel 2700G SDK headers\\n - CoreTheque\r\\n\
					ABOU00D0= - Picard da man!\\n - ffmpeg\\n - PocketMVP aka Marc Dukette\\n - XviD\\n - The Magic Lantern\r\n\
					DRAW0000=DirectDraw\r\n")},
		{LANG_STD,T("\
					DIRC0000=Direct\r\n\
					FLYT0000=FlyPresenter\r\n\
					GAPI0000=GAPI\r\n\
					GDI_0000=GDI\r\n\
					EQUA0181=60 Hz\r\n\
					EQUA0182=170 Hz\r\n\
					EQUA0183=310 Hz\r\n\
					EQUA0184=600 Hz\r\n")},
		{LANG_STD,T("\
					EQUA0185=1 Khz\r\n\
					EQUA0186=3 Khz\r\n\
					EQUA0187=6 Khz\r\n\
					EQUA0188=12 Khz\r\n\
					EQUA0189=14 Khz\r\n")},
		{LANG_STD,T("\
					EQUA018A=16 Khz\r\n\
					\r\n\
					AOUT0056=Input\r\n\
					AOUT0057=Output\r\n\
					\r\n\
					AOUT0051=Volume\r\n\
					AOUT0052=Mute\r\n\
					AOUT0055=Swap Left/Right\r\n\
					AOUT0053=Quality\r\n\
					AOUT0058=Background\r\n")},
		{LANG_STD,T("\
					FLOW0015=Buffered\r\n\
					OUTP0020=Input\r\n\
					OUTP0021=Output\r\n\
					OUTP0022=Total\r\n\
					OUTP0023=Dropped\r\n\
					FMT_0032=Input\r\n\
					FMT_0036=Filepos\r\n\
					FMT_004B=AutoReadSize\r\n\
					FMT_004E=Timing\r\n\
					FMT_0045=FindSubtitles\r\n")},
		{LANG_STD,T("\
					FMT_0047=GlobalComment\r\n\
					FMT_0048=StreamCount\r\n\
					IDCT0050=Rounding\r\n\
					IDCT0051=Format\r\n\
					IDCT0052=Output\r\n\
					IDCT0053=BufferCount\r\n\
					IDCT0057=Width\r\n\
					IDCT0058=Height\r\n\
					IDCT0056=Show\r\n\
					IDCT005A=Shift\r\n\
					CCID0100=Input\r\n\
					CCID0101=IDCT\r\n\
					STRM0090=Mime\r\n\
					STRM0092=Length\r\n\
					TIMR0060=Time\r\n\
					TIMR0062=Speed\r\n")},
		{LANG_STD,T("\
					TIMR0063=Play\r\n\
					AHI_0100=Name\r\n\
					AHI_0101=Version\r\n\
					AHI_0102=Revision \r\n\
					AHI_0103=Chip Id\r\n\
					AHI_0104=Revision Id\r\n\
					AHI_0105=Total Memory\r\n\
					AHI_0106=Internal Total Mem\r\n\
					AHI_0107=External Total Mem\r\n\
					AHI_010E=Internal Mem Block\r\n\
					AHI_010F=External Mem Block\r\n\
					AHI_0108=Caps1\r\n\
					AHI_0109=Caps2\r\n")},
		{LANG_STD,T("\
					AHI_010A=Caps3\r\n\
					AHI_010B=Caps4\r\n\
					AHI_010C=Unmap\r\n\
					GAPI0100=Width\r\n\
					GAPI0101=Height\r\n\
					GAPI0102=PitchX\r\n\
					GAPI0103=PitchY\r\n\
					GAPI0104=BPP\r\n\
					GAPI0105=Format\r\n\
					GAPI0106=Pointer\r\n")},
		{LANG_STD,T("\
					GAPI0107=DRAM\r\n\
					RAWD0100=Width\r\n\
					RAWD0101=Height\r\n\
					RAWD0102=PitchX\r\n\
					RAWD0103=PitchY\r\n\
					RAWD0104=BPP\r\n\
					RAWD0105=Format\r\n\
					RAWD0106=Pointer\r\n")},
		{LANG_STD,T("\
					VOUT0070=Primary\r\n\
					VOUT007E=Orientation\r\n\
					VOUT007D=IDCT\r\n\
					VOUT0075=FX\r\n\
					VOUT0076=Wnd\r\n\
					VOUT0078=Viewport\r\n\
					VOUT0079=OutputRect\r\n")},
		{LANG_STD,T("\
					VOUT0077=Visible\r\n\
					VOUT007F=Clipping\r\n\
					VOUT007A=ColorKey\r\n\
					VOUT007B=Dirty\r\n\
					PLAY0033=Equalizer\r\n")},
		{LANG_STD,T("\
					PLAY0069=EqEnabled\r\n\
					PLAY003A=Wnd\r\n\
					PLAY002E=PlayList Count\r\n\
					PLAY002F=PlayList Current\r\n\
					PLAY0039=FullScreen Dir.\r\n\
					PLAY0045=Skin Dir.\r\n\
					PLAY0055=Benchmark Time\r\n\
					PLAT001A=TypeNo\r\n\
					PLAT0019=Ver\r\n\
					PLAY0031=Media Comment\r\n\
					PLAY002A=Input\r\n\
					PLAY002B=Format\r\n\
					PLAY0077=Audio Stream\r\n\
					PLAY0076=Video Stream\r\n")},
		{LANG_STD,T("\
					PLAY003C=Skin Viewport\r\n\
					PLAY003B=Clipping\r\n\
					PLAY0050=Visible\r\n\
					PLAY0025=Percent\r\n\
					PLAY0032=Play\r\n\
					PLAY0049=Fast Forward\r\n\
					PLAY0028=Position\r\n\
					PLAY0046=Duration\r\n\
					PLAY0029=Title\r\n\
					PLAY003E=Fullscreen\r\n\
					PLAY0074=Stereo\r\n\
					PLAY00BE=Aspect\r\n\
					OPEN0102=Add\r\n\
					OPEN0101=Multiple\r\n\
					OPEN0103=FileType\r\n\
					OPEN0104=Path\r\n\
					OPEN0105=SortCol\r\n")},
		{LANG_STD,T("\
					OPEN0106=SortDir\r\n\
					OPEN0107=WidthName\r\n\
					OPEN0108=WidthType\r\n\
					OPEN0109=WidthSize\r\n\
					SYST0000=System Timer\r\n\
					SIDC0000=Software IDCT\r\n\
					INTF0139=&50%\r\n\
					INTF013A=&100%\r\n\
					INTF018E=15&0%\r\n\
					INTF013B=&200%\r\n\
					INTF00DE=10&%\r\n\
					INTF014D=25&%\r\n\
					INTF014E=&50%\r\n\
					INTF014F=&100%\r\n\
					INTF00CE=15&0%\r\n\
					INTF0154=&200%\r\n")},
		{LANG_STD,T("\
					INTF0155=&300%\r\n\
					INTF0230=&80%\r\n\
					INTF0231=&90%\r\n\
					INTF0232=&110%\r\n\
					INTF0233=120&%\r\n\
					PLAT0013=Caps\r\n\
					PLAT0014=ICache\r\n\
					PLAT0015=DCache\r\n")},
		{LANG_STD,T("\
					\r\n\
					DIVX0000=CoreASP\r\n\
					MP3D0000=CoreMP3\r\n\
					AVC_0000=CoreAVC\r\n\
					\r\n\
					00024400=MS ADPCM\r\n\
					00064400=a-Law\r\n\
					00074400=u-Law\r\n\
					00114400=IMA ADPCM\r\n\
					00454400=G.726\r\n\
					00454400=G.726\r\n")},
		{LANG_STD,T("\
					00574400=GSM AMR-NB\r\n\
					00584400=GSM AMR-WB\r\n\
					01604400=Windows Media Audio\r\n\
					01614400=Windows Media Audio\r\n\
					01624400=Windows Media Audio 9 Professional\r\n\
					01634400=Windows Media Audio 9 Lossless\r\n\
					000A4400=Windows Media Audio Voice\r\n")},
		{LANG_STD,T("\
					20004400=AC-3\r\n\
					04504400=QDesign Music 2\r\n\
					CC004400=Qualcomm QCELP\r\n\
					CD004400=Speex speach decoder\r\n\
					AAC04400=MPEG4 AAC Audio\r\n\
					00504400=MP3\r\n\
					00554400=MP3\r\n\
					01304400=Sipro Lab Telecom ACELP.net\r\n\
					77A14400=The True Audio\r\n")},
		{LANG_STD,T("\
					10104400=RealAudio 6\r\n\
					10114400=RealAudio 8\r\n\
					10124400=RealAudio 1\r\n\
					10134400=RealAudio 2\r\n\
					10144400=RealAudio 4/5\r\n\
					10204400=Nellymoser Audio\r\n\
					10214400=ADPCM Shockwave Flash\r\n")},
		{LANG_STD,T("\
					\r\n\
					CRAM4400=Microsoft Video 1\r\n\
					MSVC4400=Microsoft Video 1\r\n\
					CVID4400=Cinepak\r\n\
					MPG44400=Microsoft MPEG-4 Version 1\r\n\
					MP424400=Microsoft MPEG-4 Version 2\r\n\
					MP434400=Microsoft MPEG-4 Version 3\r\n\
					DIV34400=Microsoft MPEG-4 Version 3\r\n\
					IV304400=Intel Indeo 3.0\r\n")},
		{LANG_STD,T("\
					IV314400=Intel Indeo 3.1\r\n\
					IV404400=Intel Indeo 4.0\r\n\
					IV504400=Intel Indeo 5.0\r\n\
					VP304400=On2 VP3\r\n\
					VP314400=On2 VP3\r\n\
					VP604400=On2 VP6\r\n\
					VP614400=On2 VP6.1\r\n\
					VP624400=On2 VP6.2\r\n\
					VP704400=On2 VP7\r\n\
					SVQ14400=Sorenson Video 1\r\n\
					SVQ34400=Sorenson Video 3\r\n")},
		{LANG_STD,T("\
					AVC14400=AVC aka H.264\r\n\
					H2644400=AVC aka H.264\r\n\
					WMV14400=Windows Media Video 7\r\n\
					WMV24400=Windows Media Video 8\r\n\
					WMV34400=Windows Media Video 9\r\n\
					WMVP4400=Windows Media Video 9\r\n\
					WMVA4400=Windows Media Video 9 Advanced profile\r\n\
					WVP24400=Windows Media Video 9 Advanced profile\r\n\
					MSS24400=Windows Media Video 9 Screen\r\n\
					MP4V4400=MPEG-4 Video\r\n")},
		{LANG_STD,T("\
					DX504400=MPEG-4 Video\r\n\
					XVID4400=MPEG-4 Video\r\n\
					H2634400=H.263\r\n\
					S2634400=H.263\r\n\
					RV104400=RealVideo codec\r\n\
					RV134400=RealVideo codec\r\n\
					RV204400=RealVideo G2\r\n\
					RV304400=RealVideo 8\r\n\
					RV404400=RealVideo 9\r\n\
					TSCC4400=TechSmith Screen Capture Codec\r\n")},
		{LANG_STD,T("\
					\r\n\
					;-----------------\r\n\
					; file formats\r\n\
					\r\n\
					WJPG0003=and(eq(le16,0x2D2D),scan(64,stri('image/jpeg'),0,fwd))\r\n\
					RAVC0002=264:V;h264:V\r\n\
					RJPG0001=image/jpeg\r\n\
					RJPG0002=jpg:V;jpeg:V\r\n\
					RJPG0003=eq(mask(be32,~0xFF),0xFFD8FF00)\r\n")},
		{LANG_STD,T("\
					RJPG0200=vcodec/mjpg\r\n\
					RTIF0002=tif:V;tiff:V\r\n\
					RTIF0200=vcodec/tiff\r\n\
					RPNG0001=image/png\r\n\
					RPNG0002=png:V\r\n\
					RPNG0003=and(eq(be32,0x89504E47),eq(be32(4),0x0D0A1A0A))\r\n\
					RPNG0200=vcodec/png_\r\n\
					MIKM0002=xm:A;s3m:A;mod:A;stm:A;it:A;ult:A;imf:A;amf:A;669:A\r\n")},
		{LANG_STD,T("\
					MPCF0002=mpc:A\r\n\
					RSKN0002=xml:S\r\n\
					RNSV0002=nsv:V\r\n\
					RNSV0001=video/nsv\r\n\
					RNSV0003=or(eq(le32,'NSVf'),eq(le32,'NSVs'))\r\n\
					RAVI0002=avi:V;divx:V;gvi:V\r\n\
					RAVI0003=and(eq(le32,'RIFF'),or(eq(le32(8),'AVI '),eq(le32(8),'AVIX')))\r\n")},
		{LANG_STD,T("\
					RASF0002=asf:V\r\n\
					RASF0003=and(eq(le32,0x75B22630),eq(le32(4),0x11CF668E),eq(le32(8),0xAA00D9A6),eq(le32(12),0x6CCE6200))\r\n\
					RASH0001=application/vnd.ms.wms-hdr.asfv1,application/x-mms-framed\r\n\
					RASH0003=scan(4096,and(eq(le16,0x4824),eq(le32(12),0x75B22630),eq(le32(16),0x11CF668E),eq(le32(20),0xAA00D9A6),eq(le32(24),0x6CCE6200)),!eq(le16,0x4D24),fwd(add(4,le16(2))))\r\n\
					WMVF0002=wm:V;wmv:V\r\n")},
		{LANG_STD,T("\
					WMAF0002=wma:A\r\n\
					RWAV0002=wav:A;rmp:A\r\n\
					RWAV0003=and(eq(le32(8),'WAVE'),or(eq(le32,'RIFF'),eq(le32,'RMP3'),eq(le32,'SDSS')))\r\n\
					RAIF0002=aif:A;aiff:A\r\n\
					RAIF0003=and(eq(le32,'FORM'),eq(le32(8),'AIFF'))\r\n\
					MATR0002=mkv:V;mka:A;cipc:V\r\n")},
		{LANG_STD,T("\
					MATR0003=and(eq(le32,0xA3DF451A),scan(64,and(eq(le32,'matr'),eq(le32(4),'oska')),0,fwd))\r\n\
					WEBM0002=webm:V\r\n\
					WEBM0003=and(eq(le32,0xA3DF451A),scan(64,eq(le32,'webm'),0,fwd))\r\n\
					RASX0002=asx:P;wmx:P;wmweb:P;wmwebasf:P;wmwebasx:P;wvx:V;wax:A\r\n\
					RASX0003=and(text,scan(64,stri('<ASX'),0,fwd))\r\n\
					M3U_0002=m3u:P\r\n\
					M3U_0001=audio/x-mpegurl,audio/mpegurl\r\n\
					M3U_0003=and(text,gt(512,length),or(eq(lines,1),eq(lines,0)))\r\n")},
		{LANG_STD,T("\
					PLS_0002=pls:P\r\n\
					PLS_0001=audio/x-scpls,audio/scpls\r\n\
					PLS_0003=and(text,scan(64,stri('[playlist]'),0,fwd))\r\n\
					B4S_0002=b4s:P\r\n\
					MPEG0002=mpeg:V;mpg:V;mpv:V\r\n\
					MPEG0003=or(and(eq(le32,'RIFF'),eq(le32(8),'CDXA')),scan(20,or(eq(be32,0x1BA),eq(be32,0x1BB),eq(be32,0x1BC),eq(be32,0x1BD),eq(be32,0x1BE),eq(be32,0x1BF),eq(mask(be32,~0xF),0x1E0),eq(mask(be32,~0xF),0x1C0)),eq(mask(be32,~0xFF),0x100),fwd))\r\n\
					MVES0002=m1v:V;m4v:V\r\n\
					MVES0003=or(eq(mask(be32,~0x3FF),0x8000),scan(20,or(eq(be32,0x1B3),eq(be32,0x1B0)),eq(mask(be32,~0xFF),0x100),fwd))\r\n\
					MP4F0002=mp4:V;3gp:V;3g2:V;k3g:V;m4a:A;m4b:A;m4v:V;mov:V;mqv:V\r\n\
					MP4F0003=scan(4096,or(eq(le32(4),'moov'),eq(le32(4),'wide'),eq(le32(4),'free'),eq(le32(4),'mdat'),eq(le32(4),'pnot'),eq(le32(4),'prfl'),eq(le32(4),'udta')),!or(eq(le32(4),'ftyp'),eq(le32(4),'skip')),fwd(be32))\r\n\
					AAC_0002=aac:A\r\n\
					AAC_0200=acodec/0xaac0\r\n")},
		{LANG_STD,T("\
					AAC_0001=audio/aac,audio/aacp\r\n\
					MP3_0001=audio/mpeg\r\n\
					MP3_0002=mp1:A;mp2:A;mp3:A;mpa:A\r\n\
					MP3_0200=acodec/0x0055\r\n\
					AC3_0002=ac3:A\r\n\
					AC3_0200=acodec/0x2000\r\n\
					OGGV0001=application/ogg\r\n")},
		{LANG_STD,T("\
					OGGV0002=ogg:A;ogm:V;ogv:V\r\n\
					OGGV0003=eq(le32,'OggS')\r\n\
					OGHV0001=application/ogg\r\n\
					OGHV0002=ogg:A;ogm:V;ogv:V\r\n\
					OGHV0003=eq(le32,'OggS')\r\n\
					SPXL0002=spx:A;speex:A\r\n\
					SPXH0002=spx:A;speex:A\r\n")},
		{LANG_STD,T("\
					AMFN0002=amr:A\r\n\
					AMFN0200=acodec/0x0057\r\n\
					AMFW0002=awb:A\r\n\
					AMFW0200=acodec/0x0058\r\n\
					COSP0002=cos:V;cosp:V\r\n\
					FLAC0002=flac:A;fla:A\r\n\
					WVPF0002=wv:A\r\n\
					TTAF0002=tta:A\r\n\
					ASAP0002=sap:A;cmc:A;cmr:A;dmc:A;mpt:A;mpd:A;rmt:A;tmc:A;tm8:A;tm2:A\r\n")},
		{LANG_STD,T("\
					\r\n\
					;---------------\r\n\
					; codecs\r\n\
					\r\n\
					HTTP0001=http,https\r\n\
					MMS_0001=mms\r\n\
					FILE0001=file\r\n\
					FIDB0001=mem\r\n\
					VFS_0001=pose,slot1,slot2,slot3,slot4,slot5,slot6,slot7,slot8,simu1,simu2,simu3,simu4,simu5,simu6,simu7,simu8,vol1,vol2,vol3,vol4,vol5,vol6,vol7,vol8\r\n\
					\r\n\
					TIFF0001=vcodec/tiff\r\n\
					PNG_0001=vcodec/png_\r\n\
					MJPG0001=vcodec/mjpg,vcodec/ljpg,vcodec/jpgl,vcodec/jpeg,vcodec/avrn,vcodec/mjpa,vcodec/mjpb\r\n")},
		{LANG_STD,T("\
					MPG10001=vcodec/mpeg,vcodec/mpg1\r\n\
					SPEX0001=acodec/0xCD00\r\n\
					ADMS0001=acodec/0x0002\r\n\
					ADIM0001=acodec/0x0011\r\n\
					ADIQ0001=acodec/0x10011\r\n\
					G7260001=acodec/0x0045\r\n\
					ALAW0001=acodec/0x0006\r\n\
					ULAW0001=acodec/0x0007\r\n\
					A52_0001=acodec/0x2000\r\n\
					MP3D0001=acodec/0x0050,acodec/0x0055\r\n")},
		{LANG_STD,T("\
					LMAD0001=acodec/0x0050,acodec/0x0055\r\n\
					FAAD0001=acodec/0xAAC0,acodec/0x00FF\r\n\
					AACF0001=acodec/0xAAC0,acodec/0x00FF\r\n\
					WMV_0001=vcodec/mp43,vcodec/mp42,vcodec/mpeg3,vcodec/wmv1,vcodec/wmv2,vcodec/wmv3,vcodec/wmvp,vcodec/wvp2,vcodec/wmva,vcodec/mss2\r\n\
					WMV_0200=wmvdmod.dll\r\n\
					WMV_0201=wmvdecoder.dll\r\n\
					WMV_0202=82d353df-90bd-4382-8bc2-3f6192b76e34\r\n")},
		{LANG_STD,T("\
					WMVA0001=vcodec/wvp2,vcodec/wmva\r\n\
					WMVA0200=wmvadvd.dll\r\n\
					WMVA0202=03BE3AC4-84B7-4E0E-A78D-D3524E60395A\r\n\
					WMA_0001=acodec/0x0160,acodec/0x0161,acodec/0x0162,acodec/0x0163,acodec/0x000A\r\n\
					WMA_0200=wmadmod.dll\r\n\
					WMA_0201=wmadecoder.dll\r\n\
					WMA_0202=2eeb4adf-4578-4d10-bca7-bb955f56320a\r\n")},
		{LANG_STD,T("\
					WMAV0001=acodec/0x000A\r\n\
					WMAV0200=wmspdmod.dll\r\n\
					WMAV0201=wmavdecoder.dll\r\n\
					WMAV0202=874131cb-4ecc-443b-8948-746b89595d20\r\n\
					WMS_0001=vcodec/mss2\r\n\
					WMS_0200=wmsdmod.dll\r\n\
					WMS_0201=\r\n\
					WMS_0202=7BAFB3B1-D8F4-4279-9253-27DA423108DE\r\n")},
		{LANG_STD,T("\
					MP410001=vcodec/mpg4,vcodec/mp41,vcodec/div1\r\n\
					MP420001=vcodec/mp42,vcodec/div2\r\n\
					MP430001=vcodec/mp43,vcodec/mpg3,vcodec/div3,vcodec/div4,vcodec/div5,vcodec/div6,vcodec/ap41,vcodec/col0,vcodec/col1,vcodec/3ivd\r\n\
					DIVX0001=vcodec/divx,vcodec/dx50,vcodec/xvid,vcodec/mrs2,vcodec/3iv2,vcodec/mp4s,vcodec/pm4v,vcodec/blz0,vcodec/ump4,vcodec/m4s2,vcodec/mp4v,vcodec/fmp4,vcodec/em4a,vcodec/sedg\r\n\
					H2630001=vcodec/h263,vcodec/u263,vcodec/x263,vcodec/s263\r\n\
					I2630001=vcodec/i263\r\n\
					VORB0001=acodec/0x3674F\r\n")},
		{LANG_STD,T("\
					VORV0001=acodec/0x1674F\r\n\
					VOHB0001=acodec/0x3674F\r\n\
					VOHV0001=acodec/0x1674F\r\n\
					OGGP0001=acodec/0x2674F\r\n\
					OGGE0001=acodec/0x674F,acodec/0x676F,acodec/0x6750,acodec/0x6770,acodec/0x6751,acodec/0x6771\r\n")},
		{LANG_STD,T("\
					OGHP0001=acodec/0x2674F\r\n\
					OGHE0001=acodec/0x674F,acodec/0x676F,acodec/0x6750,acodec/0x6770,acodec/0x6751,acodec/0x6771\r\n\
					AMRN0001=acodec/0x0057\r\n\
					AMRW0001=acodec/0x0058\r\n\
					AVC_0001=vcodec/avc1,vcodec/h264,vcodec/x264,vcodec/vssh\r\n\
					COPC0001=vcodec/copc\r\n\
					CIPC0001=vcodec/cipc\r\n\
					EQUA0001=acodec/0x0001\r\n")},
		{LANG_EN,T(";CODEPAGE=1252\r\n\
					\r\n\
					[EN]\r\n\
					GE2D0000=Sony GE2D\r\n\
					S1D10000=EPSON Graphics Controller\r\n\
					ATI40000=ATI Overlay\r\n\
					ATII0000=ATI Decoder\r\n\
					OCON0000=Ascii Console\r\n\
					AMRN0000=GSM AMR-NB\r\n\
					AMRW0000=GSM AMR-WB\r\n\
					AMFN0000=GSM AMR-NB files (AMR)\r\n\
					AMFW0000=GSM AMR-WB files (AWB)\r\n")},
		{LANG_EN,T("\
					ASAP0000=Another Slight Atari Player (SAP,CMC,CMR,DMC,MPT,MPD,RMT,TMC,TM8,TM2)\r\n\
					MIKM0000=Module files (XM,S3M,MOD,STM,IT,ULT,IMF,AMF,669)\r\n\
					MPCF0000=Musepack files (MPC)\r\n\
					MP4F0000=MPEG4 files (MP4,MOV,3GP,M4A,M4B,K3G,MQV)\r\n\
					MP4F0200=This type of MPEG4 file is not supported.\r\n\
					MP4F0201=Multiple media formats per one stream is not supported.\r\n\
					AAC_0000=AAC files (AAC)\r\n\
					FAAD0000=LibFAAD LC,HE,PS AAC\r\n\
					FAAD0200=This type of AAC audio stream is not supported. Try installing the AACFull plugin!\r\n")},
		{LANG_EN,T("\
					AACF0000=LibFAAD LC,HE,PS,LD,LTP AAC (with ER)\r\n\
					AACF0200=This type of AAC audio stream is not supported.\r\n\
					A52_0000=LibA52 (AC-3)\r\n\
					AC3_0000=AC-3 Audio files (AC3)\r\n\
					TIFF0000=TIFF\r\n\
					TIFF0200=This type of TIFF image format is not supported.\r\n\
					RTIF0000=TIFF files (TIF,TIFF)\r\n\
					PNG_0000=PNG\r\n\
					PNG_0200=This type of PNG image format is not supported.\r\n")},
		{LANG_EN,T("\
					RPNG0000=PNG files (PNG)\r\n\
					FBMP0000=BMP\r\n\
					RBMP0000=BMP files (BMP)\r\n\
					RJPG0000=JPEG files (JPG,JPEG)\r\n\
					WJPG0000=WEB MJPEG stream\r\n\
					MJPG0000=MJPEG\r\n\
					MJPG0200=This type of motion-JPEG format is not supported.\r\n\
					MJPG0201=Progressive JPEG format is not supported yet.\r\n\
					ADMS0000=MS ADPCM\r\n\
					ADIM0000=IMA ADPCM\r\n\
					ADIQ0000=IMA QuickTime ADPCM\r\n\
					G7260000=G.726 ADPCM\r\n\
					RASX0000=Windows metafiles (ASX,WMX,WVX,WAX)\r\n")},
		{LANG_EN,T("\
					M3U_0000=M3U playlist files (M3U)\r\n\
					PLS_0000=PLS playlist files (PLS)\r\n\
					B4S_0000=B4S playlist files (B4S)\r\n\
					MATR0000=Matroska files (MKV,MKA)\r\n\
					WEBM0000=WEBM files (WEBM)\r\n\
					RSKN0000=Skin files (XML)\r\n\
					RNSV0000=NSV files (NSV)\r\n\
					RAVI0000=Video files (AVI,DIVX,GVI)\r\n\
					RAVI0100=Index corrupt. No seeking possible!\r\n")},
		{LANG_EN,T("\
					RWAV0000=Windows waveform (WAV,RMP)\r\n\
					RAIF0000=Audio Interchange File Format (AIF)\r\n\
					RASF0000=ASF media files (ASF)\r\n\
					RASH0000=ASF HTTP stream\r\n\
					WMAF0000=Windows Media Audio files (WMA)\r\n\
					WMVF0000=Windows Media Video files (WM,WMV)\r\n\
					WMV_0000=Windows Media Video\r\n\
					WMVA0000=Windows Media Video Advanced profile\r\n")},
		{LANG_EN,T("\
					WMA_0000=Windows Media Audio\r\n\
					WMAV0000=Windows Media Audio Voice\r\n\
					MP430000=MS-MPEG4\r\n\
					I2630000=Intel H.263\r\n\
					H2630000=H.263\r\n\
					AVC_0201=Lossless AVC not supported!\r\n\
					AVC_0202=This type of AVC file is not supported.\r\n")},
		{LANG_EN,T("\
					AVC_0203=Interlaced video is not supported!\r\n\
					AVC_0204=FMO feature is not supported!\r\n\
					AVC_0206=Maximum %d pixel width video supported!\r\n\
					RAVC0000=AVC elementary stream (H264)\r\n\
					DIVX0200=Quarter Pixel feature is not supported!\r\n\
					DIVX0201=Interlaced video is not supported!\r\n\
					DIVX0202=GMC feature is not supported!\r\n\
					DIVX0204=This type of H.263 video is not supported!\r\n\
					DIVX0205=Data partitioning feature is not supported!\r\n")},
		{LANG_EN,T("\
					VOUT0100=Maximum %d x %d video size is supported!\r\n\
					MPEG0000=MPEG Movie files (MPEG,MPG,MPV)\r\n\
					MPEG0200=Encrypted MPEG-2 streams are not supported!\r\n\
					MVES0000=MPEG Video elementary stream (M1V,M4V)\r\n\
					MPG10000=MPEG Video\r\n\
					MPG10201=MPEG-2 video is not supported!\r\n\
					MP3_0000=MPEG Audio files (MP1,MP2,MP3,MPA)\r\n")},
		{LANG_EN,T("\
					LMAD0000=LibMad MPEG Audio\r\n\
					VORB0000=Vorbis Audio\r\n\
					VORV0000=Vorbis Audio\r\n\
					OGGE0000=Vorbis Audio (Embedded)\r\n\
					OGGP0000=Vorbis Audio (Packed)\r\n\
					OGGV0000=Ogg Vorbis files (OGG,OGM,OGV))\r\n\
					VOHB0000=Vorbis Audio\r\n\
					VOHV0000=Vorbis Audio\r\n\
					OGHE0000=Vorbis Audio (Embedded)\r\n\
					OGHP0000=Vorbis Audio (Packed)\r\n\
					OGHV0000=Ogg Vorbis files (OGG,OGM,OGV))\r\n")},
		{LANG_EN,T("\
					SPXL0000=Speex files (SPX)\r\n\
					SPXH0000=Speex files (SPX)\r\n\
					SPEX0000=Speex speech decoder\r\n\
					AHII0000=ATI IMAGEON Decoder\r\n\
					AHI_0000=ATI IMAGEON\r\n\
					AHI_0111=Disable OS Bitmap Caching (Advised!)\r\n")},
		{LANG_EN,T("\
					AHI_0110=\"Green Tint\" bug compensation\r\n\
					AHI_0113=Reuse primary surface if needed\r\n\
					AHI_0114=Optimized primary surface mode\r\n\
					AHI_0115=Scaling LCD tearing compensation\r\n\
					AHI_0116=Keep ATI driver active (just test)\r\n\
					AHI_0117=ATI older driver memory mapping fix\r\n")},
		{LANG_EN,T("\
					AHI_0200=Out of video memory! Retry after soft-reset or choose a different video driver!\r\n\
					AHI_0201=Device open error! Retry after a warm boot (soft-reset)!\r\n\
					AHI_0202=Too old ATI driver (%s). Please update! In most cases the SDIO interface update will contain the new ATI driver. Until then try to using the player with GAPI option\r\n\
					270G0000=Intel 2700G\r\n")},
		{LANG_EN,T("\
					270G0400=Intel 2700G chip is not available, most likely used by another program!\r\n\
					270I0000=Intel 2700G Decoder\r\n\
					270G0200=LCD tearing compensation\r\n")},
		{LANG_EN,T("\
					270G0201=Decoder rotation (faster)\r\n\
					270G0202=Close WMP when resource is locked\r\n\
					GAPI0200=GAPI not supported on this device. Try a different driver!\r\n\
					HIRS0000=Standard display\r\n\
					HIRP0100=Triple buffering (needs more memory)\r\n\
					HIRP0101=Borderless in fullscreen\r\n\
					HIRP0102=Borderless mode doesn't work on all device. In this case you should soft-reset the device and disable this option!\r\n\
					DRAW0100=Overlay with colorkey\r\n\
					DRAW0101=Use blitting instead of overlay\r\n\
					DRAW0102=Use device stretching for blitting\r\n")},
		{LANG_EN,T("\
					DRAW0103=Overlay format\r\n\
					DRAF0100=Auto\r\n\
					DRAF0101=YV12\r\n\
					DRAF0102=YUY2\r\n\
					DRAF0103=RGB\r\n\
					XSC20000=Intel XScale\r\n\
					XSL20000=Intel XScale Low Quality\r\n\
					XSCD0000=Intel XScale\r\n\
					XSCD0100=Use hardware vertical zooming\r\n")},
		{LANG_EN,T("\
					XSCD0101=Use buffer flipping (solves LCD tearing)\r\n\
					XSCD0102=Allocate flipping buffers at startup\r\n\
					RAWD0000=Raw FrameBuffer\r\n\
					HTTP0000=HTTP protocol\r\n\
					MMS_0000=MMS protocol\r\n\
					FILE0000=File\r\n\
					FIDB0000=FileDb\r\n\
					VFS_0000=VFS\r\n\
					STRM0091=URL\r\n\
					WAVE0000=Wave Output\r\n")},
		{LANG_EN,T("\
					NULV0000=Null Video\r\n\
					NULA0000=Null Audio\r\n\
					ALAW0000=a-Law\r\n\
					ULAW0000=u-Law\r\n\
					FMT_0033=Duration\r\n\
					FMT_0044=Filesize\r\n\
					PLAY0000=Player\r\n\
					PLAQ0100=Low\r\n\
					PLAQ0101=Medium\r\n\
					PLAQ0102=High\r\n\
					PLAY0108=Audio codec (%s) not supported by the player!\r\n")},
		{LANG_EN,T("\
					PLAY0109=Video codec (%s) not supported by the player!\r\n\
					PLAY010B=%s decoder not included! It was removed from the official install package because of intellectual property considerations.\r\n\
					PLAY010C=%s decoder not included! It was removed from the official install package because of intellectual property considerations.\r\n\
					PLAY0106=%s\\nUnknown file format!\r\n")},
		{LANG_EN,T("\
					PLAY0107=Buffer size is too small for this media. Try increase it in player settings dialog for better playback!\r\n\
					PLAY010A=Buffer underun occured. The storage media is too slow, the player will periodical pause for loading. Try increase buffer size in settings for better playback!\r\n\
					PLAY0110=Title\r\n\
					PLAY0111=Artist\r\n\
					PLAY0112=Album\r\n\
					PLAY0113=Language\r\n\
					PLAY0114=Genre\r\n\
					PLAY0115=Author\r\n\
					PLAY0116=Copyright\r\n\
					PLAY0117=Priority\r\n\
					PLAY0118=Comment\r\n")},
		{LANG_EN,T("\
					PLAY0119=Track\r\n\
					PLAY011A=Year\r\n\
					PLAY011B=Cover\r\n\
					PLAY011C=Redirect\r\n\
					PLAY0120=Unknown\r\n")},
		{LANG_EN,T("\
					PLAY0121=Video\r\n\
					PLAY0122=Audio\r\n\
					PLAY0123=Subtitle\r\n\
					PLAB0000=Buffering\r\n\
					PLAY0020=Normal buffer size\r\n\
					PLAY0063=Play music in backgr.\r\n\
					PLAY0072=Play movie in backgr.\r\n\
					PLAY0021=Microdrive mode\r\n\
					PLAY0080=Microdrive buffer size\r\n")},
		{LANG_EN,T("\
					PLAY0022=Repeat\r\n\
					PLAY0023=Shuffle\r\n\
					PLAY0024=Play at open\r\n\
					PLAY007B=Play at open in fullscreen\r\n\
					PLAY00B9=Exit after cmd.line playback\r\n\
					PLAY0048=Keep playlist\r\n\
					ADVP002A=Soft-drop tolerance\r\n\
					ADVP002B=Hard-drop tolerance\r\n")},
		{LANG_EN,T("\
					PLAY0068=Move backward step\r\n\
					PLAY007C=Move forward step\r\n\
					PLAY002C=Audio output\r\n\
					PLAY002D=Video output\r\n\
					ADVP002C=Manual A/V offset +/-\r\n\
					PLAY0067=Preload at underrun\r\n\
					PLAY00BB=Preload for audio\r\n\
					PLAY00A3=Microdrive starts at\r\n\
					PLAY0034=Dither\r\n\
					PLAY0035=Fullscreen zoom\r\n")},
		{LANG_EN,T("\
					PLAY0036=GUI zoom\r\n\
					PLAY003D=Pre-rotate portrait movies\r\n\
					PLAY0040=Volume\r\n\
					PLAY0041=Mute\r\n\
					PLAY0042=Audio quality\r\n\
					PLAY00BC=Video quality\r\n\
					PLAY0043=Swap stereo\r\n\
					PLAY003F=Playing speed\r\n\
					PLAY004A=Fast forward speed\r\n\
					PLAY00BF=Show video in backgr.\r\n\
					PLAY00C0=Single click fullscreen\r\n")},
		{LANG_EN,T("\
					COLO0000=Colors\r\n\
					COLO0037=Brightness\r\n\
					COLO0038=Contrast\r\n\
					COLO0062=Saturation\r\n\
					COLO0063=Reset\r\n\
					COLO0064=Red offset\r\n\
					COLO0065=Green offset\r\n\
					COLO0066=Blue offset\r\n\
					ERR_0000=No error\r\n\
					ERR_FFFF=Buffer is full!\r\n")},
		{LANG_EN,T("\
					ERR_FFFE=Out of memory!\r\n\
					ERR_FFFD=Invalid data!\r\n\
					ERR_FFFC=Invalid param!\r\n\
					ERR_FFFB=Feature not supported!\r\n\
					ERR_FFFA=Need more data!\r\n\
					ERR_FFF9=Stream added!\r\n\
					ERR_FFF8=File not found!\\n%s\r\n")},
		{LANG_EN,T("\
					ERR_FFF7=End of file!\r\n\
					ERR_FFF6=Device error!\r\n\
					ERR_FFF5=Syncronized!\r\n\
					ERR_FFF4=Data not found!\r\n\
					ERR_FFF3=%s protocol not supported!\r\n\
					ERR_FFF1=%s plugin is not compatible with current player version. Please update!\r\n\
					ERR_FFF0=Could not open URL!\r\n\
					ERR_FFED=Unauthorized request!\r\n")},
		{LANG_EN,T("\
					EQUA0000=Equalizer\r\n\
					EQUA0180=Pre-amp\r\n\
					EQUA018E=Reset\r\n\
					EQUA018F=Enabled\r\n\
					EQUA0190=Volume normalization\r\n\
					ASSO0000=File Associations\r\n\
					PLAT0000=General\r\n\
					PLAT0010=Language\r\n\
					ADVP0000=Advanced\r\n\
					ADVP001F=Old style toolbars\r\n")},
		{LANG_EN,T("\
					ADVP0020=No backlight keepalive for video\r\n\
					ADVP0028=No wireless MMX usage\r\n\
					ADVP0029=Less rotation tearing (slower)\r\n\
					ADVP002D=Home Screen time out with music playback\r\n\
					ADVP002E=Different hot key handling (compatibility)\r\n\
					ADVP0030=Benchmark from current position\r\n\
					ADVP0031=Show pixel aspect ratio menu\r\n\
					ADVP0036=Prefer less buffering over smooth video\r\n")},
		{LANG_EN,T("\
					ADVP0034=Use system volume\r\n\
					ADVP0035=Override AVI frame rate based on audio\r\n\
					ADVP0037=Use VR41xx tweaks\r\n\
					ADVP0038=D-Pad follow screen orientation\r\n\
					ADVP0039=Use writeable storage memory tweak\r\n\
					ADVP003A=Widcomm BT Audio button support\r\n\
					ADVP003C=Disable non-critical battery warnings\r\n\
					ADVP003F=Force high priority audio output thread\r\n\
					ADVP0040=Auto low quality for large videos\r\n\
					ADVP0041=Disable AVC deblocking filter\r\n")},
		{LANG_EN,T("\
					PLAT0012=Model\r\n\
					PLAT0011=Processor\r\n\
					PLAT0022=Clock speed\r\n\
					PLAT0017=Platform\r\n\
					PLAT001B=OS Version\r\n\
					PLAT0018=OEM Info\r\n\
					ADVP0025=Slow video memory\r\n\
					ADVP0026=Prefer lookup tables over arithmetic\r\n\
					PLAT0027=Advanced platform settings:\r\n")},
		{LANG_EN,T("\
					PLAT006A=OK\r\n\
					PLAT006B=Cancel\r\n\
					PLAT0066=Yes\r\n\
					PLAT0067=No\r\n\
					PLAT0082=Reset\r\n\
					PLAT006C=Error\r\n\
					PLAT0079=Assign\r\n\
					PLAT007A=Clear\r\n\
					PLAT007B=Action\r\n\
					PLAT0073=Space\r\n\
					PLAT006E=Enter\r\n\
					PLAT0088=Done\r\n\
					PLAT0083=Escape\r\n\
					PLAT0084=Prev\r\n\
					PLAT0085=Next\r\n\
					PLAT0086=Play\r\n\
					PLAT0087=Stop\r\n\
					PLAT0071=Left\r\n\
					PLAT0072=Right\r\n\
					PLAT006F=Up\r\n\
					PLAT0070=Down\r\n")},
		{LANG_EN,T("\
					PLAT0074=App %d\r\n\
					PLAT0075=Ctrl\r\n\
					PLAT0076=Shift\r\n\
					PLAT0077=Alt\r\n\
					PLAT0078=Win\r\n\
					PLAT0064=Unexpected program failure. Please send \"crash.txt\" to the developers. Program will now exit.\r\n\
					PLAT0065=Crash\r\n\
					PLAT006D=Not enough memory! Free up some memory or decrease Buffer Size.\r\n\
					PLAT0068=All information is dumped to \"dump.txt\".\r\n\
					PLAT0069=Dump\r\n")},
		{LANG_EN,T("\
					INTF0000=Hot Keys\r\n\
					INTF00DD=Options\r\n\
					INTF00CD=Speed\r\n\
					INTF0132=Repeat\r\n\
					INTF0133=Shuffle\r\n\
					INTF0134=Dither\r\n\
					INTF00CF=Zoom\r\n\
					INTF0138=Fit Best\r\n\
					INTF0234=Fit + 10%\r\n\
					INTF0235=Fit + 20%\r\n\
					INTF0236=Fit + 30%\r\n\
					INTF0237=Fit + 40%\r\n\
					INTF024A=Stretch to Screen\r\n\
					INTF024B=Fill Screen\r\n\
					INTF00D2=Video\r\n\
					INTF013C=Smooth Zoom (only 50%)\r\n\
					INTF022A=Smooth Zoom (always)\r\n")},
		{LANG_EN,T("\
					INTF00D3=Audio\r\n\
					INTF018D=Quality\r\n\
					INTF0144=Low\r\n\
					INTF0145=Medium\r\n\
					INTF0146=High\r\n\
					INTF0250=Quality\r\n\
					INTF0251=Lowest\r\n\
					INTF0252=Low\r\n\
					INTF0253=Normal\r\n\
					INTF0254=Preamp decrease\r\n\
					INTF0255=Preamp %d (clear)\r\n\
					INTF0256=Preamp increase\r\n")},
		{LANG_EN,T("\
					INTF0157=Disabled\r\n\
					INTF0158=Disabled\r\n\
					INTF017F=Fullscreen\r\n\
					INTF0180=Mute\r\n\
					INTF0177=Settings...\r\n\
					INTF00D0=Orientation\r\n\
					INTF00D1=Fullscreen:\r\n\
					INTF0185=Left-handed\r\n\
					INTF0186=Upside Down\r\n\
					INTF0247=Follow GUI\r\n")},
		{LANG_EN,T("\
					INTF0188=Normal\r\n\
					INTF0189=Right-handed\r\n\
					INTF00F0=Rotate by 90癨r\n\
					INTF0224=Toggle Zoom Fit\r\n\
					INTF0225=Zoom in\r\n\
					INTF0226=Zoom out\r\n\
					INTF0227=Change video stream\r\n\
					INTF0228=Change audio stream\r\n\
					INTF0229=Change subtitle\r\n\
					INTF0240=Titlebar\r\n\
					INTF0241=Timeslider\r\n")},
		{LANG_EN,T("\
					INTF0242=Taskbar\r\n\
					INTF018B=Video Driver:\r\n\
					INTF018C=Audio Driver:\r\n\
					INTF00E7=%s Decoder\r\n\
					INTF00F3=Volume Up\r\n\
					INTF00F4=Volume Down\r\n\
					INTF022B=Volume Up Fine\r\n\
					INTF022C=Volume Down Fine\r\n\
					INTF00F5=Brightness Up\r\n\
					INTF00F6=Brightness Down\r\n\
					INTF00F7=Contrast Up\r\n")},
		{LANG_EN,T("\
					INTF00F8=Contrast Down\r\n\
					INTF0102=Saturation Up\r\n\
					INTF0101=Saturation Down\r\n\
					INTF00F9=Screen on/off\r\n\
					INTF00DC=File\r\n\
					INTF0181=Open File...\r\n\
					INTF0280=CoreTheque...\r\n\
					INTF017E=Playlist...\r\n\
					INTF014A=Media Info...\r\n\
					INTF00E6=Benchmark\r\n\
					INTF014B=About...\r\n")},
		{LANG_EN,T("\
					INTF014C=Exit\r\n\
					INTF0172=Play\r\n\
					INTF00F1=Play Fullscreen\r\n\
					INTF0246=Play\r\n\
					INTF0174=Stop\r\n\
					INTF00FA=Move Forward\r\n\
					INTF00FB=Move Back\r\n\
					INTF017A=Next\r\n\
					INTF0190=Next\r\n\
					INTF017B=Previous\r\n")},
		{LANG_EN,T("\
					INTF0183=Beginning/Previous\r\n\
					INTF0191=Beginning/Previous\r\n\
					INTF0182=Fast Forward\r\n\
					INTF0122=Equalizer\r\n\
					INTF0200=Chapters\r\n\
					INTF0223=None\r\n\
					INTF0243=Subtitles\r\n\
					INTF0244=None\r\n\
					INTF0245=Subtitle\r\n\
					INTF0201=Streams:\r\n\
					INTF0221=None\r\n\
					INTF0202=Streams:\r\n")},
		{LANG_EN,T("\
					INTF0222=None\r\n\
					INTF0216=Channels\r\n\
					INTF0203=Stereo\r\n\
					INTF0140=Stereo swapped\r\n\
					INTF0248=Mono Join\r\n\
					INTF0204=Mono Left\r\n\
					INTF0205=Mono Right\r\n\
					INTF020E=View\r\n\
					INTF020F=Pixel Aspect Ratio\r\n\
					INTF0210=Square\r\n\
					INTF0238=4:3 Screen\r\n")},
		{LANG_EN,T("\
					INTF0211=4:3 PAL\r\n\
					INTF0212=4:3 NTSC\r\n\
					INTF0239=16:9 Screen\r\n\
					INTF0213=16:9 PAL\r\n\
					INTF0214=16:9 NTSC\r\n\
					INTF0215=Auto\r\n\
					INTF0260=Skin...\r\n\
					INTF0261=Swap mono left/right\r\n")},
		{LANG_EN,T("\
					INTF0505=Capture all buttons (like games)\r\n\
					INTF0506=On some devices you have to turn on \"capture all buttons\" to this special hot key properly released after program exit (if original function doesn't work, use soft-reset)\r\n\
					INTF0507=Some of the assigned hot keys may not be properly released after program exit.\r\n\
					PLST0300=Title\r\n\
					PLST0301=Length\r\n")},
		{LANG_EN,T("\
					PLST0302=URL\r\n\
					PLST0200=Add Files...\r\n\
					PLST0201=Delete\r\n\
					PLST0202=Delete All\r\n\
					PLST0203=Sort By Name\r\n\
					PLST0204=Up\r\n\
					PLST0205=Down\r\n\
					PLST0206=Load Playlist...\r\n\
					PLST0207=Save Playlist...\r\n\
					PLST0208=File\r\n\
					PLST0209=Play\r\n\
					OPEN0000=Files\r\n")},
		{LANG_EN,T("\
					OPEN0110=Show only name\r\n\
					OPEN020C=Options\r\n\
					OPEN020B=Sort By\r\n\
					OPEN0210=Filter\r\n\
					OPEN020A=Selecting\r\n\
					OPEN0211=All\r\n\
					OPEN0212=Enter URL\r\n\
					OPEN0213=Go\r\n\
					OPEN0214=History\r\n\
					OPEN0215=Empty\r\n\
					OPEN0216=Name is empty!\r\n\
					OPEN0217=Name\r\n\
					OPEN0218=Type\r\n\
					OPEN0219=Save\r\n\
					OPEN021A=Enter file name\r\n")},
		{LANG_EN,T("\
					OPEN021B=Dir\r\n\
					OPEN021C=Directory\r\n\
					FMTM0000=Media files (all types)\r\n\
					FMTL0000=Playlist files (all types)\r\n\
					OPEN0201=All files (*.*)\r\n\
					OPEN0202=Folder\r\n\
					OPEN0206=Up\r\n\
					OPEN0203=Name\r\n\
					OPEN0204=Type\r\n\
					OPEN0205=Size\r\n\
					OPEN020E=Date\r\n")},
		{LANG_EN,T("\
					SETU0000=Settings\r\n\
					SETU0100=Select Page\r\n\
					SETU0101=Please restart the program for changes to take effect!\r\n\
					SETU0102=Please warm boot the device for changes to take effect!\r\n\
					SETU0103=Use the 'Select Page' menu for selecting further settings\r\n\
					MEDI0000=Media Info\r\n")},
		{LANG_EN,T("\
					MEDI0100=Format\r\n\
					MEDI0101=Codec\r\n\
					MEDI0102=Video Stream\r\n\
					MEDI0103=Audio Stream\r\n\
					MEDI0104=Frame Rate\r\n\
					MEDI0106=Video Size\r\n\
					MEDI0109=Data Bit Rate\r\n\
					MEDI010A=Played Frames\r\n\
					MEDI010B=Dropped\r\n\
					MEDI010C=Played FPS\r\n\
					MEDI010D=Audio Format\r\n")},
		{LANG_EN,T("\
					MEDI010E=Mono\r\n\
					MEDI010F=Stereo\r\n\
					BENC0000=Benchmark Results\r\n\
					BENC010B=Save Results\r\n\
					BENC010E=Results are saved as \"%s\"\r\n\
					BENC0100=Video Frames\r\n\
					BENC0106=Audio Samples\r\n\
					BENC0101=Bench. Time\r\n\
					BENC0102=Bench. Frame Rate\r\n\
					BENC0107=Bench. Sample Rate\r\n")},
		{LANG_EN,T("\
					BENC0103=Original Time\r\n\
					BENC0104=Original Frame Rate\r\n\
					BENC0108=Original Sample Rate\r\n\
					BENC0105=Average Speed\r\n\
					BENC0110=Amount of Data\r\n\
					BENC0111=Bench. Data Rate\r\n\
					BENC0112=Original Data Rate\r\n\
					BENC0113=Video zoom\r\n")},
		{LANG_EN,T("\
					BENC010A=Audio/Video out of sync is normal in this mode, especially in high speed situations. The point of this mode is not having any synchronization and delays.\r\n\
					BENC0120=%s Version %s Benchmark Results\r\n\
					BENC0121=URL\r\n\
					BENC0122=Size\r\n\
					BENC0123=Custom Settings\r\n")},
		{LANG_EN,T("\
					BENC0124=Some frames were dropped during benchmarking (probably invalid media file or out of memory). The results are most likely invalid!\r\n\
					QKEY0000=Assign Key\r\n\
					QKEY00C9=Press the button you want to assign!\r\n\
					QKEY00CA=Options\r\n\
					QKEY00CB=Catch in background\r\n\
					PLST0000=Playlist\r\n\
					ABOU0000=About\r\n\
					ABOU00CC=Dump Settings\r\n\
					ABOU00C8=About %s\r\n\
					ABOU00C9=%s Version %s\r\n")},
		{LANG_EN,T("\
					ABOU00CB=This program is FREE software and may be distributed according to the terms of the GNU General Public License.\r\n\
					ABOU00D6=For more information on additional licensing possibilities email license@corecodec.com\r\n\
					ABOU00CD=Libraries used:\r\n\
					ABOU00CF=Special thanks!\r\n\
					ABOU00D1=For support or development questions and comments. Visit our community website @ http://www.tcpmp.com\r\n\
					ABOU00D3=Authors for other parts:\r\n")},
		{LANG_EN,T("\
					ABOU00D4=Copyright\r\n\
					ABOU00D5=Options\r\n\
					CTQU0000=CoreTheque\r\n\
					CTQU0401=Add To Playlist\r\n\
					CTQU0402=Reset\r\n\
					CTQU0403=Artist\r\n\
					CTQU0404=Album\r\n\
					CTQU0405=Track\r\n\
					CTQU0406=Year\r\n\
					CTQU0407=Genre\r\n\
					CTQU0408=Podcasts\r\n\
					CTQU0410=Playlist\r\n")},
		{LANG_EN,T("\
					CTQU0411=Location\r\n\
					CTQU0412=Remove Selected\r\n\
					CTQU0413=Save As Playlist\r\n\
					CTQU0414=Save As Query\r\n\
					CTQU0420=Back From\r\n\
					CTQU0421=All Of\r\n\
					COSP0000=Core OS Panarama files (COSP)\r\n\
					FLAC0000=Free Lossless Audio files (FLAC)\r\n\
					WVPF0000=WavPack files (WV)\r\n\
					TTAF0000=TTA files (TTA)\r\n\
					")},
		{LANG_CHS,T(";CODEPAGE=936\r\n\
					;Version 0.70\r\n\
					\r\n\
					[CHS]\r\n\
					GE2D0000=Sony GE2D驱动器\r\n\
					S1D10000=EPSON图像控制器\r\n\
					ATI40000=ATI Imageon 2D\r\n\
					ATII0000=ATI Imageon Video解码器\r\n\
					OCON0000=Ascii控制终端\r\n\
					AMRN0000=GSM AMR-NB\r\n\
					AMRW0000=GSM AMR-WB\r\n\
					AMFN0000=GSM AMR-NB文件 (AMR)\r\n\
					AMFW0000=GSM AMR-WB文件 (AWB)\r\n")},
		{LANG_CHS,T("\
					MIKM0000=Module文件 (XM,S3M,MOD,STM,IT,ULT,IMF,AMF,669)\r\n\
					MPCF0000=Musepack文件 (MPC)\r\n\
					MP4F0000=MPEG4文件 (MP4,3GP,M4A,M4B,K3G)\r\n\
					MP4F0200=不支持这种类型的MPEG4文件.\r\n\
					MP4F0201=多种媒体格式在同一流中是不被支持的.\r\n\
					AAC_0000=AAC文件 (AAC)\r\n\
					FAAD0000=LibFAAD LC,HE,PS AAC\r\n\
					FAAD0200=不支持这种类型的AAC音频流. 试试安装AACFull插件!\r\n\
					AACF0000=LibFAAD LC,HE,PS,LD,LTP AAC (with ER)\r\n\
					AACF0200=不支持这种类型的AAC音频流.\r\n\
					A52_0000=LibA52 (AC-3)\r\n\
					AC3_0000=AC-3音频文件 (AC3)\r\n")},
		{LANG_CHS,T("\
					TIFF0000=TIFF\r\n\
					TIFF0200=这种类型的TIFF图像格式不被支持.\r\n\
					RTIF0000=TIFF文件 (TIF,TIFF)\r\n\
					PNG_0000=PNG\r\n\
					PNG_0200=这种类型的PNG图像格式不被支持.\r\n\
					RPNG0000=PNG文件 (PNG)\r\n\
					RJPG0000=JPEG文件 (JPG,JPEG)\r\n\
					WJPG0000=WEB MJPEG流媒体\r\n\
					MJPG0000=MJPEG\r\n\
					MJPG0200=不支持这种类型的motion-JPEG格式.\r\n")},
		{LANG_CHS,T("\
					MJPG0201=渐进的(Progressive)JPEG格式依然不被支持.\r\n\
					ADMS0000=MS ADPCM\r\n\
					ADIM0000=IMA ADPCM\r\n\
					ADIQ0000=IMA QuickTime ADPCM\r\n\
					G7260000=G.726 ADPCM\r\n\
					RASX0000=Windows图元文件 (ASX,WMX,WVX,WAX)\r\n\
					M3U_0000=M3U播放列表文件 (M3U)\r\n\
					PLS_0000=PLS播放列表文件 (PLS)\r\n\
					B4S_0000=B4S播放列表文件 (B4S)\r\n\
					MATR0000=Matroska文件 (MKV,MKA)\r\n\
					WEBM0000=WEBM 文件(WEBM)\r\n")},
		{LANG_CHS,T("\
					RAVI0000=视频文件 (AVI,DIVX)\r\n\
					RAVI0100=索引损坏. 不能搜索!\r\n\
					RWAV0000=Windows波形文件 (WAV,RMP)\r\n\
					RASF0000=ASF媒体文件 (ASF)\r\n\
					RASH0000=ASF HTTP流\r\n\
					WMAF0000=Windows媒体音频文件 (WMA)\r\n\
					WMVF0000=Windows媒体视频文件 (WM,WMV)\r\n\
					WMV_0000=Windows媒体视频\r\n\
					WMVA0000=Windows媒体视频高级描述\r\n")},
		{LANG_CHS,T("\
					WMA_0000=Windows媒体音频\r\n\
					WMAV0000=Windows媒体音频声音\r\n\
					MP430000=MS-MPEG4\r\n\
					I2630000=Intel H.263\r\n\
					H2630000=H.263\r\n\
					DIVX0000=MPEG4 DivX/XviD\r\n\
					DIVX0200=不支持四分之一象素功能!\r\n")},
		{LANG_CHS,T("\
					DIVX0201=不支持隔行视频!\r\n\
					DIVX0202=不支持GMC功能!\r\n\
					DIVX0204=不支持这种类型的H.263视频!\r\n\
					DIVX0205=不支持数据分区功能!\r\n\
					VOUT0100=最大支持%d x %d视频尺寸!\r\n\
					MPEG0000=MPEG电影文件 (MPEG,MPG,MPV)\r\n\
					MPEG0200=不支持加密的MPEG-2流!\r\n\
					MVES0000=MPEG视频元素流 (M1V,M4V)\r\n")},
		{LANG_CHS,T("\
					MPG10000=MPEG视频\r\n\
					MPG10201=不支持MPEG-2视频!\r\n\
					MP3_0000=MPEG音频文件 (MP1,MP2,MP3,MPA)\r\n\
					MP3D0000=MPEG音频\r\n\
					LMAD0000=LibMad MPEG音频\r\n\
					VORB0000=Vorbis音频\r\n\
					VORV0000=Vorbis音频\r\n\
					OGGE0000=Vorbis音频 (嵌入的)\r\n\
					OGGP0000=Vorbis音频 (打包的)\r\n\
					OGGV0000=Ogg Vorbis文件 (OGG,OGM)\r\n\
					VOHB0000=Vorbis音频\r\n\
					VOHV0000=Vorbis音频\r\n\
					OGHE0000=Vorbis音频 (嵌入的)\r\n")},
		{LANG_CHS,T("\
					OGHP0000=Vorbis音频 (打包的)\r\n\
					OGHV0000=Ogg Vorbis文件 (OGG,OGM)\r\n\
					SPXL0000=Speex文件 (SPX)\r\n\
					SPXH0000=Speex文件 (SPX)\r\n\
					SPEX0000=Speex语音解码器\r\n\
					AHII0000=ATI IMAGEON解码器\r\n\
					AHI_0000=ATI IMAGEON\r\n\
					AHI_0111=关闭系统位图缓冲 (慎用!)\r\n")},
		{LANG_CHS,T("\
					AHI_0110=\"绿色\" 缺陷补偿\r\n\
					AHI_0113=如果需要，重用原来的界面\r\n\
					AHI_0114=后台小内存模式\r\n\
					AHI_0115=按比例决定LCD图像断裂补偿\r\n\
					AHI_0200=视频内存溢出! 试着重启或选择另外的视频驱动!\r\n\
					AHI_0201=设备打开出错! 请重新启动设备后再试!\r\n")},
		{LANG_CHS,T("\
					AHI_0202=ATI驱动程序太旧(%s). 请升级! 多数情况下SDIO界面更新包含新版ATI驱动程序. 然后尝试选择‘GAPI’驱动启动播放器.\r\n\
					270G0000=Intel 2700G\r\n\
					270G0400=Intel 2700G芯片不可用, 很可能被其他程序正在使用!\r\n\
					270I0000=Intel 2700G解码器\r\n\
					270G0200=LCD图像断裂补偿\r\n\
					270G0201=解码器运转 (较快)\r\n\
					270G0202=当资源锁定时关闭WMP\r\n\
					GAPI0200=您的设备不支持GAPI, 请选择其它的驱动!\r\n\
					HIRS0000=标准显示驱动\r\n\
					HIRP0100=三倍缓冲 (需要更多的内存)\r\n")},
		{LANG_CHS,T("\
					HIRP0101=全屏无边界\r\n\
					HIRP0102=无边界模式在所以设备上都不工作. 你应该重启设备并且取消这个选项!\r\n\
					DRAW0100=带有colorkey的重叠\r\n\
					DRAW0101=使用blitting取代重叠\r\n\
					DRAW0102=为blitting使用设备拉伸 \r\n\
					XSC20000=Intel XScale\r\n\
					XSL20000=Intel XScale低质量\r\n\
					XSCD0000=Intel XScale\r\n\
					XSCD0100=使用硬件垂直缩放\r\n\
					XSCD0101=使用缓冲翻转(解决LCD图像断裂)\r\n")},
		{LANG_CHS,T("\
					XSCD0102=在启动时分配翻转缓冲\r\n\
					RAWD0000=Raw帧缓冲\r\n\
					HTTP0000=HTTP协议\r\n\
					MMS_0000=MMS协议\r\n\
					FILE0000=文件\r\n\
					FIDB0000=FileDb\r\n\
					VFS_0000=VFS\r\n\
					STRM0091=URL\r\n\
					WAVE0000=音频输出\r\n")},
		{LANG_CHS,T("\
					NULV0000=无视频\r\n\
					NULA0000=无音频\r\n\
					ALAW0000=a-Law\r\n\
					ULAW0000=u-Law\r\n\
					FMT_0033=持续时间\r\n\
					FMT_0044=文件大小\r\n\
					PLAY0000=播放器\r\n")},
		{LANG_CHS,T("\
					PLAQ0100=低\r\n\
					PLAQ0101=中\r\n\
					PLAQ0102=高\r\n\
					PLAY0108=音频解码(%s)不被播放器支持!\r\n")},
		{LANG_CHS,T("\
					PLAY0109=视频解码(%s)不被播放器支持!\r\n\
					PLAY010B=未包含解码器%s!\\n\\n非常抱歉, 出于知识产权考虑该解码器从官方安装包中去除.\r\n\
					PLAY010C=未包含解码器%s!\\n\\n非常抱歉, 出于知识产权考虑该解码器从官方安装包中去除.\r\n\
					PLAY0106=%s\\n未知文件格式!\r\n\
					PLAY0107=对于这个媒体,缓冲尺寸太小. 试着增加缓冲以获得更好的播放效果!\r\n\
					PLAY010A=缓冲区出错. 存储介质太慢, 播放器要定期地暂停以缓冲数据. 试着增加缓冲区以获得更好的播放效果!\r\n\
					PLAY0110=标题\r\n")},
		{LANG_CHS,T("\
					PLAY0111=艺术家\r\n\
					PLAY0112=唱片辑\r\n\
					PLAY0113=语言\r\n\
					PLAY0114=流派\r\n\
					PLAY0115=作者\r\n\
					PLAY0116=版权所有\r\n\
					PLAY0117=优先级\r\n\
					PLAY0118=注释\r\n\
					PLAY0119=轨\r\n\
					PLAY011A=年\r\n\
					PLAY011B=覆盖\r\n\
					PLAY011C=重定向\r\n")},
		{LANG_CHS,T("\
					PLAY0120=未知\r\n\
					PLAY0121=视频\r\n\
					PLAY0122=音频\r\n\
					PLAY0123=字幕\r\n\
					PLAY0020=正常缓冲尺寸\r\n\
					PLAY0063=后台播放音乐\r\n\
					PLAY0072=后台播放电影\r\n\
					PLAY0021=微型驱动器模式\r\n\
					PLAY0080=微型驱动器缓存尺寸\r\n\
					PLAY0022=重复\r\n\
					PLAY0023=随机\r\n")},
		{LANG_CHS,T("\
					PLAY0024=打开时播放\r\n\
					PLAY007B=打开时全屏播放\r\n\
					PLAY00B9=命令行重放后退出\r\n\
					PLAY0048=保留播放列表\r\n\
					ADVP002A=软性渐弱公差\r\n\
					ADVP002B=硬性渐弱公差\r\n\
					PLAY0068=快退幅度(秒)\r\n\
					PLAY007C=快进幅度(秒)\r\n\
					PLAY002C=音频输出\r\n")},
		{LANG_CHS,T("\
					PLAY002D=视频输出\r\n\
					ADVP002C=手工调整影音同步\r\n\
					PLAY0067=缺载时预载\r\n\
					PLAY00A3=微型驱动器启动\r\n\
					PLAY0034=抖动\r\n\
					PLAY0035=全屏缩放\r\n\
					PLAY0036=GUI缩放\r\n\
					PLAY003D=自动预旋转\r\n")},
		{LANG_CHS,T("\
					PLAY0040=音量\r\n\
					PLAY0041=静音\r\n\
					PLAY0042=音频质量\r\n\
					PLAY00BC=视频质量\r\n\
					PLAY0043=交换立体声\r\n\
					PLAY003F=播放速度\r\n\
					PLAY004A=快进速度\r\n\
					COLO0000=颜色\r\n\
					COLO0037=亮度\r\n\
					COLO0038=对比度\r\n")},
		{LANG_CHS,T("\
					COLO0062=饱和度\r\n\
					COLO0063=重置\r\n\
					COLO0064=红色偏移\r\n\
					COLO0065=绿色偏移\r\n\
					COLO0066=蓝色偏移\r\n\
					ERR_0000=没有错误\r\n\
					ERR_FFFF=缓冲区满!\r\n")},
		{LANG_CHS,T("\
					ERR_FFFE=内存溢出!\r\n\
					ERR_FFFD=无效数据!\r\n\
					ERR_FFFC=无效参数!\r\n\
					ERR_FFFB=特性不支持!\r\n\
					ERR_FFFA=需要更多的数据!\r\n\
					ERR_FFF9=添加流!\r\n\
					ERR_FFF8=文件未发现!\\n%s\r\n")},
		{LANG_CHS,T("\
					ERR_FFF7=文件结束!\r\n\
					ERR_FFF6=设备错误!\r\n\
					ERR_FFF5=同步!\r\n\
					ERR_FFF4=数据未找到!\r\n\
					ERR_FFF3=%s协议不被支持!\r\n\
					ERR_FFF1=%s插件不兼容当前的播放器版本. 请升级!\r\n\
					ERR_FFF0=不能打开URL!\r\n\
					ERR_FFED=未授权的请求!\r\n\
					EQUA0000=均衡器\r\n\
					EQUA0100=预放大\r\n")},
		{LANG_CHS,T("\
					EQUA010E=重置\r\n\
					EQUA010F=开启\r\n\
					EQUA0110=减弱\r\n\
					ASSO0000=文件关联\r\n\
					PLAT0000=通用\r\n\
					PLAT0010=语言\r\n\
					ADVP0000=高级\r\n\
					ADVP001F=旧风格工具栏\r\n\
					ADVP0020=为视频关闭背光保持\r\n")},
		{LANG_CHS,T("\
					ADVP0028=没有无线MMX使用\r\n\
					ADVP0029=更少的旋转裂开 (较慢)\r\n\
					ADVP002D=音乐后台播放时今日屏幕超时有效\r\n\
					ADVP002E=不同的热键处理 (兼容)\r\n\
					ADVP0030=从当前位置开始基准测试\r\n\
					ADVP0031=显示画面比例菜单\r\n\
					ADVP0036=在平滑视频上优选较少的缓存\r\n\
					ADVP0034=使用系统音量\r\n\
					ADVP0035=基于音频不考虑AVI帧速率\r\n")},
		{LANG_CHS,T("\
					ADVP0037=使用VR41xx调整\r\n\
					ADVP0038=D-Pad跟随屏幕方向\r\n\
					ADVP0039=使用可写的存储内存调整\r\n\
					ADVP003A=支持Widcomm BT音频按键\r\n\
					ADVP003C=禁用非严重危机的电池告警\r\n\
					ADVP003F=强制高优先级音频输出线程\r\n\
					ADVP0040=为大视频自动选择低质量\r\n\
					PLAT0012=型号\r\n\
					PLAT0011=处理器\r\n\
					PLAT0022=时钟速度\r\n\
					PLAT0017=平台\r\n\
					PLAT001B=操作系统版本\r\n\
					PLAT0018=OEM信息\r\n\
					ADVP0025=慢速视频内存\r\n\
					ADVP0026=优选基于算法的表格查找\r\n")},
		{LANG_CHS,T("\
					PLAT0027=高级平台设置:\r\n\
					PLAT006A=确定\r\n\
					PLAT006B=取消\r\n\
					PLAT0066=是\r\n\
					PLAT0067=否\r\n\
					PLAT0082=重置\r\n\
					PLAT006C=错误\r\n\
					PLAT0079=指派\r\n\
					PLAT007A=清除\r\n\
					PLAT007B=动作\r\n\
					PLAT0073=空格\r\n\
					PLAT006E=Enter\r\n")},
		{LANG_CHS,T("\
					PLAT0088=完成\r\n\
					PLAT0083=ESC\r\n\
					PLAT0084=上一个\r\n\
					PLAT0085=下一个\r\n\
					PLAT0086=播放\r\n\
					PLAT0087=停止\r\n\
					PLAT0071=左\r\n\
					PLAT0072=右\r\n\
					PLAT006F=上\r\n\
					PLAT0070=下\r\n\
					PLAT0074=应用 %d\r\n\
					PLAT0075=Ctrl\r\n\
					PLAT0076=Shift\r\n\
					PLAT0077=Alt\r\n\
					PLAT0078=Win\r\n")},
		{LANG_CHS,T("\
					PLAT0064=意外的程序失败. 请发送\"crash.txt\"文件给开发者. 程序现在退出.\r\n\
					PLAT0065=崩溃\r\n\
					PLAT006D=没有足够的内存! 释放一些内存或者减少缓冲尺寸.\r\n\
					PLAT0068=所有的信息已经转储至\"dump.txt\"文件.\r\n\
					PLAT0069=转储\r\n\
					INTF0000=热键设置\r\n\
					INTF00DD=选项\r\n\
					INTF00CD=速度\r\n\
					INTF0132=重复\r\n\
					INTF0133=随机\r\n\
					INTF0134=抖动\r\n\
					INTF00CF=缩放\r\n\
					INTF0138=最佳缩放\r\n")},
		{LANG_CHS,T("\
					INTF0234=适合 + 10%\r\n\
					INTF0235=适合 + 20%\r\n\
					INTF0236=适合 + 30%\r\n\
					INTF0237=适合 + 40%\r\n\
					INTF024A=拉伸至全屏\r\n\
					INTF024B=填充至全屏\r\n\
					INTF00D2=视频\r\n\
					INTF013C=平滑缩放 (仅50%)\r\n\
					INTF022A=平滑缩放 (始终)\r\n\
					INTF00D3=音频\r\n\
					INTF018D=质量\r\n\
					INTF0144=低\r\n\
					INTF0145=中\r\n\
					INTF0146=高\r\n\
					INTF0250=质量\r\n\
					INTF0251=最低\r\n")},
		{LANG_CHS,T("\
					INTF0252=低\r\n\
					INTF0253=普通\r\n\
					INTF0254=预放大减小\r\n\
					INTF0255=预放大 %d (清除)\r\n\
					INTF0256=预放大增大\r\n\
					INTF0157=禁用\r\n\
					INTF0158=禁用\r\n\
					INTF017F=全屏\r\n\
					INTF0180=静音\r\n\
					INTF0177=设置...\r\n\
					INTF00D0=方向\r\n\
					INTF00D1=全屏:\r\n\
					INTF0185=左手持机\r\n\
					INTF0186=上下颠倒\r\n\
					INTF0247=跟随GUI\r\n\
					INTF0188=普通\r\n")},
		{LANG_CHS,T("\
					INTF0189=右手持机\r\n\
					INTF00F0=旋转90度\r\n\
					INTF0224=触发缩放适合\r\n\
					INTF0225=放大\r\n\
					INTF0226=缩小\r\n\
					INTF0227=改变视频流\r\n\
					INTF0228=改变音频流\r\n\
					INTF0229=改变字幕\r\n\
					INTF0240=标题栏\r\n\
					INTF0241=时间进度条\r\n\
					INTF0242=任务栏\r\n")},
		{LANG_CHS,T("\
					INTF018B=视频驱动:\r\n\
					INTF018C=音频驱动:\r\n\
					INTF00E7=%s 解码器\r\n\
					INTF00F3=增大音量\r\n\
					INTF00F4=减小音量\r\n\
					INTF022B=增大音量－合适\r\n\
					INTF022C=减小音量－合适\r\n\
					INTF00F5=增大亮度\r\n\
					INTF00F6=减小亮度\r\n\
					INTF00F7=增大对比度\r\n\
					INTF00F8=减小对比度\r\n\
					INTF0102=增大饱和度\r\n")},
		{LANG_CHS,T("\
					INTF0101=减小饱和度\r\n\
					INTF00F9=屏幕 开/关\r\n\
					INTF00DC=文件\r\n\
					INTF0181=打开文件...\r\n\
					INTF017E=播放列表...\r\n\
					INTF014A=媒体信息...\r\n\
					INTF00E6=基准测试\r\n\
					INTF014B=关于...\r\n\
					INTF014C=退出\r\n\
					INTF0172=播放\r\n\
					INTF00F1=全屏播放\r\n")},
		{LANG_CHS,T("\
					INTF0246=播放\r\n\
					INTF0174=停止\r\n\
					INTF00FA=前进\r\n\
					INTF00FB=后退\r\n\
					INTF017A=下一个\r\n\
					INTF0190=下一个\r\n\
					INTF017B=上一个\r\n\
					INTF0183=开始/上一个\r\n\
					INTF0191=开始/上一个\r\n\
					INTF0182=快进\r\n\
					INTF0122=均衡器\r\n")},
		{LANG_CHS,T("\
					INTF0200=章节\r\n\
					INTF0223=无\r\n\
					INTF0243=字幕\r\n\
					INTF0244=无\r\n\
					INTF0245=字幕\r\n\
					INTF0201=流:\r\n\
					INTF0221=无\r\n\
					INTF0202=流:\r\n\
					INTF0222=无\r\n\
					INTF0216=声道\r\n\
					INTF0203=立体声\r\n")},
		{LANG_CHS,T("\
					INTF0140=交换左右声道\r\n\
					INTF0248=单声道合成\r\n\
					INTF0204=单声道 (左)\r\n\
					INTF0205=单声道 (右)\r\n\
					INTF020E=查看\r\n\
					INTF020F=画面比例\r\n\
					INTF0210=原始\r\n\
					INTF0238=4:3 屏幕\r\n\
					INTF0211=4:3 PAL\r\n\
					INTF0212=4:3 NTSC\r\n\
					INTF0239=16:9 屏幕\r\n\
					INTF0213=16:9 PAL\r\n\
					INTF0214=16:9 NTSC\r\n")},
		{LANG_CHS,T("\
					INTF0215=自动\r\n\
					PLST0300=标题\r\n\
					PLST0301=长度\r\n\
					PLST0302=URL\r\n\
					PLST0200=增加文件...\r\n\
					PLST0201=删除\r\n\
					PLST0202=删除全部\r\n\
					PLST0203=按名字排序\r\n\
					PLST0204=上移\r\n\
					PLST0205=下移\r\n\
					PLST0206=载入播放列表...\r\n")},
		{LANG_CHS,T("\
					PLST0207=保存播放列表...\r\n\
					PLST0208=文件\r\n\
					PLST0209=播放\r\n\
					OPEN0000=文件\r\n\
					OPEN0110=仅显示名字\r\n\
					OPEN020C=选项\r\n\
					OPEN020B=排序以\r\n\
					OPEN0210=过滤\r\n\
					OPEN020A=选择\r\n\
					OPEN0211=全部\r\n\
					OPEN0212=输入URL\r\n\
					OPEN0213=确定\r\n\
					OPEN0214=历史\r\n\
					OPEN0215=空\r\n\
					OPEN0216=名字为空!\r\n")},
		{LANG_CHS,T("\
					OPEN0217=名字\r\n\
					OPEN0218=类型\r\n\
					OPEN0219=保存\r\n\
					OPEN021A=输入文件名\r\n\
					OPEN021B=目录\r\n\
					OPEN021C=目录\r\n\
					FMTM0000=媒体文件 (所有类型)\r\n\
					FMTL0000=播放列表文件 (所有类型)\r\n\
					OPEN0201=所有文件 (*.*)\r\n\
					OPEN0202=文件夹\r\n\
					OPEN0206=上一级\r\n\
					OPEN0203=名字\r\n\
					OPEN0204=类型\r\n\
					OPEN0205=大小\r\n\
					OPEN020E=日期\r\n\
					SETU0000=设置\r\n\
					SETU0100=选择页\r\n")},
		{LANG_CHS,T("\
					SETU0101=请重新启动程序以使改变生效!\r\n\
					SETU0102=请重新启动设备以使改变生效!\r\n\
					SETU0103=使用'选择页'菜单选择更进一步的设置\r\n\
					MEDI0000=媒体信息\r\n\
					MEDI0100=格式\r\n\
					MEDI0101=编解码器\r\n\
					MEDI0102=视频流\r\n\
					MEDI0103=音频流\r\n\
					MEDI0104=帧速率\r\n\
					MEDI0106=视频尺寸\r\n\
					MEDI0109=数据比特率\r\n")},
		{LANG_CHS,T("\
					MEDI010A=已播放的帧数\r\n\
					MEDI010B=已丢弃的\r\n\
					MEDI010C=播放帧速率\r\n\
					MEDI010D=音频格式\r\n\
					MEDI010E=单声道\r\n\
					MEDI010F=立体声\r\n\
					BENC0000=基准测试结果\r\n")},
		{LANG_CHS,T("\
					BENC010B=保存结果\r\n\
					BENC010E=结果被保存为\"%s\"\r\n\
					BENC0100=视频帧\r\n\
					BENC0106=音频帧\r\n\
					BENC0101=基准时间\r\n\
					BENC0102=基准帧速率\r\n\
					BENC0107=基准取样率\r\n\
					BENC0103=原始时间\r\n\
					BENC0104=原始帧速率\r\n")},
		{LANG_CHS,T("\
					BENC0108=原始取样率\r\n\
					BENC0105=平均速率\r\n\
					BENC0110=数据合计\r\n\
					BENC0111=基准数据速率\r\n\
					BENC0112=原始数据速率\r\n\
					BENC0113=视频缩放\r\n\
					BENC010A=在该模式下,影音不同步是正常的,特别是在高速的运行环境下. 这个播放模式没有任何同步和延时.\r\n\
					BENC0120=%s 版本 %s 基准测试结果\r\n\
					BENC0121=URL\r\n\
					BENC0122=尺寸\r\n\
					BENC0123=自定义设置\r\n")},
		{LANG_CHS,T("\
					BENC0124=在基准测试中,一些帧会被丢弃(可能是有问题的媒体文件或内存不够). 这种结果多数是有问题的!\r\n\
					QKEY0000=定义按键\r\n\
					QKEY00C9=请按下你要定义的按键!\r\n\
					QKEY00CA=选项\r\n\
					QKEY00CB=在后台捕捉\r\n\
					PLST0000=播放列表\r\n\
					ABOU0000=关于\r\n\
					ABOU00CC=转储设置\r\n\
					ABOU00C8=关于 %s\r\n\
					ABOU00C9=%s 版本 %s\r\n")},
		{LANG_CHS,T("\
					ABOU00CB=该程序是自由软件,可以依照GNU公众许可协议进行传播.\r\n\
					ABOU00D6=获取更多附加的许可协议,请email至license@corecodec.com\r\n\
					ABOU00CD=使用的库:\r\n\
					ABOU00CF=特别感谢!\r\n\
					ABOU00D1=寻求技术支持或有关开发的意见和建议.请访问我们的论坛http://www.tcpmp.com\r\n\
					ABOU00D2=Translated by coool\r\n\
					ABOU00D3=其他部分的作者:\r\n\
					ABOU00D4=版权所有\r\n\
					ABOU00D5=选项\r\n\
					")},
		{0,T("")},
};

int IsHaveResLang(int Fourcc)
{
	int Found = 0;
	int i = 0;
	Language* lang = &Lang[0];
	
	while(lang&&lang->Fourcc)
	{
		if(lang->Fourcc == Fourcc)
		{
			Found = 1;
			break;
		}		
		lang++;
		i++;
	}
	if(Found)
		return i;
	
	return -1;
}
int GetResLang(int Fourcc,uint8_t* Buffer)
{
	int size = 0;
	Language* lang;
	int i = IsHaveResLang(Fourcc);
	if(i < 0 )
		return 0;
		
	lang = &Lang[i];
	
	while(lang&&lang->Fourcc==Fourcc)
	{
		tchar_t* Tail = strrchr(lang->szText,0x0a);
		memcpy(Buffer,lang->szText,sizeof(lang->szText));
		Buffer+=Tail-lang->szText;
		size+=Tail-lang->szText;
		lang++;
	}
	return size;
}

#endif
