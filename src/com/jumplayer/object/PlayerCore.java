package com.jumplayer.object;


import com.jumplayer.PlayerEnvironment;
import com.jumplayer.StartupActivity;


import android.os.Build;
import android.util.Log;
import android.view.Surface;
//播放接口。

public class PlayerCore{ 
	  
	private static PlayerCore sInstance;
	private static final String TAG = "JumPlayer";
	private static int  sCoreInstance      = 0;
	
	public static final int VIDEO_MODE_HALF = 0;
	public static final int VIDEO_MODE_FIT = 1;
	public static final int VIDEO_MODE_FILL = 2;
	
	public static final int STREAM_VIDEO = 1;//视频流
	public static final int STREAM_AUDIO = 2;//音频流
	public static final int STREAM_SUBTITLE = 3;//字幕流，目前不支持字幕
		
	public static final int PLAY_STATE_PLAY = 1;//播放状态
	public static final int PLAY_STATE_PAUSE= 2;//暂停状态
	public static final int PLAY_STATE_PLAY_SYNC = 3;//媒体文件渲染出第一帧或者拖动后的第一帧会发出此消息。
	public static final int PLAY_STATE_PLAY_COMPLETE= 4;//媒体文件播放完毕发出此消息
	
	public static final int LANG_EN 	=1;
	public static final int LANG_CHS 	=2;

	public static final int THUMB_RGB32	=4;
	public static final int THUMB_RGB565=2;
	static
	{
		//根据系统版本加载相应版本的JNI接口库。	
		try 
	   	{
	   		int version = Integer.parseInt(Build.VERSION.SDK);
			if (version < 8) 
			{
		   		System.loadLibrary("core");
		   		System.loadLibrary("player-d-jni");
			}
	   		else if(version ==8)
	   		{
	   			System.loadLibrary("core");
		   		System.loadLibrary("player-f-jni");
	   		}
	   		else if(version >=9 && version<14)
	   		{
	   			System.loadLibrary("core");
		   		System.loadLibrary("player-g-jni");
	   		}
			else if(version >= 14 && version < 16)
	   		{
				System.loadLibrary("core");
	   			System.loadLibrary("player-i-jni");
	   		}
	   		else
	   		{
	   			System.loadLibrary("core");
	   			System.loadLibrary("player-j-jni");
	   		}            
	   	} 
	   	catch (UnsatisfiedLinkError ule)
	   	{
	   		Log.e(TAG, "Could not load library: " + ule);
		}
	   	catch (SecurityException se) 
	   	{
            Log.e(TAG, "Encountered a security issue when loading library: " + se);
        }
	}
	
	public static PlayerCore getInstance()
	{
		if (sInstance == null) 
		{
		   	/* First call */
		sInstance = new PlayerCore();
		sInstance.ContextInit(StartupActivity.g_PlayerEnv);
			
		}
        sCoreInstance++;
        
        Log.i(TAG, "Player Instance Num ++ " + sCoreInstance);
        
    	return sInstance;
	}
	public void Destory()
    {
		sCoreInstance--;
		
		Log.i(TAG, "Player Instance Num  -- " + sCoreInstance);
        
    }
	//播放核心环境初始化，在程序启动时调用
	private native int ContextInit(PlayerEnvironment env);
	//播放核心环境反初始化，在程序退出时调用
	private native int ContextDone();
	//打开媒体文件
	public native int OpenURL(final String url);
	//播放
	public native int Play();
	//暂停
	public native int Pause();
	//停止
	public native int Stop(int NoFillBuf);
	/**
	* Update the MediaPlayer ISurface. Call after updating mSurface.
	*/
	//设定播放器视频图像渲染的图层surface，此值为空则不显示视频图像
	public native void SetVideoSurface(Surface surface);
	//surface渲染图层发生变化时，通知播放器接口方法，
	public native void VideoSurfaceChanged(Surface surface);
	//设定图像的显示方式VIDEO_MODE_FIT，VIDEO_MODE_FILL，
	public native void SetVideoMode(int VideoMode);
	//图像旋转，目前未实现
	public native void Rotate(int type);
	/**
	* Returns the width of the video.
	*
	* @return the width of the video, or 0 if there is no video,
	* no display surface was set, or the width has not been determined
	* yet. The OnVideoSizeChangedListener can be registered via
	* {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
	* to provide a notification when the width is available.
	*/
	//得到视频图像的解码后实际宽度信息//不可用
	public native int GetVideoWidth();
			
	/**
	* Returns the height of the video.
	*
	* @return the height of the video, or 0 if there is no video,
	* no display surface was set, or the height has not been determined
	* yet. The OnVideoSizeChangedListener can be registered via
	* {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
	* to provide a notification when the height is available.
	*/
	//得到视频图像的解码后实际高度信息//不可用
	public native int GetVideoHeight();
		
	/**
	* Checks whether the MediaPlayer is playing.
	*
	* @return true if currently playing, false otherwise
	*/
	//是否正在播放状态
	public native boolean IsPlaying();
		
	/**
	* Seeks to specified time position.
	*
	* @param msec the offset in milliseconds from the start to seek to
	* @throws IllegalStateException if the internal player engine has not been
	* initialized
	*/
	//实施拖动动作，单位为毫秒 0.001秒
	public native void SeekTo(int msec) throws IllegalStateException;
		
	/**
	* Gets the current playback position.
	*
	* @return the current position in milliseconds
	*/
	//得到当前的播放位置 单位为毫秒0.001秒
	public native int GetCurrentPosition();
		
	/**
	* Gets the duration of the file.
	*
	* @return the duration in milliseconds
	*/
	//得到媒体的总播放时长，单位毫秒0.001秒
	public native int GetDuration();
	//设定音量大小，未实现
	public native void SetVolume(int nVol);
	//得到音量大小，未实现 
	public native int  GetVolume();
	//url文件是否被播放器支持。
	public native boolean IsFileSupport(final String url);
	//得到核心支持的媒体文件后缀名
	public native String GetSupportFileSuffix();
	//得到当前播放文件流的个数
	public native int GetAllStreamCount();
	//得到某种STREAM_VIDEO,STREAM_AUDIO,STREAM_SUB的流个数
	public native int GetStreamCount(int StreamType);
	//是否含有某种STREAM_VIDEO,STREAM_AUDIO,STREAM_SUB的流
	public native boolean  HasStream(int StreamType);
	//得到MediaInputInfo 信息
	public native MediaInputInfo GetMediaInputInfo();
	//得到MediaCommentInfo 信息
	public native MediaCommentInfo GetMediaCommentInfo();
	//得到MediaVideoInfo 信息
	public native MediaVideoInfo[] GetMediaVideoInfo();
	//得到MediaAudioInfo 信息
	public native MediaAudioInfo[] GetMediaAudioInfo();
	//得到MediaRenderInfo 信息
	public native MediaRenderInfo GetMediaRenderInfo();
	//设定PlayerListener
	public native int	SetPlayerListener(PlayerListener listen);
	//提取媒体文件截图，int RGBFlag,输出的是指定高宽的RGB32,RGB565位的位图图像数据
	// 指定为0 到  1000ms
	public native byte[] GetThumbnail(String URL, int i_width, int i_height,int RGBFlag,int msec);
	
	} 
