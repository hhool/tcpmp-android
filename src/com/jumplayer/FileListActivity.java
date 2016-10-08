package com.jumplayer;

import android.app.Activity;
import android.content.res.Configuration;
import android.os.Bundle;
//import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.Window;
import android.widget.ListView;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.google.ads.*;
import com.google.ads.AdRequest.ErrorCode;
import com.jumplayer.object.PlayerCore;
import com.jumplayer.utils.ActivityManager;
import com.jumplayer.utils.MediaInfoScannerThread;
import com.jumplayer.utils.PlayerMenu;
import com.jumplayer.utils.Preferences;

public class FileListActivity extends Activity /*implements AdListener*/{
	private PlayerCore mPlayer;
	private ListView mFileList;
	private FileListAdapter mFileAdapter;
	//AdView mAdView;
	private String mRootPath;
	private boolean mAcquireThumbnail;
	private static final String   TAG = "JumPlayer";
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		requestWindowFeature(Window.FEATURE_INDETERMINATE_PROGRESS);
		setContentView(R.layout.filelist);
		
		RelativeLayout layout = (RelativeLayout) findViewById(R.id.playlist_title_menu_layout);
		// Please replace MY_BANNER_UNIT_ID with your AdMob Publisher ID
	    AdView adView = new AdView(this, AdSize.BANNER, "a15020b660d2d55");
	    // Add the adView to it
	    layout.addView(adView);
	    
	    AdRequest request = new AdRequest();
	    //request.setTesting(true);
	    adView.loadAd(request);    
	    	    
	    mPlayer = PlayerCore.getInstance();
	    TextView pathView = (TextView) findViewById(R.id.file_path_text);
	    mRootPath = Preferences.getInstance(this.getApplicationContext()).getDefaultPath();
		mFileAdapter = new FileListAdapter(this, mRootPath,pathView,mPlayer);
		mFileList = (ListView) findViewById(R.id.filelist);
		mFileList.setDividerHeight(2);
		
		mFileList.setAdapter(mFileAdapter);
		mFileList.setOnItemClickListener(mFileAdapter);
		mFileList.setOnScrollListener(mFileAdapter);
		
		mAcquireThumbnail = Preferences.getInstance(this.getApplicationContext()).getAcquireThumbnail();

		ActivityManager.getInstance().addActivity(this);
	}

	
	public void onDismissScreen(Ad arg0) {
		// TODO Auto-generated method stub
		
	}

	public void onFailedToReceiveAd(Ad arg0, ErrorCode arg1) {
		// TODO Auto-generated method stub
		//Log.d(TAG, "failed to receive ad (" + arg1 + ")");

	}

	public void onLeaveApplication(Ad arg0) {
		// TODO Auto-generated method stub
		
	}

	public void onPresentScreen(Ad arg0) {
		// TODO Auto-generated method stub
		
	}

	public void onReceiveAd(Ad arg0) {
		// TODO Auto-generated method stub
		//Log.d(TAG, "onReceiveAd ");

	}

	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if(mFileAdapter.onKey(null, keyCode, event))
			return true;
		else
			return super.onKeyDown(keyCode, event);
	}
	
	public void onConfigurationChanged(Configuration newConfig) {
	  super.onConfigurationChanged(newConfig);
	}
	
	protected void onStop() {
		// TODO Auto-generated method stub
		super.onStop();
	}

	public void onDestroy()
	{
		ActivityManager.getInstance().removeActivity(this);

	    super.onDestroy();
		mPlayer.Destory();
	}
	
	protected void onResume() 
	{
		super.onResume();
		StartupActivity.setActivityIndex(StartupActivity.g_iActivity_FileList);
		if(mRootPath.compareToIgnoreCase(Preferences.getInstance(this.getApplicationContext()).getDefaultPath())!=0)
		{
			mRootPath = Preferences.getInstance(this.getApplicationContext()).getDefaultPath();
			mFileAdapter.setDirectory(mRootPath);
		}
		
		if(mAcquireThumbnail != Preferences.getInstance(this.getApplicationContext()).getAcquireThumbnail())
		{
			mAcquireThumbnail = Preferences.getInstance(this.getApplicationContext()).getAcquireThumbnail();
			mFileAdapter.UpdateAcquireSetting();			
		}
		
		MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance();
		if(tnThread != null)
		{
			tnThread.resumeScannerThread();
		}
		//Log.d(TAG, "FileListActivity:onResume");
	}

	public boolean onCreateOptionsMenu(Menu menu) 
	{
		// TODO Auto-generated method stub
		return PlayerMenu.getInstance(this).onCreateOptionsMenu(this, menu);
	}

	public boolean onOptionsItemSelected(MenuItem item)
	{
		// TODO Auto-generated method stub		
		return PlayerMenu.getInstance(this).onOptionsItemSelected(this, item);
	}	
}

