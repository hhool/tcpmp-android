package com.jumplayer.object;


import com.jumplayer.PlayerEnvironment;
import com.jumplayer.StartupActivity;


import android.os.Build;
import android.util.Log;
import android.view.Surface;
//���Žӿڡ�

public class PlayerCore{ 
	  
	private static PlayerCore sInstance;
	private static final String TAG = "JumPlayer";
	private static int  sCoreInstance      = 0;
	
	public static final int VIDEO_MODE_HALF = 0;
	public static final int VIDEO_MODE_FIT = 1;
	public static final int VIDEO_MODE_FILL = 2;
	
	public static final int STREAM_VIDEO = 1;//��Ƶ��
	public static final int STREAM_AUDIO = 2;//��Ƶ��
	public static final int STREAM_SUBTITLE = 3;//��Ļ����Ŀǰ��֧����Ļ
		
	public static final int PLAY_STATE_PLAY = 1;//����״̬
	public static final int PLAY_STATE_PAUSE= 2;//��ͣ״̬
	public static final int PLAY_STATE_PLAY_SYNC = 3;//ý���ļ���Ⱦ����һ֡�����϶���ĵ�һ֡�ᷢ������Ϣ��
	public static final int PLAY_STATE_PLAY_COMPLETE= 4;//ý���ļ�������Ϸ�������Ϣ
	
	public static final int LANG_EN 	=1;
	public static final int LANG_CHS 	=2;

	public static final int THUMB_RGB32	=4;
	public static final int THUMB_RGB565=2;
	static
	{
		//����ϵͳ�汾������Ӧ�汾��JNI�ӿڿ⡣	
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
	//���ź��Ļ�����ʼ�����ڳ�������ʱ����
	private native int ContextInit(PlayerEnvironment env);
	//���ź��Ļ�������ʼ�����ڳ����˳�ʱ����
	private native int ContextDone();
	//��ý���ļ�
	public native int OpenURL(final String url);
	//����
	public native int Play();
	//��ͣ
	public native int Pause();
	//ֹͣ
	public native int Stop(int NoFillBuf);
	/**
	* Update the MediaPlayer ISurface. Call after updating mSurface.
	*/
	//�趨��������Ƶͼ����Ⱦ��ͼ��surface����ֵΪ������ʾ��Ƶͼ��
	public native void SetVideoSurface(Surface surface);
	//surface��Ⱦͼ�㷢���仯ʱ��֪ͨ�������ӿڷ�����
	public native void VideoSurfaceChanged(Surface surface);
	//�趨ͼ�����ʾ��ʽVIDEO_MODE_FIT��VIDEO_MODE_FILL��
	public native void SetVideoMode(int VideoMode);
	//ͼ����ת��Ŀǰδʵ��
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
	//�õ���Ƶͼ��Ľ����ʵ�ʿ����Ϣ//������
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
	//�õ���Ƶͼ��Ľ����ʵ�ʸ߶���Ϣ//������
	public native int GetVideoHeight();
		
	/**
	* Checks whether the MediaPlayer is playing.
	*
	* @return true if currently playing, false otherwise
	*/
	//�Ƿ����ڲ���״̬
	public native boolean IsPlaying();
		
	/**
	* Seeks to specified time position.
	*
	* @param msec the offset in milliseconds from the start to seek to
	* @throws IllegalStateException if the internal player engine has not been
	* initialized
	*/
	//ʵʩ�϶���������λΪ���� 0.001��
	public native void SeekTo(int msec) throws IllegalStateException;
		
	/**
	* Gets the current playback position.
	*
	* @return the current position in milliseconds
	*/
	//�õ���ǰ�Ĳ���λ�� ��λΪ����0.001��
	public native int GetCurrentPosition();
		
	/**
	* Gets the duration of the file.
	*
	* @return the duration in milliseconds
	*/
	//�õ�ý����ܲ���ʱ������λ����0.001��
	public native int GetDuration();
	//�趨������С��δʵ��
	public native void SetVolume(int nVol);
	//�õ�������С��δʵ�� 
	public native int  GetVolume();
	//url�ļ��Ƿ񱻲�����֧�֡�
	public native boolean IsFileSupport(final String url);
	//�õ�����֧�ֵ�ý���ļ���׺��
	public native String GetSupportFileSuffix();
	//�õ���ǰ�����ļ����ĸ���
	public native int GetAllStreamCount();
	//�õ�ĳ��STREAM_VIDEO,STREAM_AUDIO,STREAM_SUB��������
	public native int GetStreamCount(int StreamType);
	//�Ƿ���ĳ��STREAM_VIDEO,STREAM_AUDIO,STREAM_SUB����
	public native boolean  HasStream(int StreamType);
	//�õ�MediaInputInfo ��Ϣ
	public native MediaInputInfo GetMediaInputInfo();
	//�õ�MediaCommentInfo ��Ϣ
	public native MediaCommentInfo GetMediaCommentInfo();
	//�õ�MediaVideoInfo ��Ϣ
	public native MediaVideoInfo[] GetMediaVideoInfo();
	//�õ�MediaAudioInfo ��Ϣ
	public native MediaAudioInfo[] GetMediaAudioInfo();
	//�õ�MediaRenderInfo ��Ϣ
	public native MediaRenderInfo GetMediaRenderInfo();
	//�趨PlayerListener
	public native int	SetPlayerListener(PlayerListener listen);
	//��ȡý���ļ���ͼ��int RGBFlag,�������ָ���߿��RGB32,RGB565λ��λͼͼ������
	// ָ��Ϊ0 ��  1000ms
	public native byte[] GetThumbnail(String URL, int i_width, int i_height,int RGBFlag,int msec);
	
	} 
