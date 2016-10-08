package com.jumplayer.utils;

import java.nio.charset.Charset;
import java.util.Iterator;
import java.util.Map;

import com.jumplayer.FileListAdapter;
import com.jumplayer.object.PlayerCore;

import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
//import android.util.Log;

public class Preferences 
{
	public static final String Preference_Name = "Prefs";

	///////////////////////////////////////////////////////
	// config option
	public static final String g_szPlayConfigTitle 	= "playconfig_title";
	public static final String g_szFitVideoMode		= "fitviedomode";
	public static final String g_szPlayMode 		= "playmode";
	public static final String g_szScreenDirection	= "screendirection";
	public static final String g_szSubCharset		= "subCharset";
	public static final String g_szDefaultPath 		= "defaultpath";
	public static final String g_szAcquireThumbnail	= "AcquireThumbnail";
	public static final String g_szDefaultSettings	= "defaultsettings";
	
	public static final String g_szAboutTitle		= "about_title";
	public static final	String g_szAbout		= "aboutjump";	
	public static final String g_szThumbnailCollationTime = "thumbnailcollationtime"; 
	
	// Play mode type
	public static final int g_iPlayMode_Loop = 0;		// 顺序播放目录中的文件
	public static final int g_iPlayMode_Single = 1;		// 循环播放单个文件
	public static final int g_iPlayMode_OnlyOne = 2;	// 只播放单个文件
	
	//////////////////////////////////////////////////
	// Prefs options
	private int		m_iFitVideoMode;
	private int		m_iDefFitVideoMode;

	private int		m_iPlayMode;		// 0: 顺序播放	1：单个播放
	private int 	m_iDefPlayMode;
		
	private int 	m_iScreenDirection;
	private int 	m_iDefScreenDirection;
	
	private String  m_szSubCharset;
	private String  m_szDefSubCharset;
	private String[]m_szAllCharset;
	
	private String	m_szDefaultPath;
	private String 	m_szDefDefaultPath;
	
	private boolean m_bAcquireThumbnail;
	private boolean m_bDefAcquireThumbnail;
	
	private long	m_lCollationTime;
	
	/////////////////////////////////////////////////
	
	private static Preferences m_stInstance;
	private Context m_Context;
	private SharedPreferences m_Prefs;
	
	public static Preferences getInstance(Context context)
	{
		if(m_stInstance == null)
			m_stInstance = new Preferences(context);
		
		return m_stInstance;
	}
	
	private Preferences(Context context)
	{
		m_Context = context.getApplicationContext();

		m_iDefFitVideoMode 		= PlayerCore.VIDEO_MODE_FIT;
		m_iDefPlayMode 			= g_iPlayMode_OnlyOne;
		m_iDefScreenDirection	= ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
		m_szDefDefaultPath		= FileListAdapter.getExternalStoragePath();
		m_szDefSubCharset		= "UTF-8";
		m_bDefAcquireThumbnail	= true;
		
		m_lCollationTime		= 0;
		
		doLoadPrefs();
	}	
	
	// Fit mode
	public int getFitVideoMode()
	{
		return m_iFitVideoMode;
	}
	
	public void setFitVideoMode(int iMode)
	{
		m_iFitVideoMode = iMode; 
	}

	// play mode type
	public int getPlayModeType()
	{
		return m_iPlayMode;
	}
	public void setPlayModeType(int iType)
	{
		m_iPlayMode = iType;
	}

	// Screen Direction
	public int getScreenDirection()
	{
		if(m_iScreenDirection>2)
			return 2;
		return m_iScreenDirection;
	}
	public void setScreenDirection(int iType)
	{
		if(iType == 0)
			m_iScreenDirection = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
		else if(iType == 1)
			m_iScreenDirection = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
		else 
			m_iScreenDirection = ActivityInfo.SCREEN_ORIENTATION_SENSOR;
	}

	public String getSubCharset()
	{
		if(m_szSubCharset == null)
		{
			m_szSubCharset = Charset.defaultCharset().name();
		}
		return m_szSubCharset;
	}
	public int GetSubCharsetIndex(String szCharset)
	{
		int nIndex = 0;
		Map<String, Charset> charSets = Charset.availableCharsets();  
	    Iterator<String> it = charSets.keySet().iterator();
	    while (it.hasNext()) { 
	    	if(((String) it.next()).compareTo(szCharset) == 0)
	    		break;
	    	nIndex++;
	    }
	    return nIndex;
	}
	public void setSubCharset(String Charset)
	{
		m_szSubCharset = Charset;
	}
	public int getAvailableCharsetCount()
	{
		int nCount = 0;
		Map<String, Charset> charSets = Charset.availableCharsets();  
	    Iterator<String> it = charSets.keySet().iterator();
	    while (it.hasNext()) { 
	    	nCount++;
	    	it.next();
	    }
	    return nCount;
	}
	public String[] getAllCharset()
	{
		if(m_szAllCharset==null){
			int nCount = getAvailableCharsetCount();
			m_szAllCharset = new String[nCount];
			int i = 0;
			 Map<String, Charset> charSets = Charset.availableCharsets();  
		     Iterator<String> it = charSets.keySet().iterator();
		     while (it.hasNext()&&i<nCount) { 
		    	 m_szAllCharset[i] = (String) it.next();  
	             
		         //Log.i("JumPlayer", m_szAllCharset[i]);
		         i++;
		     }
		}
		return m_szAllCharset;
	}
	public void setDefaultPath(String szDefaultPath) 
	{
		m_szDefaultPath = szDefaultPath;
	}

	public String getDefaultPath() 
	{
		return m_szDefaultPath;
	}	
	
	public String getDefDefaultPath() 
	{
		return m_szDefDefaultPath;
	}	
	
	public void setAcquireThumbnail(boolean bAcquireThumbnail) 
	{
		m_bAcquireThumbnail = bAcquireThumbnail;
	}

	public boolean getAcquireThumbnail() 
	{
		return m_bAcquireThumbnail;
	}	

	public boolean getDefAcquireThumbnail()
	{
		return m_bDefAcquireThumbnail;
	}
	
	public long getThumbnailCollationTime()
	{
		return m_lCollationTime;
	}
	
	// Load Prefs
	public void doLoadPrefs()
	{
		m_Prefs = m_Context.getSharedPreferences(Preference_Name, 0);
		
//		m_bShowExtraName = m_Prefs.getBoolean(ASConfigView.eszShowExtraName, m_bDefShowExtraName);
		
		m_iFitVideoMode 	= m_Prefs.getInt(Preferences.g_szFitVideoMode, m_iDefFitVideoMode);
		m_iPlayMode 		= m_Prefs.getInt(Preferences.g_szPlayMode, m_iDefPlayMode);
		m_iScreenDirection	= m_Prefs.getInt(Preferences.g_szScreenDirection, m_iDefScreenDirection);
		m_szSubCharset		= m_Prefs.getString(Preferences.g_szSubCharset,m_szDefSubCharset);
		m_szDefaultPath		= m_Prefs.getString(Preferences.g_szDefaultPath, m_szDefDefaultPath);
		m_bAcquireThumbnail	= m_Prefs.getBoolean(Preferences.g_szAcquireThumbnail, m_bDefAcquireThumbnail);
		
		m_lCollationTime	= m_Prefs.getLong(Preferences.g_szThumbnailCollationTime, 0);
	}
	
	// Save Prefs
	public void doSavePrefs()
	{
		if(m_Prefs == null)
			return;
		
		SharedPreferences.Editor prefEditor = m_Prefs.edit();
		
//		prefEditor.putBoolean(ASConfigView.eszShowExtraName, m_bShowExtraName);
		prefEditor.putInt(g_szFitVideoMode, m_iFitVideoMode);
		prefEditor.putInt(g_szPlayMode, m_iPlayMode);
		prefEditor.putInt(g_szScreenDirection, m_iScreenDirection);
		prefEditor.putString(g_szDefaultPath, m_szDefaultPath);
		prefEditor.putString(g_szSubCharset,m_szSubCharset);
		prefEditor.putBoolean(g_szAcquireThumbnail, m_bAcquireThumbnail);		
		
		prefEditor.commit();
	}
	
	public void doSaveCollationTime(long lTime)
	{
		if(m_Prefs == null)
			return;
		
		SharedPreferences.Editor prefEditor = m_Prefs.edit();

		m_lCollationTime = lTime;
		prefEditor.putLong(g_szThumbnailCollationTime, m_lCollationTime);

		prefEditor.commit();
	}
	
	// Use default settings
	public void doUseDefaultSettings()
	{
		m_iFitVideoMode			= m_iDefFitVideoMode;
		m_iPlayMode				= m_iDefPlayMode;
		m_iScreenDirection		= m_iDefScreenDirection;
		m_szDefaultPath			= m_szDefDefaultPath;
		m_szSubCharset			= m_szDefSubCharset;
		m_bAcquireThumbnail		= m_bDefAcquireThumbnail;
	}
}
