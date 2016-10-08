ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_mpeg4aac
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common \
        $(LOCAL_PATH)/faad2/include \
        $(LOCAL_PATH)/faad2/libfaad \
        $(LOCAL_PATH)/faad2/libfaad/codebook \
        $(LOCAL_PATH)/libpaac \
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DLC_ONLY_DECODER -DFIXED_POINT -DHAVE_STDLIB_H -DHAVE_STRING_H  
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		faad.c \
        		faad2/libfaad/bits.c \
        		faad2/libfaad/cfft.c \
        		faad2/libfaad/common.c \
        		faad2/libfaad/decoder.c \
        		faad2/libfaad/drc.c \
        		faad2/libfaad/drm_dec.c \
        		faad2/libfaad/filtbank.c \
        		faad2/libfaad/hcr.c \
        		faad2/libfaad/huffman.c \
        		faad2/libfaad/ic_predict.c \
        		faad2/libfaad/is.c \
        		faad2/libfaad/lt_predict.c \
        		faad2/libfaad/mdct.c \
        		faad2/libfaad/mp4.c \
        		faad2/libfaad/ms.c \
        		faad2/libfaad/output.c \
        		faad2/libfaad/pns.c \
        		faad2/libfaad/ps_dec.c \
        		faad2/libfaad/ps_syntax.c \
        		faad2/libfaad/pulse.c \
        		faad2/libfaad/rvlc.c \
        		faad2/libfaad/sbr_dct.c \
        		faad2/libfaad/sbr_dec.c \
        		faad2/libfaad/sbr_e_nf.c \
        		faad2/libfaad/sbr_fbt.c \
        		faad2/libfaad/sbr_hfadj.c \
        		faad2/libfaad/sbr_hfgen.c \
        		faad2/libfaad/sbr_huff.c \
        		faad2/libfaad/sbr_qmf.c \
        		faad2/libfaad/sbr_syntax.c \
        		faad2/libfaad/sbr_tf_grid.c \
        		faad2/libfaad/specrec.c \
        		faad2/libfaad/ssr.c \
        		faad2/libfaad/ssr_fb.c \
        		faad2/libfaad/ssr_ipqf.c \
        		faad2/libfaad/syntax.c \
        		faad2/libfaad/tns.c \
        		libpaac/aac_imdct.c  
        	
include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif