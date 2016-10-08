package com.jumplayer;

import com.jumplayer.object.PlayerCore;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;

public class StartupActivity extends Activity
{
	public static PlayerEnvironment g_PlayerEnv = null;
	public static final int g_iActivity_None 		= 0;
	public static final int g_iActivity_FileList 	= 1;
	public static final int g_iActivity_Movie 		= 2;
	public static final int g_iActivity_Config	 	= 3;
	
	private static boolean 	g_bRunning 		= false;
	private static int 	   	g_iActiveIndex 	= g_iActivity_None;

	protected void onCreate(Bundle savedInstanceState) 
	{
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		g_PlayerEnv = getEnvironment(getApplicationContext());
		if(g_bRunning)
		{
			doChangeActivity(g_iActiveIndex);
			return;
		}
		
		doChangeActivity(g_iActivity_FileList);
		g_bRunning = true;
	}
	public static PlayerEnvironment getEnvironment(Context context)
	{
		PlayerEnvironment env = new PlayerEnvironment();
		env.m_iSdkVersion = Build.VERSION.SDK_INT;
		env.m_szModel		= Build.MODEL;
		env.m_szSysVersion	= Build.VERSION.RELEASE;

		env.m_szCountry 	= context.getResources().getConfiguration().locale.getLanguage();
		env.m_LangId		= GetCountryId(env.m_szCountry);
		
		return env;
	}
	public static int  GetCountryId(String strCountry)
	{
		int LangID = PlayerCore.LANG_EN;
		
		if(strCountry.equals("zh"))
		{
			LangID = PlayerCore.LANG_CHS;
		}
		
		return LangID;
	}
	public void doChangeActivity(int iActivityIndex)
	{
		Intent intent = null;
		switch(iActivityIndex)
		{
		case g_iActivity_None:
		case g_iActivity_FileList:
			{
				intent = new Intent(this, FileListActivity.class);
				startActivity(intent);
			}
			break;
		case g_iActivity_Movie:
			{
				intent = new Intent(this, PlayerActivity.class);
				startActivity(intent);				
			}
			break;
		case g_iActivity_Config:
			{
				intent = new Intent(this, ConfigActivity.class);
				startActivity(intent);				
			}
			break;			
		default:
			break;
		}
		this.finish();
	}
	
	public static void setActivityIndex(int iActivity)
	{
		g_iActiveIndex = iActivity;
	}
}
