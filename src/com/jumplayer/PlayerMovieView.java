package com.jumplayer;

import java.io.File;
import java.util.Arrays;
import java.util.Vector;

import android.content.Context;
import android.content.res.Configuration;
import android.os.Handler;
import android.os.Message;
import android.telephony.TelephonyManager;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;
import android.view.View.OnKeyListener;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.widget.Toast;

import com.jumplayer.interfaces.PlayerStateListener;
import com.jumplayer.object.MediaAudioInfo;
import com.jumplayer.object.MediaCommentInfo;
import com.jumplayer.object.MediaInputInfo;
import com.jumplayer.object.MediaRenderInfo;
import com.jumplayer.object.MediaVideoInfo;
import com.jumplayer.object.PlayerCore;
import com.jumplayer.object.PlayerListener;
import com.jumplayer.utils.FileFilter;
import com.jumplayer.utils.Preferences;
import com.google.ads.*;
import com.google.ads.AdRequest.ErrorCode;

public class PlayerMovieView extends SurfaceView implements PlayerListener,SurfaceHolder.Callback, OnKeyListener 
{
	//////////////////////////////////////////////////////////////////////////
	// constace
	public static final int	sfi_Core_Error_NotSupport = 0;
	public static final int sfi_Core_Subtitle = 1;
	public static final int sfi_Player_UpdateMediaInfo = 2;
	public static final int sfi_Player_UpdatePlayState = 4;
	public static final int sfi_Player_UpdatePlayTime = 5;
	public static final int sfi_Player_PlayOver = 6;
	//////////////////////////////////////////////////////////////////////////
	// error message entry
	class PlayerNotifyInfo
	{
		private String m_szFilePath;
		private String m_szInfo;
		public void setFilePath(String szFilePath) 
		{
			this.m_szFilePath = szFilePath;
		}
		public String getFilePath() 
		{
			return m_szFilePath;
		}
		public void setErrorInfo(String szErrorInfo) 
		{
			m_szInfo = szErrorInfo;
		}
		public String getErrorInfo() 
		{
			return m_szInfo;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////
	// error handler
	class PlayerHandler extends Handler
	{
		public synchronized void handleMessage(Message msg) 
		{
			switch(msg.what) 
			{
			case sfi_Core_Error_NotSupport:
				{
					PlayerNotifyInfo info = (PlayerNotifyInfo)msg.obj;
					m_vUnSupportFiles.add(info.getFilePath());
					
					showErrorInfo(info);
					if(mPlayer!=null)
					{
						mPlayer.Stop(1);
					}
					PlayNext();
				}
				break;
			case sfi_Core_Subtitle:
				{
					PlayerNotifyInfo info = (PlayerNotifyInfo)msg.obj;
					if(m_CtrlSub!= null)
					 {
						int iState = m_CtrlSub.getState();
						if(iState == PlayerMovieControllor.m_stfState_Dismiss)
							m_CtrlSub.doShowControllor();
						m_CtrlSub.doShowSubtile(info.m_szInfo);
					 }
				}
				break;
			case sfi_Player_UpdateMediaInfo:
				{
					if(m_CtrlBar != null)
						m_CtrlBar.doFillMediaInfo(m_CtrlBar.getIsShowingMediaInfo());
				}
				break;
			case sfi_Player_UpdatePlayState:
				{
					if(m_CtrlBar!=null)
					{
			 			m_CtrlBar.doUpdatePlayState();
					}
				}
				break;
			case sfi_Player_UpdatePlayTime:
				{
					if(m_CtrlBar!=null)
					{
			 			m_CtrlBar.doUpdatePlayTime();
					}
				}
				break;
			case sfi_Player_PlayOver:
				{
					if(m_CtrlBar != null &&m_CtrlBar.getState() == PlayerMovieControllor.m_stfState_Showing)
					 {
						 m_CtrlBar.doDismissControllor();
					 }
					 if(m_CtrlSub!=null &&m_CtrlSub.getState() == PlayerMovieSub.m_stfState_Showing)
					 {
						 m_CtrlSub.doDismissControllor();
					 }	
					 if(m_CtrlBar!=null)
					 {
						m_CtrlBar.UninitControllor();
						m_CtrlBar = null;
					 }
					 if(m_CtrlSub!=null)
					 {
						m_CtrlSub.UninitControllor();
						m_CtrlSub = null;
					 }
					((PlayerActivity)mContext).finish();
				}
				break;
			default:
				break;
			}
			super.handleMessage(msg);
		}
		
		private void showErrorInfo(PlayerNotifyInfo info)
		{
			String szError = "";
			File file = new File(info.getFilePath());
			if(file != null)
				szError = file.getName() + ": \r\n";
			szError += info.getErrorInfo();
			
			Toast.makeText(mContext.getApplicationContext(), szError, Toast.LENGTH_SHORT).show();									
		}
	}	
	
	private static final String 		TAG = "JumPlayer"; 
	private Context						mContext;
	public  PlayerCore 			mPlayer;
	private SurfaceHolder 				mSurfaceHolder;
	public  String						mFileName;
	private boolean 					m_bHasError;
	private int	mOrientation = Configuration.ORIENTATION_LANDSCAPE;
		
	
	PlayerMovieControllor 		m_CtrlBar;
	PlayerMovieSub				m_CtrlSub;
	private PlayerHandler			m_hPlayerNotify;
	boolean								m_bOpened;
	private boolean						m_PlayState;//for phone
	
	private Vector<String>				m_vUnSupportFiles;
		
	private PlayerStateListener			mPlayerStateListen;
	
	public PlayerMovieView(Context context) {
		super(context);
		mContext = context;
		mSurfaceHolder = getHolder();
		mSurfaceHolder.addCallback(this);
		mOrientation = this.getResources().getConfiguration().orientation ;
		
		m_hPlayerNotify 	= new PlayerHandler();
		m_bOpened = false;
		
		m_vUnSupportFiles = new Vector<String>();
		
		if (mPlayer == null) 
		{
			mPlayer = PlayerCore.getInstance();
		}
		m_CtrlBar = new PlayerMovieControllor(this);
		m_CtrlSub = new PlayerMovieSub(this);
	}
	
	public PlayerMovieView(Context context, AttributeSet attrs)
	{
		super(context, attrs);
		
		mContext = context;
		mSurfaceHolder = getHolder();
		mSurfaceHolder.addCallback(this);
		mOrientation = this.getResources().getConfiguration().orientation ;
		
		m_hPlayerNotify 	= new PlayerHandler();
		m_bOpened = false;
		
		m_vUnSupportFiles = new Vector<String>();
		
		if (mPlayer == null) 
		{
			mPlayer = PlayerCore.getInstance();
		}
		m_CtrlBar = new PlayerMovieControllor(this);
		m_CtrlSub = new PlayerMovieSub(this);			
	}
	
	public Context getMovieContext()
	{
		return mContext;
	}
	
	public void SetPlayerStateListen(PlayerStateListener Listen)
	{
		mPlayerStateListen = Listen;
	}
	public void Open(String szFileName,boolean TheSameDoNoting)
	{
		if(mFileName!=null && mFileName.compareTo(szFileName)==0)
		{
			if(TheSameDoNoting)
				return ;
		}
	
		if(mPlayer.IsPlaying())
			mPlayer.Stop(1);
		
		mPlayer.SetPlayerListener(this);
		mFileName = new String(szFileName);
		m_bOpened = false;
	
	}
	public void Pause()
	{
		if(mPlayer != null)
		{
			mPlayer.Pause();
			if(mPlayerStateListen != null)
			{
				mPlayerStateListen.PlayerState(PlayerCore.PLAY_STATE_PAUSE);
			}
		}
	}
	public void Play()
	{
		if(mPlayer != null)
		{
			mPlayer.Play();
		}
	}
	public boolean IsPlaying()
	{
		if(mPlayer != null)
		{
			return mPlayer.IsPlaying();
		}
		return false;
	}
	public void SeekTo(int msec)
	{
		if(mPlayer != null)
		{
			mPlayer.SeekTo(msec);
		}
	}
	public int GetCurrentPosition()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetCurrentPosition();
		}
		
		return -1;
	}
	public int GetDuration()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetDuration();
		}
		
		return -1;
	}
	public void SetVideoMode(int VideoMode)
	{
		if(mPlayer != null)
		{
			mPlayer.SetVideoMode(VideoMode);
		}
		
	}
	//�õ���ǰ�����ļ����ĸ���
	public int GetAllStreamCount()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetAllStreamCount();
		}
		return 0;
	}
	//�õ�ĳ��STREAM_VIDEO,STREAM_AUDIO,STREAM_SUB��������
	public int GetStreamCount(int StreamType)
	{
		if(mPlayer != null)
		{
			return mPlayer.GetStreamCount(StreamType);
		}
		return 0;
	}
	//�Ƿ���ĳ��STREAM_VIDEO,STREAM_AUDIO,STREAM_SUB����
	public boolean  HasStream(int StreamType)
	{
		if(mPlayer != null)
		{
			return mPlayer.HasStream(StreamType);
		}
		return false;
	}
	public  MediaInputInfo GetMediaInputInfo()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetMediaInputInfo();
		}
		return null;
		
	}
	//�õ�MediaCommentInfo ��Ϣ
	public  MediaCommentInfo GetMediaCommentInfo()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetMediaCommentInfo();
		}
		return null;
	}
	//�õ�MediaVideoInfo ��Ϣ
	public  MediaVideoInfo[] GetMediaVideoInfo()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetMediaVideoInfo();
		}
		return null;
	}
	//�õ�MediaAudioInfo ��Ϣ
	public  MediaAudioInfo[] GetMediaAudioInfo()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetMediaAudioInfo();
		}
		return null;
	}
	//�õ�MediaRenderInfo ��Ϣ
	public  MediaRenderInfo GetMediaRenderInfo()
	{
		if(mPlayer != null)
		{
			return mPlayer.GetMediaRenderInfo();
		}
		return null;
	}
	public void Close()
	{
		if(mPlayer != null)
		{
			if(mPlayer.IsPlaying())
			{
				mPlayer.Stop(1);
			}
			mPlayer.SetVideoSurface(null);
			mPlayer.SetPlayerListener(null);
			mPlayer.Destory();
			mPlayer = null;
		}
	}

	public String getNextMedia(String szFileName)
	{
		if(Preferences.getInstance(mContext).getPlayModeType() == Preferences.g_iPlayMode_Loop)
		{
			return findNextMedia(szFileName);
		}
		else if(Preferences.getInstance(mContext).getPlayModeType() == Preferences.g_iPlayMode_Single)
		{
			if(checkSupport(szFileName))
				return szFileName;
		}

		return null;
	}
		
	private String findNextMedia(String szCurFile)
	{
		File file = new File(szCurFile);
		if(file == null)
			return null;
		
		File fParent = file.getParentFile();
		if(fParent != null)
		{
			File[] list = fParent.listFiles(new FileFilter(mPlayer));
			if (list != null)
			{
				Arrays.sort(list, FileFilter.FileNameComparator);
			}
			else 
			{
				return null;
			}
			
			boolean bFound = false;
			for (File f : list) 
			{
				if (f != null && f.isFile() && !f.isHidden() && f.getAbsolutePath().compareTo(szCurFile) == 0) 
				{
					bFound = true;
				}
				else if(bFound && f != null && f.isFile() && !f.isHidden())
				{
					if(checkSupport(f.getAbsolutePath()))
						return new String(f.getAbsolutePath());
				}
			}
			
			for (File f : list)
			{
				if(f != null && f.isFile() && !f.isHidden() && checkSupport(f.getAbsolutePath()))
					return new String(f.getAbsolutePath());
			}
		}
		
		return null;		
	}
	
	public boolean checkSupport(String szPath)
	{
		if(m_vUnSupportFiles == null || m_vUnSupportFiles.size() == 0)
			return true;
		
		for(int i=0; i<m_vUnSupportFiles.size(); i++)
			if(m_vUnSupportFiles.get(i).compareTo(szPath) == 0)
				return false;
		
		return true;
	}
	
	public void onConfigurationChanged(Configuration newConfig)
	{
		if(newConfig.orientation != mOrientation)
		{
			mOrientation = newConfig.orientation;
		}
	}
	public void surfaceChanged(SurfaceHolder holder, int format, int width,int height) {
			Surface sur = holder.getSurface();
			if(mPlayer != null)
			{
				mPlayer.VideoSurfaceChanged(sur);
				mPlayer.SetVideoMode(Preferences.getInstance(mContext).getFitVideoMode());
			}
	}

	
	public void surfaceDestroyed(SurfaceHolder holder) {
		
			if(mPlayer != null)
			{
				mPlayer.SetVideoSurface(null);
			}
			// after we return from this we can't use the surface any more
            mSurfaceHolder = null;
	}

	
	public void surfaceCreated(SurfaceHolder holder) {
		
		this.setOnKeyListener(this);
		
		Surface sur = holder.getSurface();
		if(mPlayer != null)
		{
			mPlayer.SetVideoSurface(sur);
			mPlayer.SetVideoMode(Preferences.getInstance(mContext).getFitVideoMode());
		}
		if(!m_bOpened)
		{
			Toast.makeText(mContext, mFileName, Toast.LENGTH_SHORT).show();
			doPlayMedia(mFileName);
		}
	}
	
	 public boolean onTouchEvent(android.view.MotionEvent event) 
	 {
		 int iAction = event.getAction();
		 if(iAction == android.view.MotionEvent.ACTION_UP && m_CtrlBar != null)
		 {
			 int iState = m_CtrlBar.getState();
			 if(iState == PlayerMovieControllor.m_stfState_Dismiss)
				 m_CtrlBar.doShowControllor();
			 else if(iState == PlayerMovieControllor.m_stfState_Showing)
				 m_CtrlBar.doDismissControllor();
		 }

		 return true;		 
	    }

		
	public void SubtitleNotify(String strSubTitle)
	{
		if(m_bHasError)
			return ;
		if(strSubTitle.length()>=0)
		{
			PlayerNotifyInfo Info = new PlayerNotifyInfo();
			Info.setErrorInfo(strSubTitle);
			Message msg = m_hPlayerNotify.obtainMessage(sfi_Core_Subtitle, 0, 0, Info);
			m_hPlayerNotify.sendMessage(msg);
		}
	}
	public void ErrorNotify(String strMsg) 
	{
		m_bHasError = true;
		PlayerNotifyInfo Info = new PlayerNotifyInfo();
		Info.setFilePath(mFileName);
		Info.setErrorInfo(strMsg);
		Message msg = m_hPlayerNotify.obtainMessage(sfi_Core_Error_NotSupport, 0, 0, Info);
		m_hPlayerNotify.sendMessage(msg);
		
		return ;
	}
	public void PlayerNotify(int MsgId) 
	{
		if(m_bHasError)
			return ;
		if(MsgId == 1)//PLAY_STATE_PLAY
		{
			if(mPlayerStateListen != null)
			{
				mPlayerStateListen.PlayerState(PlayerCore.PLAY_STATE_PLAY);
			}
		}
		else if(MsgId == 2)//PLAY_STATE_PAUSE
		{
			
		}
		else if(MsgId == 3)//PlayerCore::PLAY_STATE_PLAY_SYNC)
		{
			//��һ��PLAY_STATE_PLAY_SYNC��ϢΪ�򿪺�ɹ���Ⱦ��һ֡ý��������Ⱦ���
			//�Ժ�Ϊ�϶����һ֡ý��������Ⱦ���
			//����������Ĺ���
			//1.�򿪺�����Ƿ���������
			//run or pause after media render sucess
			//mPlayer.Pause();
			//2.��ȡý���ļ���Ϣ �˴����û����������
			//get the media info 
			Message msg = m_hPlayerNotify.obtainMessage(sfi_Player_UpdateMediaInfo, 0, 0, null);
			m_hPlayerNotify.sendMessage(msg);
			msg = m_hPlayerNotify.obtainMessage(sfi_Player_UpdatePlayState, 0, 0, null);
			m_hPlayerNotify.sendMessage(msg);
			msg = m_hPlayerNotify.obtainMessage(sfi_Player_UpdatePlayTime, 0, 0, null);
			m_hPlayerNotify.sendMessage(msg);
		}
		else if(MsgId == 4)//PlayerCore::PLAY_STATE_PLAY_COMPLETE)
		{
			//PLAY_STATE_PLAY_COMPLETE ý���ļ�������Ϸ�������Ϣ
			//�˴������������飬
			//1.���ݲ����б���ģʽ���ظ����ŵ�ǰ�ļ���������һ��
			//mPlayer.Stop();//�ڲ��Ѿ�����Stop��������ֹ���Ļ᲻�ϲ������������Ϣ��ԭ����)������Reset��Surface
			
			PlayNext();
		}
		return ;
	}	
	
	public void PlayNext()
	{
		// get next media
		String szFileName = getNextMedia(mFileName);
		if(szFileName != null)
		{
			doPlayMedia(szFileName);
		}
		else
		{
			Message msg = m_hPlayerNotify.obtainMessage(sfi_Player_PlayOver, 0, 0, null);
			m_hPlayerNotify.sendMessage(msg);
		}		
	}
	
	public void doPlayMedia(String szPath)
	{
		m_bOpened = true;
		mFileName = szPath;
		m_bHasError = false;
		mPlayer.OpenURL(szPath);
	}

	public boolean onKey(View v, int keyCode, KeyEvent event) 
	{
		if(keyCode == KeyEvent.KEYCODE_BACK)
		{
			Message msg = m_hPlayerNotify.obtainMessage(sfi_Player_PlayOver, 0, 0, null);
			m_hPlayerNotify.sendMessage(msg);
		}
		return false;
	}
	
	public void onCallStateChanged(int state, String incomingNumber)
	{
		switch(state){  
        case TelephonyManager.CALL_STATE_RINGING:  
        	if(mPlayer!=null)
        	{
        		if(mPlayer.IsPlaying())
	        	{
	        		m_PlayState = true;
	        		mPlayer.Pause();
	        	}
	        	else
	        		m_PlayState = false;
        	}
        	
            break;  
        case TelephonyManager.CALL_STATE_IDLE:   
        	if(mPlayer!=null)
        	{
        		if(m_PlayState)
	        	{
	        		mPlayer.Play();
	        	}
        	}
            break;  
        case TelephonyManager.CALL_STATE_OFFHOOK:  
            break;  
        } 
	}
}
