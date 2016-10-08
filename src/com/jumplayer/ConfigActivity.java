package com.jumplayer;

import android.app.Activity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ListView;

import com.jumplayer.utils.ActivityManager;
import com.jumplayer.utils.PlayerMenu;
import com.jumplayer.utils.Preferences;

public class ConfigActivity extends Activity //implements AdListener
{
	protected void onCreate(Bundle savedInstanceState) 
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.config);

	    ConfigAdapter adapter = new ConfigAdapter(this);
	    ListView lv = (ListView)findViewById(R.id.configlist);
	    lv.setAdapter(adapter);
	    lv.setOnItemClickListener(adapter);
	    
	    ActivityManager.getInstance().addActivity(this);
	}

	protected void onResume() 
	{
		// TODO Auto-generated method stub
		super.onResume();
		StartupActivity.setActivityIndex(StartupActivity.g_iActivity_Config);
	}
	
	protected void onStop() 
	{
		// TODO Auto-generated method stub
		super.onStop();
	
		Preferences.getInstance(this).doSavePrefs();
	}

	protected void onDestroy() 
	{
		// TODO Auto-generated method stub
		ActivityManager.getInstance().removeActivity(this);
		super.onDestroy();
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
