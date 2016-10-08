ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_pictrue
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common \
        $(LOCAL_PATH)/../common/zlib \
        $(LOCAL_PATH)/g726
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DASAP
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		adpcm.c \
        		law.c \
        		mjpeg.c \
        		png.c \
        		tiff.c \
        		g726/g726_16.c \
        		g726/g726_24.c \
        		g726/g726_32.c \
        		g726/g726_40.c \
        		g726/g72x.c 
        		

include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif