ifeq ($(TARGET_ARCH_ABI),armeabi)
LOCAL_PATH:= $(call my-dir)

ANDROID16_SYS_INC := $(LOCAL_PATH)/aosp-d

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := player-d-jni #<system ver < 8 (2.2)
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(ANDROID16_SYS_INC)/frameworks/base/include \
		$(ANDROID16_SYS_INC)/hardware/libhardware/include \
    	$(ANDROID16_SYS_INC)/system/core/include \
    	$(ANDROID16_SYS_INC)/external/skia/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common 
        
        
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 #-DLOG #-g  #-fvisibility=hidden 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMULTITHREAD -DPLATFORM=4
LOCAL_CFLAGS +=-DPACKAGE_NAME=com_jumpplayer_object -DPACKAGE_DIR=\"com/jumplayer/object\"

LOCAL_LDLIBS :=-L$(call host-path,$(LOCAL_PATH)/aosp-d) \
               -lutils \
               -lui \
               -lmedia                 \
               -L$($(SYSROOT)/usr/lib) \
               -ldl                    \
               -llog
LOCAL_PRELINK_MODULE :=false
LOCAL_SHARED_LIBRARIES := libcore

LOCAL_SRC_FILES := \
        		player-jni.cpp \
        		overlay_vsurface.c \
        		vsurface_wrap.cpp \
        		waveout_android.c \
        		wave_audiotrack.cpp \
        		mediainfo.cpp
        		
include $(BUILD_SHARED_LIBRARY)

ANDROID22_SYS_INC := $(LOCAL_PATH)/aosp-f

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := player-f-jni #<system ver >= 8 (2.2)
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(ANDROID22_SYS_INC)/frameworks/base/include \
		$(ANDROID22_SYS_INC)/hardware/libhardware/include \
    	$(ANDROID22_SYS_INC)/system/core/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common 
        
        
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 #-DLOG #-g  #-fvisibility=hidden 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMULTITHREAD  -DPLATFORM=8
LOCAL_CFLAGS +=-DPACKAGE_NAME=com_jumpplayer_object -DPACKAGE_DIR=\"com/jumplayer/object\"

LOCAL_LDLIBS :=-L$(call host-path,$(LOCAL_PATH)/aosp-f) \
               -lutils \
               -lsurfaceflinger_client \
               -lmedia                 \
               -L$($(SYSROOT)/usr/lib) \
               -ldl                    \
               -llog
LOCAL_PRELINK_MODULE :=false
LOCAL_SHARED_LIBRARIES := libcore 

LOCAL_SRC_FILES := \
        		player-jni.cpp \
        		overlay_vsurface.c \
        		vsurface_wrap.cpp \
        		waveout_android.c \
        		wave_audiotrack.cpp \
        		mediainfo.cpp
        		
include $(BUILD_SHARED_LIBRARY)

ANDROID23_SYS_INC := $(LOCAL_PATH)/aosp-g

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := player-g-jni #<system ver >== 9 (2.3)
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(ANDROID23_SYS_INC)/frameworks/base/native/include \
		$(ANDROID23_SYS_INC)/frameworks/base/include \
		$(ANDROID23_SYS_INC)/hardware/libhardware/include \
    	$(ANDROID23_SYS_INC)/system/core/include \
    	$(ANDROID23_SYS_INC)/system/media/opensles/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common 
        
        
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 #-DLOG #-g  #-fvisibility=hidden 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMULTITHREAD  -DPLATFORM=9 -DUSESLES
LOCAL_CFLAGS +=-DPACKAGE_NAME=com_jumplayer_object -DPACKAGE_DIR=\"com/jumplayer/object\"

LOCAL_LDLIBS :=-L$(call host-path,$(LOCAL_PATH)/aosp-g) \
               -lutils \
               -lsurfaceflinger_client \
               -lmedia                 \
               -lOpenSLES              \
               -L$($(SYSROOT)/usr/lib) \
               -ldl                    \
               -llog
LOCAL_PRELINK_MODULE :=false
LOCAL_SHARED_LIBRARIES := libcore 

LOCAL_SRC_FILES := \
        		player-jni.cpp \
        		overlay_vsurface.c \
        		vsurface_wrap.cpp \
        		waveout_android.c \
        		wave_audiotrack.cpp \
        		wave_audioopensles.cpp \
        		mediainfo.cpp
        		
include $(BUILD_SHARED_LIBRARY)


ANDROID40_SYS_INC := $(LOCAL_PATH)/aosp-i

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := player-i-jni #<system ver >== 14 (4.0)
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(ANDROID40_SYS_INC)/frameworks/base/opengl/include \
		$(ANDROID40_SYS_INC)/frameworks/base/native/include \
		$(ANDROID40_SYS_INC)/frameworks/base/include \
		$(ANDROID40_SYS_INC)/hardware/libhardware/include \
    	$(ANDROID40_SYS_INC)/system/core/include \
    	$(ANDROID40_SYS_INC)/system/media/wilhelm/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common 
        
        
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 #-DLOG #-g  #-fvisibility=hidden 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMULTITHREAD  -DPLATFORM=14 -DUSESLES
LOCAL_CFLAGS +=-DPACKAGE_NAME=com_jumplayer_object -DPACKAGE_DIR=\"com/jumplayer/object\"

LOCAL_LDLIBS :=-L$(call host-path,$(LOCAL_PATH)/aosp-i) \
               -lutils \
               -lgui \
               -lmedia \
               -lOpenSLES              \
               -L$($(SYSROOT)/usr/lib) \
               -ldl                    \
               -llog
LOCAL_PRELINK_MODULE :=false
LOCAL_SHARED_LIBRARIES := libcore 

LOCAL_SRC_FILES := \
        		player-jni.cpp \
        		overlay_vsurface.c \
        		vsurface_wrap.cpp \
        		waveout_android.c \
        		wave_audiotrack.cpp \
        		wave_audioopensles.cpp \
        		mediainfo.cpp
        		
include $(BUILD_SHARED_LIBRARY)

ANDROID41_SYS_INC := $(LOCAL_PATH)/aosp-j

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)

LOCAL_MODULE    := player-j-jni #<system ver >== 16 (4.1,4.2)
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(ANDROID41_SYS_INC)/frameworks/av/include \
		$(ANDROID41_SYS_INC)/frameworks/native/opengl/include \
		$(ANDROID41_SYS_INC)/frameworks/native/include \
    	$(ANDROID41_SYS_INC)/frameworks/wilhelm/include \
		$(ANDROID41_SYS_INC)/hardware/libhardware/include \
    	$(ANDROID41_SYS_INC)/system/core/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/../common 
        
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm
LOCAL_CFLAGS += -fPIC -O2 #-DLOG #-g  #-fvisibility=hidden 
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMULTITHREAD  -DPLATFORM=16 #-DUSESLES
LOCAL_CFLAGS +=-DPACKAGE_NAME=com_jumplayer_object -DPACKAGE_DIR=\"com/jumplayer/object\"

LOCAL_LDLIBS :=-L$(call host-path,$(LOCAL_PATH)/aosp-j) \
               -lutils \
               -lgui \
               -lmedia \
               -lOpenSLES              \
               -L$($(SYSROOT)/usr/lib) \
               -ldl                    \
               -llog
LOCAL_PRELINK_MODULE :=false
LOCAL_SHARED_LIBRARIES := libcore 

LOCAL_SRC_FILES := \
        		player-jni.cpp \
        		overlay_vsurface.c \
        		vsurface_wrap.cpp \
        		waveout_android.c \
        		wave_audiotrack.cpp \
        		wave_audioopensles.cpp \
        		mediainfo.cpp
        		
include $(BUILD_SHARED_LIBRARY)
else
include $(all-subdir-makefiles)
endif