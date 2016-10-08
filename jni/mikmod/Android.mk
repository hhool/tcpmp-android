ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := mikmod
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libmikmod/include \
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		mikmod.c \
        		libmikmod/drivers/drv_nos.c \
        		libmikmod/loaders/load_669.c \
        		libmikmod/loaders/load_amf.c \
        		libmikmod/loaders/load_asy.c \
        		libmikmod/loaders/load_dsm.c \
        		libmikmod/loaders/load_far.c \
        		libmikmod/loaders/load_gdm.c \
        		libmikmod/loaders/load_imf.c \
        		libmikmod/loaders/load_it.c \
        		libmikmod/loaders/load_m15.c \
        		libmikmod/loaders/load_med.c \
        		libmikmod/loaders/load_mod.c \
        		libmikmod/loaders/load_mtm.c \
        		libmikmod/loaders/load_okt.c \
        		libmikmod/loaders/load_s3m.c \
        		libmikmod/loaders/load_stm.c \
        		libmikmod/loaders/load_stx.c \
        		libmikmod/loaders/load_ult.c \
        		libmikmod/loaders/load_uni.c \
        		libmikmod/loaders/load_xm.c \
        		libmikmod/playercode/mdriver.c \
        		libmikmod/playercode/mloader.c \
        		libmikmod/playercode/mlreg.c \
        		libmikmod/playercode/mlutil.c \
        		libmikmod/playercode/mplayer.c \
        		libmikmod/playercode/munitrk.c \
        		libmikmod/playercode/mwav.c \
        		libmikmod/playercode/npertab.c \
        		libmikmod/playercode/sloader.c \
        		libmikmod/playercode/virtch.c \
        		libmikmod/playercode/virtch2.c \
        		libmikmod/playercode/virtch_common.c \
        		libmikmod/mmio/mmio.c \
        		libmikmod/mmio/mmerror.c 
        		
include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif