ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_a52
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common \
        $(LOCAL_PATH)/liba52 \
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DBUILDFIXED -DLIBA52_FIXED
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		ac3.c \
        		liba52/bit_allocate.c \
        		liba52/bitstream2.c \
        		liba52/crc.c \
        		liba52/downmix.c \
        		liba52/imdct.c \
        		liba52/parse.c

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif