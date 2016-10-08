ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_flac
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common \
        $(LOCAL_PATH)/flac/include  \
        $(LOCAL_PATH)/flac/src/libFLAC/include  
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DFLAC__NO_DLL
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		flac.c \
        		flac/src/libFLAC/bitbuffer.c \
        		flac/src/libFLAC/bitmath.c \
        		flac/src/libFLAC/cpu.c \
        		flac/src/libFLAC/crc.c \
        		flac/src/libFLAC/fixed.c \
        		flac/src/libFLAC/float.c \
        		flac/src/libFLAC/format.c \
        		flac/src/libFLAC/lpc.c \
        		flac/src/libFLAC/md5.c \
        		flac/src/libFLAC/memory.c \
        		flac/src/libFLAC/metadata_object.c \
        		flac/src/libFLAC/seekable_stream_decoder.c \
        		flac/src/libFLAC/stream_decoder.c

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif