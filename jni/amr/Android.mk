ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_amr
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common \
        $(LOCAL_PATH)/26104 \
        $(LOCAL_PATH)/26204 
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID 
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		amrnb.c \
        		amrwb.c \
        		stdafx.c \
        		26104/interf_dec.c \
        		26104/interf_enc.c \
        		26104/sp_dec.c \
        		26104/sp_enc.c \
        		26204/dec_acelp.c \
        		26204/dec_dtx.c \
        		26204/dec_gain.c \
        		26204/dec_if.c \
        		26204/dec_lpc.c \
        		26204/dec_rom.c \
        		26204/dec_util.c \
        		26204/if_rom.c \
        		26204/dec_main.c
        		
include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif