package com.jumplayer;

import com.google.ads.Ad;
import com.google.ads.AdListener;
import com.google.ads.AdRequest;
import com.google.ads.AdRequest.ErrorCode;
import com.google.ads.AdSize;
import com.google.ads.AdView;
import com.jumplayer.interfaces.PlayerStateListener;
import com.jumplayer.object.PlayerCore;
import com.jumplayer.utils.Preferences;
import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Bundle;
import android.provider.MediaStore;
import android.telephony.PhoneStateListener;
import android.telephony.TelephonyManager;
import android.util.Log;
//import android.util.Log;
import android.view.Gravity;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.view.Window;
import android.view.WindowManager;
import android.widget.FrameLayout;

public class PlayerActivity  extends Activity implements AdListener,PlayerStateListener
{
	private PlayerMovieView m_Player = null;
	private static final String   TAG = "JumPlayer";
	protected void onCreate(Bundle savedInstanceState) 
	{
		requestWindowFeature(Window.FEATURE_NO_TITLE);
		
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
				
		requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		createPhoneListener();
		
		if (Preferences.getInstance(this).getScreenDirection() == ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE) 
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		else if (Preferences.getInstance(this).getScreenDirection() == ActivityInfo.SCREEN_ORIENTATION_PORTRAIT)
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
		else
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR);
		
		setContentView(R.layout.playview);
		
		processIntentData(getIntent());		
	}
	protected void onNewIntent(Intent intent) 
	{
		// TODO Auto-generated method stub
   		//Log.e(TAG, "dongwei: PlayerActivity onNewIntent");
		super.onNewIntent(intent);
		setIntent(intent);
		processIntentData(intent);
	}
	protected void processIntentData(Intent intent)
	{
		String path = null;
		if (intent.getAction() != null&& intent.getAction().equals(Intent.ACTION_VIEW )) 
		{
            /* Started from external application */
            path = getSchemePath(intent.getData());
            
            if (path == null || path.length() <= 1)
            {
            	//finish();
            	return ;
            }
			if(StartupActivity.g_PlayerEnv == null)
			{
				StartupActivity.g_PlayerEnv = StartupActivity.getEnvironment(getApplicationContext());
			}
		}
		else
		{
			Bundle bundle = intent.getExtras();
			if(bundle == null)
			{
//				this.finish();
				return ;
			}
			path = bundle.getString("file");
		}
		
		if(m_Player == null)
		{
			m_Player = (PlayerMovieView)findViewById(R.id.thd_player);
			m_Player.Open(new String(path),false);	
			m_Player.SetPlayerStateListen(this);
		}
		else
		{
			m_Player.Open(new String(path),true);
		}
	}
	protected String getSchemePath(Uri uri)
	{
		if(uri != null)
		{
			if("content".equals(uri.getScheme()))
			{
				ContentResolver contentResolver = getContentResolver();
				Cursor c = contentResolver.query(uri, null, null, null, null);
				if(c.moveToFirst())
				{
					return c.getString(c.getColumnIndex(MediaStore.Video.Media.DATA));
				}
			}
			else if("file".equals(uri.getScheme()))
			{
				return uri.getPath();
//				String path = uri.toString();
//				//先对字符串进行解编码，若本身不是编码方式原文输出
//				path = Uri.decode(path);				
//				path = this.encodeFileName(path, "/");
//				URI fileURI = URI.create(path);
//				File file = new File(fileURI);
//				
//				return file.getPath();
			}
			else if("http".equals(uri.getScheme()))
			{
				return uri.toString();
			}
		}
		
		return "";
	}
	
	/**
	 * 对文件夹或者文件特殊字符进行遍历编码
	 * @author song.lj
	 * @return String
	 */
	private String encodeFileName(String path, String regular){
	    String[] parts = path.split(regular);
	    StringBuffer sb = new StringBuffer();
	    sb.append(parts[0]);
	    for(int i=1; i<parts.length; i++){
	    	sb.append(regular);
	    	sb.append(Uri.encode(parts[i]));
	    }
	    
	    return sb.toString();
	}
	
	protected void onStart() 
	{
		// TODO Auto-generated method stub
		super.onStart();
	}

	protected void onResume() 
	{
		// TODO Auto-generated method stub
		super.onResume();
		StartupActivity.setActivityIndex(StartupActivity.g_iActivity_Movie);
	}

	protected void onPause() 
	{
		// TODO Auto-generated method stub
		super.onPause();
	}

	protected void onStop() 
	{
		// TODO Auto-generated method stub
		//Log.d(TAG, "PlayerActivity:onStop");
		super.onStop();
	}

	protected void onDestroy() 
	{
		// TODO Auto-generated method stub	
		super.onDestroy();
	}
	public void finish()
	{
		if(m_Player!=null)
		{
			m_Player.Close();
		}
		super.finish();
		
		StartupActivity.setActivityIndex(StartupActivity.g_iActivity_FileList);
	}
	
	/** 
     * 按钮-监听电话 
     * @param v 
     */  
    public void createPhoneListener() {  
        TelephonyManager telephony = (TelephonyManager)getSystemService(Context.TELEPHONY_SERVICE);  
        telephony.listen(new OnePhoneStateListener(),  PhoneStateListener.LISTEN_CALL_STATE);  
    }  
    
    /** 
     * 电话状态监听. 
     * @author stephen 
     * 
     */  
    class OnePhoneStateListener extends PhoneStateListener{  
        @Override  
        public void onCallStateChanged(int state, String incomingNumber) {  
            //Log.i(, "[Listener]电话号码:"+incomingNumber);  
        	if(m_Player !=null)
            {
        		m_Player.onCallStateChanged(state, incomingNumber);
            }
            super.onCallStateChanged(state, incomingNumber);  
        }  
    }

	@Override
	public void onDismissScreen(Ad arg0) {
		// TODO Auto-generated method stub
		Log.d(TAG, "onDismissScreen");
	}
	@Override
	public void onFailedToReceiveAd(Ad arg0, ErrorCode arg1) {
		// TODO Auto-generated method stub
		Log.d(TAG, "onFailedToReceiveAd");
	}
	@Override
	public void onLeaveApplication(Ad arg0) {
		// TODO Auto-generated method stub
		Log.d(TAG, "onLeaveApplication");
	}
	@Override
	public void onPresentScreen(Ad arg0) {
		// TODO Auto-generated method stub
		Log.d(TAG, "onPresentScreen");
	}
	@Override
	public void onReceiveAd(Ad arg0) {
		// TODO Auto-generated method stub
		Log.d(TAG, "onReceiveAd");
	}
	@Override
	public void PlayerState(int newState) {
		// TODO Auto-generated method stub
		if(m_Player == null)
			return ;
		if(!m_Player.HasStream(PlayerCore.STREAM_VIDEO))
		{

		}
		else if(newState == PlayerCore.PLAY_STATE_PAUSE)
		{
			
		}
		else
		{
			
		}
	}  
}
