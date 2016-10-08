ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := codec_speex
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
		$(LOCAL_PATH)/speex/include  \
        $(LOCAL_PATH)/../common
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 -fvisibility=hidden #-g -DLOG
LOCAL_CFLAGS +=-DTARGET_ANDROID -DFIXED_POINT
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		stdafx.c \
        		speexcodec.c \
        		speex/libspeex/bits.c \
        		speex/libspeex/cb_search.c \
        		speex/libspeex/exc_10_16_table.c \
        		speex/libspeex/exc_10_32_table.c \
        		speex/libspeex/exc_20_32_table.c \
        		speex/libspeex/exc_5_256_table.c \
        		speex/libspeex/exc_5_64_table.c \
        		speex/libspeex/exc_8_128_table.c \
        		speex/libspeex/filters.c \
        		speex/libspeex/lbr_48k_tables.c \
        		speex/libspeex/lpc.c \
        		speex/libspeex/lsp.c \
        		speex/libspeex/lsp_tables_nb.c \
        		speex/libspeex/ltp.c \
        		speex/libspeex/math_approx.c \
        		speex/libspeex/misc.c \
        		speex/libspeex/modes.c \
        		speex/libspeex/nb_celp.c \
        		speex/libspeex/quant_lsp.c \
        		speex/libspeex/sb_celp.c \
        		speex/libspeex/speex.c \
        		speex/libspeex/speex_callbacks.c \
        		speex/libspeex/speex_header.c \
        		speex/libspeex/stereo.c \
        		speex/libspeex/vbr.c \
        		speex/libspeex/vq.c \
        		speex/libspeex/gain_table.c \
        		speex/libspeex/gain_table_lbr.c \
        		speex/libspeex/hexc_10_32_table.c \
        		speex/libspeex/hexc_table.c \
        		speex/libspeex/high_lsp_tables.c
        		
include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif