LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(patsubst /cygdrive/c/%,c:/%,$1)
ADDITION_INC := $(LOCAL_PATH)/arm/Makefile
include $(LOCAL_PATH)/../av.mk
ADDITION_INC := 

LOCAL_SRC_FILES := $(FFFILES)

LOCAL_C_INCLUDES :=		\
	$(LOCAL_PATH)		\
	$(LOCAL_PATH)/..
LOCAL_CFLAGS += $(FFCFLAGS)
LOCAL_LDLIBS := -lz
LOCAL_STATIC_LIBRARIES := libavutil
LOCAL_MODULE := libffmpeg #$(FFNAME)
LOCAL_ARM_MODE := arm
include $(BUILD_SHARED_LIBRARY)

