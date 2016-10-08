LOCAL_PATH:= $(call my-dir)

SRC_FILES := \
	ffmpega_origin.c \
	ffmpega_real.c \
	ffmpegv_origin.c \
	ffmpegv_real.c \
	stdafx.c \
	libavutil/adler32.c \
	libavutil/aes.c \
	libavutil/audioconvert.c \
	libavutil/avstring.c \
	libavutil/base64.c \
	libavutil/cpu.c \
	libavutil/crc.c \
	libavutil/des.c \
	libavutil/dict.c \
	libavutil/error.c \
	libavutil/eval.c \
	libavutil/fifo.c \
	libavutil/file.c \
	libavutil/imgutils.c \
	libavutil/intfloat_readwrite.c \
	libavutil/inverse.c \
	libavutil/lfg.c \
	libavutil/lls.c \
	libavutil/log.c \
	libavutil/lzo.c \
	libavutil/mathematics.c \
	libavutil/md5.c \
	libavutil/mem.c \
	libavutil/opt.c \
	libavutil/parseutils.c \
	libavutil/pixdesc.c \
	libavutil/random_seed.c \
	libavutil/rational.c \
	libavutil/rc4.c \
	libavutil/samplefmt.c \
	libavutil/sha.c \
	libavutil/tree.c \
	libavutil/utils.c \
	libavutil/arm/cpu.c \
	libavcodec/aac_ac3_parser.c \
	libavcodec/aac_parser.c \
	libavcodec/aacadtsdec.c \
	libavcodec/aacdec.c \
	libavcodec/aacps.c \
	libavcodec/aacsbr.c \
	libavcodec/aactab.c \
	libavcodec/ac3_parser.c \
	libavcodec/ac3tab.c \
	libavcodec/acelp_filters.c \
	libavcodec/acelp_pitch_delay.c \
	libavcodec/acelp_vectors.c \
	libavcodec/adpcm.c \
	libavcodec/allcodecs.c \
	libavcodec/atrac.c \
	libavcodec/atrac3.c \
	libavcodec/audioconvert.c \
	libavcodec/avfft.c \
	libavcodec/avpacket.c \
	libavcodec/bitstream.c \
	libavcodec/bitstream_filter.c \
	libavcodec/bmp.c \
	libavcodec/cabac.c \
	libavcodec/cavs_parser.c \
	libavcodec/celp_filters.c \
	libavcodec/celp_math.c \
	libavcodec/cinepak.c \
	libavcodec/cook.c \
	libavcodec/dca_parser.c \
	libavcodec/dct.c \
	libavcodec/dct32_fixed.c \
	libavcodec/dct32_float.c \
	libavcodec/dirac_parser.c \
	libavcodec/dnxhd_parser.c \
	libavcodec/dsputil.c \
	libavcodec/dvbsub_parser.c \
	libavcodec/dvdsub_parser.c \
	libavcodec/error_resilience.c \
	libavcodec/faanidct.c \
	libavcodec/fft_fixed.c \
	libavcodec/fft_float.c \
	libavcodec/flac.c \
	libavcodec/flac_parser.c \
	libavcodec/flacdata.c \
	libavcodec/flashsv.c \
	libavcodec/flvdec.c \
	libavcodec/fmtconvert.c \
	libavcodec/golomb.c \
	libavcodec/h261.c \
	libavcodec/h261_parser.c \
	libavcodec/h261dec.c \
	libavcodec/h263.c \
	libavcodec/h263_parser.c \
	libavcodec/h263dec.c \
	libavcodec/h264.c \
	libavcodec/h264_cabac.c \
	libavcodec/h264_cavlc.c \
	libavcodec/h264_direct.c \
	libavcodec/h264_loopfilter.c \
	libavcodec/h264_parser.c \
	libavcodec/h264_ps.c \
	libavcodec/h264_refs.c \
	libavcodec/h264_sei.c \
	libavcodec/h264dsp.c \
	libavcodec/h264idct.c \
	libavcodec/h264pred.c \
	libavcodec/huffman.c \
	libavcodec/imgconvert.c \
	libavcodec/intelh263dec.c \
	libavcodec/intrax8.c \
	libavcodec/intrax8dsp.c \
	libavcodec/ituh263dec.c \
	libavcodec/jrevdct.c \
	libavcodec/kbdwin.c \
	libavcodec/latm_parser.c \
	libavcodec/lsp.c \
	libavcodec/mdct_fixed.c \
	libavcodec/mdct_float.c \
	libavcodec/mjpeg_parser.c \
	libavcodec/mlp.c \
	libavcodec/mlp_parser.c \
	libavcodec/mpeg12.c \
	libavcodec/mpeg12data.c \
	libavcodec/mpeg4audio.c \
	libavcodec/mpeg4video.c \
	libavcodec/mpeg4video_parser.c \
	libavcodec/mpeg4videodec.c \
	libavcodec/mpegaudio.c \
	libavcodec/mpegaudio_parser.c \
	libavcodec/mpegaudiodata.c \
	libavcodec/mpegaudiodec.c \
	libavcodec/mpegaudiodecheader.c \
	libavcodec/mpegaudiodsp.c \
	libavcodec/mpegaudiodsp_fixed.c \
	libavcodec/mpegaudiodsp_float.c \
	libavcodec/mpegvideo.c \
	libavcodec/mpegvideo_parser.c \
	libavcodec/msmpeg4.c \
	libavcodec/msmpeg4data.c \
	libavcodec/msrle.c \
	libavcodec/msrledec.c \
	libavcodec/msvideo1.c \
	libavcodec/nellymoser.c \
	libavcodec/nellymoserdec.c \
	libavcodec/opt.c \
	libavcodec/options.c \
	libavcodec/parser.c \
	libavcodec/pnm.c \
	libavcodec/pnm_parser.c \
	libavcodec/qdm2.c \
	libavcodec/ra144.c \
	libavcodec/ra144dec.c \
	libavcodec/ra288.c \
	libavcodec/raw.c \
	libavcodec/rawdec.c \
	libavcodec/rdft.c \
	libavcodec/resample.c \
	libavcodec/resample2.c \
	libavcodec/rv10.c \
	libavcodec/rv30.c \
	libavcodec/rv30dsp.c \
	libavcodec/rv34.c \
	libavcodec/rv40.c \
	libavcodec/rv40dsp.c \
	libavcodec/simple_idct.c \
	libavcodec/sinewin.c \
	libavcodec/sipr.c \
	libavcodec/sipr16k.c \
	libavcodec/svq1.c \
	libavcodec/svq1dec.c \
	libavcodec/svq3.c \
	libavcodec/tscc.c \
	libavcodec/utils.c \
	libavcodec/vc1.c \
	libavcodec/vc1_parser.c \
	libavcodec/vc1data.c \
	libavcodec/vc1dec.c \
	libavcodec/vc1dsp.c \
	libavcodec/vorbis_data.c \
	libavcodec/vp3.c \
	libavcodec/vp3_parser.c \
	libavcodec/vp3dsp.c \
	libavcodec/vp5.c \
	libavcodec/vp56.c \
	libavcodec/vp56data.c \
	libavcodec/vp56dsp.c \
	libavcodec/vp56rac.c \
	libavcodec/vp6.c \
	libavcodec/vp6dsp.c \
	libavcodec/vp8.c \
	libavcodec/vp8_parser.c \
	libavcodec/vp8dsp.c \
	libavcodec/wma.c \
	libavcodec/wmadec.c \
	libavcodec/wmv2.c \
	libavcodec/wmv2dec.c \
	libavcodec/xiph.c	
ARM_SRC_FILES := \
	libavcodec/arm/asm.S \
	libavcodec/arm/dsputil_arm.S \
	libavcodec/arm/dsputil_init_arm.c \
	libavcodec/arm/dsputil_init_armv5te.c \
	libavcodec/arm/fft_fixed_init_arm.c \
	libavcodec/arm/fft_init_arm.c \
	libavcodec/arm/fmtconvert_init_arm.c \
	libavcodec/arm/h264dsp_init_arm.c \
	libavcodec/arm/h264pred_init_arm.c \
	libavcodec/arm/jrevdct_arm.S \
	libavcodec/arm/mpegaudiodsp_init_arm.c \
	libavcodec/arm/mpegvideo_arm.c \
	libavcodec/arm/mpegvideo_armv5te.c \
	libavcodec/arm/mpegvideo_armv5te_s.S \
	libavcodec/arm/simple_idct_arm.S \
	libavcodec/arm/simple_idct_armv5te.S \
	libavcodec/arm/vp56dsp_init_arm.c \
	libavcodec/arm/vp8dsp_init_arm.c
VFP_SRC_FILES := \
	libavcodec/arm/dsputil_init_vfp.c \
	libavcodec/arm/dsputil_vfp.S \
	libavcodec/arm/fmtconvert_vfp.S
