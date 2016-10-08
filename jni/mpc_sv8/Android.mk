ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := demux_mpc
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/libmusepack_sv8/include\
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMPC_LITTLE_ENDIAN -DMPC_FIXED_POINT
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		mpc.c	\
        		libmusepack_sv8/common/crc32.c \
        		libmusepack_sv8/libmpcdec/huffman.c \
        		libmusepack_sv8/libmpcdec/mpc_bits_reader.c \
        		libmusepack_sv8/libmpcdec/mpc_decoder.c \
        		libmusepack_sv8/libmpcdec/mpc_demux.c \
        		libmusepack_sv8/libmpcdec/mpc_reader.c \
        		libmusepack_sv8/libmpcdec/requant.c \
        		libmusepack_sv8/libmpcdec/streaminfo.c \
        		libmusepack_sv8/libmpcdec/synth_filter.c
        		
include $(BUILD_SHARED_LIBRARY)

else
include $(all-subdir-makefiles)
endif