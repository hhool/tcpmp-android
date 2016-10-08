package com.jumplayer.utils;

import java.util.LinkedList;
import java.util.List;

import com.jumplayer.StartupActivity;

import android.app.Activity;

public class ActivityManager 
{
	private static ActivityManager m_Instance = null;
	public static ActivityManager getInstance()
	{
		if(m_Instance == null)
			m_Instance = new ActivityManager();
		
		return m_Instance;
	}
	
	private List<Activity> activityList = new LinkedList<Activity>(); 
	
	public void addActivity(Activity activity)
	{
		activityList.add(activity);
	}
	
	public void removeActivity(Activity activity)
	{
		activityList.remove(activity);
	}
	
	public void exit()
	{
		StartupActivity.setActivityIndex(StartupActivity.g_iActivity_None);
		for(Activity activity:activityList) 
			activity.finish();
	}
}