ARM6_SRC_FILES:=\
	libavcodec/arm/dsputil_armv6.S \
	libavcodec/arm/dsputil_init_armv6.c \
	libavcodec/arm/mpegaudiodsp_fixed_armv6.S \
	libavcodec/arm/simple_idct_armv6.S \
	libavcodec/arm/vp8_armv6.S	
NEON_SRC_FILES := \
	libavcodec/arm/dsputil_init_neon.c \
	libavcodec/arm/dsputil_neon.S \
	libavcodec/arm/fft_fixed_neon.S \
	libavcodec/arm/fft_neon.S \
	libavcodec/arm/fmtconvert_neon.S \
	libavcodec/arm/h264dsp_neon.S \
	libavcodec/arm/h264idct_neon.S \
	libavcodec/arm/h264pred_neon.S \
	libavcodec/arm/int_neon.S \
	libavcodec/arm/mdct_fixed_neon.S \
	libavcodec/arm/mdct_neon.S \
	libavcodec/arm/mpegvideo_neon.S \
	libavcodec/arm/rdft_neon.S \
	libavcodec/arm/simple_idct_neon.S \
	libavcodec/arm/vp3dsp_neon.S \
	libavcodec/arm/vp56dsp_neon.S \
	libavcodec/arm/vp8dsp_neon.S
	

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

##codec_avcodec_v5te##
LOCAL_MODULE    := codec_avcodec

LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libavutil \
    	$(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS +=-fPIC -O3  -fvisibility=hidden #-DLOG #-g
LOCAL_CFLAGS +=-DTARGET_ANDROID -DTCPMP -DHAVE_AV_CONFIG_H -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math

LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES :=$(SRC_FILES)
LOCAL_SRC_FILES +=$(ARM_SRC_FILES)

include $(BUILD_SHARED_LIBRARY)


##codec_avcodec_v6##

#cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

#include $(CLEAR_VARS)

#LOCAL_MODULE    := codec_avcodec_v6

#LOCAL_C_INCLUDES := \
#		$(SYSROOT)/usr/include \
#		$(LOCAL_PATH) \
#		$(LOCAL_PATH)/libavutil \
#    	$(LOCAL_PATH)/../common
        
#LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
#LOCAL_ARM_MODE := arm

#LOCAL_CFLAGS +=-fPIC -O2  -fvisibility=hidden #-DLOG #-g
#LOCAL_CFLAGS +=-DTARGET_ANDROID -DTCPMP -DHAVE_AV_CONFIG_H -DINLINE=inline 
#LOCAL_CFLAGS +=-mfloat-abi=softfp

#LOCAL_CFLAGS +=-march=armv6
#LOCAL_CFLAGS +=-DCMP_HAVE_ARMV6=1 -DCMP_HAVE_ARMV6T2=1

#LOCAL_SRC_FILES :=$(SRC_FILES)
#LOCAL_SRC_FILES +=$(ARM_SRC_FILES)
#LOCAL_SRC_FILES +=$(ARM6_SRC_FILES)  	  

#LOCAL_SHARED_LIBRARIES := libcore

#include $(BUILD_SHARED_LIBRARY)


cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := codec_avcodec_v6_vfp

LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libavutil \
    	$(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS +=-fPIC -O3  -fvisibility=hidden #-DLOG #-g
LOCAL_CFLAGS +=-DTARGET_ANDROID -DTCPMP -DHAVE_AV_CONFIG_H -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math

LOCAL_CFLAGS +=-mfpu=vfp
LOCAL_CFLAGS +=-march=armv6
LOCAL_CFLAGS +=-DCMP_HAVE_ARMV6=1 -DCMP_HAVE_ARMV6T2=1 -DCMP_HAVE_VFP=1


LOCAL_SRC_FILES :=$(SRC_FILES)	 			
LOCAL_SRC_FILES +=$(ARM_SRC_FILES)	
LOCAL_SRC_FILES +=$(ARM6_SRC_FILES)
LOCAL_SRC_FILES +=$(VFP_SRC_FILES)  

LOCAL_SHARED_LIBRARIES := libcore

include $(BUILD_SHARED_LIBRARY)


cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := codec_avcodec_v7_v3fp

LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libavutil \
    	$(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS +=-fPIC -O3  -fvisibility=hidden #-DLOG #-g
LOCAL_CFLAGS +=-DTARGET_ANDROID -DTCPMP -DHAVE_AV_CONFIG_H -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math

LOCAL_CFLAGS +=-mfpu=vfpv3
LOCAL_CFLAGS +=-march=armv7-a
LOCAL_CFLAGS +=-DCMP_HAVE_ARMV6=1 -DCMP_HAVE_ARMV6T2=1  -DCMP_HAVE_VFPV3=1


LOCAL_SRC_FILES :=$(SRC_FILES)	 			
LOCAL_SRC_FILES +=$(ARM_SRC_FILES)	
LOCAL_SRC_FILES +=$(ARM6_SRC_FILES)
LOCAL_SRC_FILES +=$(VFP_SRC_FILES)	  

LOCAL_SHARED_LIBRARIES := libcore

include $(BUILD_SHARED_LIBRARY)


cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := codec_avcodec_v7_neon

LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libavutil \
    	$(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm

LOCAL_CFLAGS +=-fPIC -O3  -fvisibility=hidden #-DLOG #-g
LOCAL_CFLAGS +=-DTARGET_ANDROID -DTCPMP -DHAVE_AV_CONFIG_H -DINLINE=inline 
LOCAL_CFLAGS +=-mfloat-abi=softfp -ffast-math

LOCAL_CFLAGS +=-mfpu=neon
LOCAL_CFLAGS +=-march=armv7-a  
#LOCAL_CFLAGS +=-DCMP_HAVE_NEON=1 -DCMP_HAVE_ARMV6=1 -DCMP_HAVE_VFP=1  -DCMP_HAVE_VFPV3=1 -DCMP_HAVE_ARMV6T2=1
LOCAL_CFLAGS +=-DCMP_HAVE_ARMV6=1 -DCMP_HAVE_ARMV6T2=1 -DCMP_HAVE_NEON=1


LOCAL_SRC_FILES :=$(SRC_FILES)	 			
LOCAL_SRC_FILES +=$(ARM_SRC_FILES)	
LOCAL_SRC_FILES +=$(ARM6_SRC_FILES)
LOCAL_SRC_FILES +=$(NEON_SRC_FILES)  

LOCAL_SHARED_LIBRARIES := libcore

include $(BUILD_SHARED_LIBRARY)
