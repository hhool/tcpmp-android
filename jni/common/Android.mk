LOCAL_PATH:= $(call my-dir)

cmd-strip = $(TOOLCHAIN_PREFIX)strip --strip-debug --strip-unneeded $(call cygwin-to-host-path,$1)

include $(CLEAR_VARS)


LOCAL_MODULE    := core
LOCAL_C_INCLUDES := \
		$(SYSROOT)/usr/include \
		$(LOCAL_PATH) \
        $(LOCAL_PATH)/android \
        $(LOCAL_PATH)/blit \
        $(LOCAL_PATH)/cpu \
        $(LOCAL_PATH)/dyncode \
        $(LOCAL_PATH)/pcm \
        $(LOCAL_PATH)/softidct \
        $(LOCAL_PATH)/zlib \
        $(LOCAL_PATH)/overlay \
        $(LOCAL_PATH)/ini 
       
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_ARM_MODE := arm 
LOCAL_CFLAGS +=  -fPIC -O2 -fvisibility=hidden  #-DLOG #-g -DDUMP   
LOCAL_CFLAGS +=-DTARGET_ANDROID -DMULTITHREAD  -DBUILDFIXED #-DREGISTRY_GLOBAL 
LOCAL_CFLAGS +=-DDYNAMIC_CRC_TABLE  -DZLIB_DLL -DFASTEST #-DNO_ERRNO_H
LOCAL_CFLAGS +=-DRES_IN_COMMOM 
LOCAL_CFLAGS +=-DINIPATH=\"data/data/com.jumplayer/player.ini\"
LOCAL_CFLAGS +=-DPACKAGEPATH=\"data/data/com.jumplayer\"

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl -llog

LOCAL_SRC_FILES := \
        advanced.c \
        audio.c \
        bitstrm.c \
        buffer.c \
        codec.c \
        color.c \
        context.c \
        dumpoutput.c \
        equalizer.c \
        flow.c \
        format.c \
        format_base.c \
        format_subtitle.c \
        gzip.c \
        helper_base.c \
        helper_video.c \
        id3tag.c \
        idct.c \
        node.c \
        nulloutput.c \
        overlay.c  \
        parser2.c  \
        platform.c  \
        player.c  \
        playlist.c  \
        probe.c  \
        rawaudio.c  \
        rawimage.c  \
        stdafx.c  \
        str.c  \
        streams.c  \
        tchar.c  \
        timer.c  \
        tools.c  \
        video.c  \
        vlc.c  \
        waveout.c \
        android/association_android.c \
        android/context_android.c \
        android/cpu-features.c	\
        android/file_android.c \
        android/mem_android.c \
        android/multithread_android.c \
        android/node_android.c \
        android/platform_android.c \
        android/platfom_android_jvm.cpp \
        android/res_android.c \
        android/str_android.c \
        android/waveout_android.c	\
        blit/blit_arm_fix.c  \
        blit/blit_arm_gray.c  \
        blit/blit_arm_half.c  \
        blit/blit_arm_packed_yuv.c  \
        blit/blit_arm_rgb16.c  \
        blit/blit_arm_stretch.c  \
        blit/blit_arm_yuv.c  \
        blit/blit_mips_fix.c  \
        blit/blit_mips_gray.c  \
        blit/blit_sh3_fix.c  \
        blit/blit_sh3_gray.c  \
        blit/blit_soft.c  \
        blit/blit_wmmx_fix.c \
        cpu/cpu.c \
        dyncode/dyncode.c \
        dyncode/dyncode_arm.c \
        ini/headings.c \
        ini/ini.c \
        ini/keys.c \
        ini/list.c \
        ini/types.c \
		overlay/overlay_thumb.c \
        pcm/pcm_arm.c \
        pcm/pcm_soft.c \
        playlist/asx.c \
        playlist/m3u.c \
        playlist/pls.c \
        softidct/block_c.c \
        softidct/block_half.c \
        softidct/block_mx1.c \
        softidct/idct_c.c \
        softidct/idct_half.c \
        softidct/mcomp4x4_c.c \
        softidct/mcomp_c.c \
        softidct/softidct.c \
        zlib/adler32.c \
        zlib/compress.c \
        zlib/crc32.c \
        zlib/gzio.c \
        zlib/infback.c \
        zlib/inffast.c \
        zlib/inflate.c \
        zlib/inftrees.c \
        zlib/uncompr.c \
        zlib/zutil.c \
        zlib/deflate.c \
        zlib/trees.c
include $(BUILD_SHARED_LIBRARY)
