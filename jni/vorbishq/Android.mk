ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := vorbishq
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/tremor  \
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DBYTE_ORDER=1 -DLITTLE_ENDIAN=1 -DBIG_ENDIAN=0
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		ogg.c \
        		oggembedded.c \
        		oggpacket.c \
        		vorbis.c \
        		tremor/bitwise.c \
        		tremor/block.c \
        		tremor/codebook.c \
        		tremor/floor0.c \
        		tremor/floor1.c \
        		tremor/framing.c \
        		tremor/info.c \
        		tremor/mapping0.c \
        		tremor/mdctvorbis.c \
        		tremor/registry.c \
        		tremor/res012.c \
        		tremor/sharedbook.c \
        		tremor/synthesis.c \
        		tremor/vorbisfile.c \
        		tremor/window.c
        		
include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif