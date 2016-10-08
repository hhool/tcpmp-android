ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_libmad
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libmad \
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS +=-fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DFPM_DEFAULT -DOPT_SPEED -DASO_INTERLEAVE1 -DBUILDFIXED
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_MODULE_PATH :=$(TARGET_OUT)/lib

LOCAL_SRC_FILES := \
        		stdafx.c \
        		libmad.c \
        		libmad/bit.c \
        		libmad/fixed.c \
        		libmad/frame.c \
        		libmad/huffmanmad.c \
        		libmad/layer12.c \
        		libmad/layer3.c \
        		libmad/stream.c \
        		libmad/synth.c

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif