ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_mpeg4
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID
LOCAL_CFLAGS +=-DMPEG4_EXPORTS
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		divx3_data.c \
        		divx3_header.c \
        		divx3_mblock.c \
        		divx3_vlc.c \
        		h263_header.c \
        		h263_mblock.c \
        		h263_vld.c \
        		mp4_decode.c \
        		mp4_header.c \
        		mp4_mblock.c \
        		mp4_mv.c \
        		mp4_predict.c \
        		mp4_stream.c \
        		mp4_vld_h263.c \
        		mp4_vld_mpeg.c \
        		stdafx.c

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif