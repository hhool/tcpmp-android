package com.jumplayer;

import java.text.SimpleDateFormat;
import java.util.TimeZone;
import java.util.Timer;
import java.util.TimerTask;

import com.jumplayer.object.MediaAudioInfo;
import com.jumplayer.object.MediaInputInfo;
import com.jumplayer.object.MediaVideoInfo;
import com.jumplayer.object.PlayerCore;
import com.jumplayer.utils.Preferences;

import android.content.Context;
import android.media.AudioManager;
import android.os.Handler;
import android.os.Message;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.ImageButton;
import android.widget.PopupWindow;
import android.widget.SeekBar;

public class PlayerMovieControllor implements OnTouchListener
											, OnKeyListener
											, OnClickListener
											, SeekBar.OnSeekBarChangeListener
{
	////////////////////////////////////////////////////////////
	// control bar state
	public static final int 	m_stfState_Showing = 0;
	public static final int 	m_stfState_Dismiss = 1;
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	// const value
	public static final int 	m_stfShowingPeriod = 2000;
	public static final int 	m_stfSeekStep	   = 15000; 
	////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////
	// timer message
	public static final int		m_stfMsg_Dismiss 		= 1000;
	public static final int		m_stfMsg_UpdatePlayTime = 1001;
	////////////////////////////////////////////////////////////	
	
	private PlayerMovieView	m_viewParent;
	private Context				m_Context;
	private AudioManager 		m_AudioManager;
	
	private View				m_viewControllor;
	private PopupWindow 		m_wndPopup;
	
	private int					m_iState;
	
	private Handler				m_hTimerMsgHandler;
	private Timer				m_TimerTask;
	//private PlayerCtrlTimer	m_TimerAntryShowing;
	
	private boolean				m_bShowMediaInfo;
	private int					m_iFitVideoMode;
	public int getState()
	{
		return m_iState;
	}
	
	public boolean getIsShowingMediaInfo()
	{
		return m_bShowMediaInfo;
	}
	
	public PlayerMovieControllor(PlayerMovieView parent)
	{
		m_viewParent = parent;
		m_Context = parent.getMovieContext();
		m_AudioManager = (AudioManager) m_Context.getSystemService(Context.AUDIO_SERVICE);
		
		m_bShowMediaInfo = false;
		m_iFitVideoMode = Preferences.getInstance(m_Context).getFitVideoMode();//PlayerCore.VIDEO_MODE_FIT;
		
		InitControllor();
		
		m_iState = m_stfState_Dismiss;
	}
	
	public void InitControllor()
	{
		LayoutInflater li = (LayoutInflater) (m_Context.getSystemService(Context.LAYOUT_INFLATER_SERVICE));
		m_viewControllor = (View) li.inflate(R.layout.gridview_menu, null, true);
				
		doInitListener();
		ImageButton btnViewpoint = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_viewpoint);
		if(btnViewpoint != null)
		{
			
		}
		switch(m_iFitVideoMode)
		{
		case PlayerCore.VIDEO_MODE_HALF:
			btnViewpoint.setImageResource(R.drawable.zoom_fit_48);
			break;
		case PlayerCore.VIDEO_MODE_FIT:
			btnViewpoint.setImageResource(R.drawable.zoom_fill_48);
			break;
		case PlayerCore.VIDEO_MODE_FILL:
			btnViewpoint.setImageResource(R.drawable.zoom_half_48);
			break;				
		}
		
		m_wndPopup = new PopupWindow(m_viewControllor, LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT, true);
//		m_wndPopup.setBackgroundDrawable(new BitmapDrawable());
//		m_wndPopup.setAnimationStyle(R.style.PopupAnimation);
		
		m_hTimerMsgHandler = new PlayerCtrlTimerHandler();
		
		m_TimerTask = new Timer();
		PlayerCtrlTimer timerantry = new PlayerCtrlTimer(m_stfMsg_UpdatePlayTime);
		m_TimerTask.schedule(timerantry, 0, 1000);	
	}
	
	public void UninitControllor()
	{
		m_TimerTask.cancel();
	}
	
	public void doShowControllor()
	{
		if(m_wndPopup != null)
		{
			m_wndPopup.showAtLocation(m_viewParent, Gravity.CENTER|Gravity.BOTTOM, 0, 0);
			m_iState = m_stfState_Showing;
			
			//m_TimerAntryShowing = new PlayerCtrlTimer(m_stfMsg_Dismiss);
			//m_TimerTask.schedule(m_TimerAntryShowing, m_stfShowingPeriod);	
		}
		//popupWindow.update();		
	}
	
	public void doDismissControllor()
	{
		if(m_wndPopup != null)
		{
			m_bShowMediaInfo = false;	//不显示媒体信息
			doFillMediaInfo(m_bShowMediaInfo);
			
			m_wndPopup.dismiss();
			m_iState = m_stfState_Dismiss;
		}
	}
	
	private void doInitListener()
	{
		m_viewControllor.setFocusableInTouchMode(true);
		
		m_viewControllor.setOnTouchListener(this);
		m_viewControllor.setOnKeyListener(this);
		
		ImageButton btnBackward = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_backward);
		if(btnBackward != null)
		{
			btnBackward.setImageResource(R.drawable.backward_48);
			btnBackward.setOnClickListener(this);
			btnBackward.setOnTouchListener(this);
		}

		ImageButton btnPlayPause = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_playpause);
		if(btnPlayPause != null)
		{
			btnPlayPause.setImageResource(R.drawable.pause_48);
			btnPlayPause.setOnClickListener(this);
			btnPlayPause.setOnTouchListener(this);
		}
		
		ImageButton btnForeward = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_foreward);
		if(btnForeward != null)
		{
			btnForeward.setImageResource(R.drawable.forward_48);
			btnForeward.setOnClickListener(this);
			btnForeward.setOnTouchListener(this);
		}

		ImageButton btnViewpoint = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_viewpoint);
		if(btnViewpoint != null)
		{
			btnViewpoint.setImageResource(R.drawable.zoom_fill_48);
			btnViewpoint.setOnClickListener(this);
			btnViewpoint.setOnTouchListener(this);
		}
		ImageButton btnMediaInfo = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_mediainfo);
		if(btnMediaInfo != null)
		{
			btnMediaInfo.setImageResource(R.drawable.mediainfo_48);
			btnMediaInfo.setOnClickListener(this);
			btnMediaInfo.setOnTouchListener(this);
		}
		
		SeekBar prg = (SeekBar)m_viewControllor.findViewById(R.id.thd_ctrl_bar_progress);
		if(prg != null)
		{
			prg.setOnSeekBarChangeListener(this);
			prg.setOnTouchListener(this);
		}
	}
	
	public boolean onTouch(View v, MotionEvent event) 
	{
		// TODO Auto-generated method stub		
		if(event.getAction() == MotionEvent.ACTION_DOWN)
		{
			if(v.getId() == R.id.thd_PlayerCtrlBar)
			{
				doDismissControllor();
			}
			//m_TimerAntryShowing.cancel();
		}
		else if(event.getAction() == MotionEvent.ACTION_UP)
		{
			//m_TimerAntryShowing = new PlayerCtrlTimer(m_stfMsg_Dismiss);
			//m_TimerTask.schedule(m_TimerAntryShowing, m_stfShowingPeriod);
		}
		return false;
	}	
	public boolean onKey(View v, int keyCode, KeyEvent event) 
	{
		// TODO Auto-generated method stub
		if (keyCode == KeyEvent.KEYCODE_BACK) 
		{
			this.m_viewParent.onKey(v, keyCode, event);
//			doDismissControllor();
			return true;
		} 
		else
		{
			if(event.getAction() == KeyEvent.ACTION_DOWN)
			{
				if(keyCode == KeyEvent.KEYCODE_VOLUME_UP)
					m_AudioManager.adjustVolume(AudioManager.ADJUST_RAISE, AudioManager.FLAG_SHOW_UI);
				if(keyCode == KeyEvent.KEYCODE_VOLUME_DOWN)
					m_AudioManager.adjustVolume(AudioManager.ADJUST_LOWER, AudioManager.FLAG_SHOW_UI);
			}
			return false;
		}
	}	
	
	public void onClick(View v) 
	{
		// TODO Auto-generated method stub
		int iID = v.getId();
		switch(iID)
		{
		case R.id.thd_ctrl_bar_backward:
			try
			{
				int iCurPos = m_viewParent.GetCurrentPosition();
				int iSeekTo = iCurPos - m_stfSeekStep;
				if(iSeekTo < 0)
					iSeekTo = 0;
				m_viewParent.SeekTo(iSeekTo);
			}
			catch(Exception e)
			{
			}
			finally
			{
			}
			break;
		case R.id.thd_ctrl_bar_foreward:
			try
			{
				int iCurPos = m_viewParent.GetCurrentPosition();
				int iSeekTo = iCurPos + m_stfSeekStep;
				if(iSeekTo > m_viewParent.GetDuration())
					iSeekTo = m_viewParent.GetDuration();
				m_viewParent.SeekTo(iSeekTo);
			}
			catch(Exception e)
			{
			}
			finally
			{
			}
			break;
		case R.id.thd_ctrl_bar_playpause:
			ImageButton btnPlayPause = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_playpause);
			if(m_viewParent.IsPlaying())
			{
				m_viewParent.Pause();
				btnPlayPause.setImageResource(R.drawable.play_48);
			}
			else
			{
				m_viewParent.Play();
				btnPlayPause.setImageResource(R.drawable.pause_48);				
			}
			break;
		case R.id.thd_ctrl_bar_viewpoint:
			doFitVideoMode();
			break;
		case R.id.thd_ctrl_bar_mediainfo:
			m_bShowMediaInfo = !m_bShowMediaInfo;
			doFillMediaInfo(m_bShowMediaInfo);
			break;
		}
	}

	public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) 
	{
		// TODO Auto-generated method stub
		seekBar.setProgress(progress);
		
	}

	public void onStartTrackingTouch(SeekBar seekBar) 
	{
		// TODO Auto-generated method stub
		
	}

	public void onStopTrackingTouch(SeekBar seekBar) 
	{
		// TODO Auto-generated method stub
		double dRation = (double)seekBar.getProgress() / seekBar.getMax();
		int iSeek = (int)(m_viewParent.GetDuration() * dRation);
		m_viewParent.SeekTo(iSeek);
	}
	
	public void doFitVideoMode()
	{
		ImageButton btnViewpoint = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_viewpoint);
		switch(m_iFitVideoMode)
		{
		case PlayerCore.VIDEO_MODE_HALF:
			{
				m_iFitVideoMode = PlayerCore.VIDEO_MODE_FIT;
				if(btnViewpoint != null)
					btnViewpoint.setImageResource(R.drawable.zoom_fill_48);
			}
			break;
		case PlayerCore.VIDEO_MODE_FIT:
			{
				m_iFitVideoMode = PlayerCore.VIDEO_MODE_FILL;
				if(btnViewpoint != null)
					btnViewpoint.setImageResource(R.drawable.zoom_half_48);
			}
			break;
		case PlayerCore.VIDEO_MODE_FILL:
			{
				m_iFitVideoMode = PlayerCore.VIDEO_MODE_HALF;
				if(btnViewpoint != null)
					btnViewpoint.setImageResource(R.drawable.zoom_fit_48);
			}
			break;				
		}
		m_viewParent.SetVideoMode(m_iFitVideoMode);		
	}
	
	public synchronized void doFillMediaInfo(boolean bShowInfo)
	{
		LableView infoview = (LableView)m_viewControllor.findViewById(R.id.thd_mediainfotext);
		String info = new String();
		if(bShowInfo)
		{
			MediaInputInfo InputInfo = m_viewParent.GetMediaInputInfo();
			if(InputInfo == null ||InputInfo.mFileName==null)
				return ;
			info += "File: "  + InputInfo.mFileName;
			info +=  "\n";
			info += "Format: " + InputInfo.mFormat;
			info +=  "\n";
			info += "Size: " + InputInfo.mSize+"KB";

			SimpleDateFormat fmTotalTime = new SimpleDateFormat("HH:mm:ss");
	 		fmTotalTime.setTimeZone(TimeZone.getTimeZone("GMT"));
	 		String szDuration = fmTotalTime.format(new Integer(InputInfo.mDuration));
	 		if(InputInfo.mDuration>0)
			{
	 			info += "   Duration: " + szDuration;
			}
			info +=  "\n";
	
			MediaVideoInfo[] VideoInfo = m_viewParent.GetMediaVideoInfo();
			
			int length = VideoInfo.length;
			int i = 0;
			for( ; i < length &&  VideoInfo != null && VideoInfo[i]!=null; i++)
			{
				info += "Video: "  + VideoInfo[i].mStreamID+" ";
				info += "Format: " + VideoInfo[i].mFormat;
				info += " [" + VideoInfo[i].mWidth + "x"+ VideoInfo[i].mHeight;
				if(VideoInfo[i].mByteRate>0)
					info += "@" + VideoInfo[i].mByteRate+"kbps";
				info += "@" + VideoInfo[i].mFps + "/s]";
				info +=  "\n";
				if(VideoInfo[i].mCodec!=null&&VideoInfo[i].mCodec.length()>0)
				{
					info += "Codec: "  + VideoInfo[i].mCodec;
					info +=  "\n";
				}
			}
			MediaAudioInfo[] AudioInfo = m_viewParent.GetMediaAudioInfo();
			length = AudioInfo.length;
			i = 0;
			for( ; i < length && AudioInfo != null && AudioInfo[i]!=null; i++)
			{
				info += "Audio: " + AudioInfo[i].mStreamID+" ";
				info += "Format: " + AudioInfo[i].mFormat;
				info += " [";
				if(AudioInfo[i].mByteRate>0)
				{
					info += AudioInfo[i].mByteRate +"kbps";
					info += "@";
				}
				info +=  AudioInfo[i].mChannels + "ch";
				info += "@" + AudioInfo[i].mSampleRate + "Hz]";
				info +=  "\n";
				
				if(AudioInfo[i].mCodec!=null&&AudioInfo[i].mCodec.length()>0)
				{
					info += "Codec: " + AudioInfo[i].mCodec;
					info +=  "\n";
				}
			}
		}
		infoview.setText(info.toString());		
	}
	public synchronized void doUpdatePlayState()
	{
		try
		{
			ImageButton btnPlayPause = (ImageButton)m_viewControllor.findViewById(R.id.thd_ctrl_bar_playpause);
			if(m_viewParent.IsPlaying())
			{
				btnPlayPause.setImageResource(R.drawable.pause_48);
			}
			else
			{
				btnPlayPause.setImageResource(R.drawable.play_48);				
			}
		}
		catch(Exception ex)
		{
		}
		finally
		{
		}
	}
	public synchronized void doUpdatePlayTime()
	{
		try
		{
			int iPosition = m_viewParent.GetCurrentPosition();
			int iDuration = m_viewParent.GetDuration();
			
			LableView curtime = (LableView)m_viewControllor.findViewById(R.id.curtime);
			SeekBar	prg  = (SeekBar)m_viewControllor.findViewById(R.id.thd_ctrl_bar_progress);
		
	 		SimpleDateFormat fmPlayTime = new SimpleDateFormat("HH:mm:ss");
	 		fmPlayTime.setTimeZone(TimeZone.getTimeZone("GMT"));
	 		String szTime = fmPlayTime.format(new Integer(iPosition));
	 		if(curtime.getText().toString().compareToIgnoreCase(szTime) != 0)
	 			curtime.setText(szTime.toString());
	 		
	 		double dRation = 0.0;
	 		if(iDuration != 0)
	 			dRation = (double)iPosition / iDuration;
	 		else
	 			dRation = 0;
	 		prg.setProgress((int)(prg.getMax()*dRation));
		}
		catch(Exception ex)
		{
		}
		finally
		{
		}
	}
	
	public synchronized void doUpdateDurationTime()
	{
		try
		{
			int iDuration = m_viewParent.GetDuration();

			LableView alltime = (LableView)m_viewControllor.findViewById(R.id.alltime);
	 		SimpleDateFormat fmAllTime = new SimpleDateFormat("HH:mm:ss");
	 		fmAllTime.setTimeZone(TimeZone.getTimeZone("GMT"));
	 		String szAllTime = fmAllTime.format(new Integer(iDuration));
	 		if(alltime.getText().toString().compareToIgnoreCase(szAllTime) != 0)
	 			alltime.setText(szAllTime.toString());

		}	
		catch(Exception ex)
		{
		}
		finally
		{
		}
		
	}	
	
	/////////////////////////////////////////////////////////////////////
	// inner class
	private class PlayerCtrlTimerHandler extends Handler
	{
		public void handleMessage(Message msg)
		{
			switch(msg.what)
			{
			case m_stfMsg_Dismiss:
				doDismissControllor();
				break;
			case m_stfMsg_UpdatePlayTime:
				doUpdatePlayTime();
				doUpdateDurationTime();
				break;
			default:
				break;
			}
		}
	}
	private class PlayerCtrlTimer extends TimerTask
	{
		int m_iMsg = 0;
		public PlayerCtrlTimer(int iMsg)
		{
			m_iMsg = iMsg;
		}
		
		public void run() 
		{
			// TODO Auto-generated method stub
			Message msg = new Message();
			msg.what = m_iMsg;
			m_hTimerMsgHandler.sendMessage(msg);
		}
	}
	/////////////////////////////////////////////////////////////////////
}
