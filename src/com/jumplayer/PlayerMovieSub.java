package com.jumplayer;

import java.io.UnsupportedEncodingException;

import com.jumplayer.utils.Preferences;

import android.content.Context;
import android.media.AudioManager;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnKeyListener;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.PopupWindow;
import android.widget.TextView;

public class PlayerMovieSub implements OnTouchListener, OnKeyListener {
	
	////////////////////////////////////////////////////////////
	// control bar state
	public static final int 	m_stfState_Showing = 0;
	public static final int 	m_stfState_Dismiss = 1;
	
	private PlayerMovieView	m_viewParent;
	private Context					m_Context;
	private AudioManager 			m_AudioManager;
	
	private View					m_viewControllor;
	private PopupWindow 			m_wndPopup;
	
	private int						m_iState;
	
	public PlayerMovieSub(PlayerMovieView parent)
	{
		m_viewParent = parent;
		m_Context = parent.getMovieContext();
		m_AudioManager = (AudioManager) m_Context.getSystemService(Context.AUDIO_SERVICE);
		
		InitControllor();
		
		m_iState = m_stfState_Dismiss;
	}
	public void InitControllor()
	{
		LayoutInflater li = (LayoutInflater) (m_Context.getSystemService(Context.LAYOUT_INFLATER_SERVICE));
		m_viewControllor = (View) li.inflate(R.layout.subtitle, null, true);
				
		doInitListener();
		
		m_wndPopup = new PopupWindow(m_viewControllor, LayoutParams.FILL_PARENT, LayoutParams.FILL_PARENT, true);
	}
	public void UninitControllor()
	{
		
	}
	public int getState()
	{
		return m_iState;
	}
	private void doInitListener()
	{
		m_viewControllor.setFocusableInTouchMode(true);
		
		m_viewControllor.setOnTouchListener(this);
		m_viewControllor.setOnKeyListener(this);
	}
	public synchronized void doShowSubtile(String strSub)
	{
		TextView infoview = (TextView)m_viewControllor.findViewById(R.id.thd_subtitle);
		String newStr=null;
		try {
			newStr = new String(strSub.getBytes(), Preferences.getInstance(m_Context).getSubCharset());
		} catch (UnsupportedEncodingException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if(newStr!=null)
			infoview.setText(newStr.toString());	
	}
	public boolean onTouch(View v, MotionEvent event) 
	{
		// TODO Auto-generated method stub	
		boolean Touch = m_viewParent.onTouchEvent(event);
		return Touch;
	}	
	public void doShowControllor()
	{
		if(m_wndPopup != null)
		{
			m_wndPopup.showAtLocation(m_viewParent, Gravity.CENTER|Gravity.BOTTOM, 0, 0);
			m_iState = m_stfState_Showing;
		}	
	}
	
	public void doDismissControllor()
	{
		if(m_wndPopup != null)
		{			
			m_wndPopup.dismiss();
			m_iState = m_stfState_Dismiss;
		}
	}
	public boolean onKey(View v, int keyCode, KeyEvent event) 
	{
		// TODO Auto-generated method stub
		if (keyCode == KeyEvent.KEYCODE_BACK) 
		{
			this.m_viewParent.onKey(v, keyCode, event);
			
//				doDismissControllor();
			
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
}
