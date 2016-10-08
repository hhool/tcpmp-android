ifeq ($(TARGET_ARCH_ABI),armeabi)

LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_atari8
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common \
        $(LOCAL_PATH)/asap \
        $(LOCAL_PATH)/atari800 \
        $(LOCAL_PATH)/atari800/src \
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DASAP
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		asapfmt.c \
        		asap/asap.c \
        		atari800/src/cpu.c \
        		atari800/src/mzpokeysnd.c \
        		atari800/src/pokeysnd.c \
        		atari800/src/remez.c

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif