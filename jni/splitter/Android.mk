ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := demuxer
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include\
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG 
LOCAL_CFLAGS +=-DTARGET_ANDROID 
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_PRELINK_MODULE :=false
LOCAL_SHARED_LIBRARIES := libcore
LOCAL_MODULE_PATH :=$(TARGET_OUT)/lib

LOCAL_SRC_FILES := \
        		asf.c \
        		avi.c \
        		mov.c \
        		mpg.c \
        		nsv.c \
        		stdafx.c \
        		wav.c \
        		flv.c \
        		rm.c

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif