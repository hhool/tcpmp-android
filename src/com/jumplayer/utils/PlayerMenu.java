package com.jumplayer.utils;

import com.jumplayer.R;
import com.jumplayer.ConfigActivity;
import com.jumplayer.FileListActivity;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.view.Menu;
import android.view.MenuItem;

public class PlayerMenu 
{
	////////////////////////////////////////////////////////////////
	// Menu item
	public static final int g_sfi_Menu_ShowFileList			=100;
	public static final int g_sfi_Menu_ShowConfig			=101;
	public static final int g_sfi_Menu_Exit					=102;
	////////////////////////////////////////////////////////////////	

	private static PlayerMenu m_stInstance = null;
	private	Context m_Context;
	
	public static PlayerMenu getInstance(Context context)
	{
		if(m_stInstance == null)
			m_stInstance = new PlayerMenu(context);
		
		return m_stInstance;
	}
	
	protected PlayerMenu(Context context)
	{
		m_Context = context.getApplicationContext();
	}
	
	public boolean onCreateOptionsMenu(Activity from, Menu menu)
	{
		if(from.getClass() == FileListActivity.class)
		{
			menu.add(Menu.NONE, g_sfi_Menu_ShowConfig, 1, m_Context.getResources().getString(R.string.menu_item_config)).setIcon(android.R.drawable.ic_menu_manage);			
		}
		else if(from.getClass() == ConfigActivity.class)
		{
			menu.add(Menu.NONE, g_sfi_Menu_ShowFileList, 1, m_Context.getResources().getString(R.string.menu_item_filelist)).setIcon(android.R.drawable.ic_menu_mapmode);			
		}
		menu.add(Menu.NONE, g_sfi_Menu_Exit, 1, m_Context.getResources().getString(R.string.menu_item_exit)).setIcon(android.R.drawable.ic_lock_power_off);
		
		return true;
	}
	
	public boolean onOptionsItemSelected(Activity from, MenuItem item)
	{
		switch(item.getItemId())
		{
		case g_sfi_Menu_ShowFileList:
			from.startActivity(new Intent(from, FileListActivity.class));
			break;
		case g_sfi_Menu_ShowConfig:
			from.startActivity(new Intent(from, ConfigActivity.class));
			break;
		case g_sfi_Menu_Exit:
			{
				//from.finish();
				ActivityManager.getInstance().exit();
			}
			break;
		default:
			break;
		}
		
		return false;
	}
}
