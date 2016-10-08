package com.jumplayer;

import java.io.File;
import java.util.Vector;

import com.jumplayer.utils.Preferences;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.AdapterView.OnItemClickListener;

public class ConfigAdapter extends BaseAdapter implements OnItemClickListener
{
	public class ConfigListItem
	{
		private String 		m_szKey;
		private View 		m_View;
		
		public ConfigListItem(String szKey, View view)
		{
			m_szKey = szKey;
			m_View 	= view;
		}
		
		public void setKey(String szKey) 
		{
			this.m_szKey = szKey;
		}
		public String getKey() 
		{
			return m_szKey;
		}
		
		public void setView(View view) 
		{
			this.m_View = view;
		}
		public View getView() 
		{
			return m_View;
		}
	};
	
	private Context m_Context = null;
	private Vector<ConfigListItem> 		m_vItems;

	public ConfigAdapter(Context context)
	{
		m_Context = context;
		
		m_vItems = new Vector<ConfigListItem>();
		m_vItems.add(new ConfigListItem(Preferences.g_szPlayConfigTitle, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szFitVideoMode, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szPlayMode, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szScreenDirection, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szSubCharset, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szDefaultPath, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szAcquireThumbnail, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szDefaultSettings, null));
		
		m_vItems.add(new ConfigListItem(Preferences.g_szAboutTitle, null));
		m_vItems.add(new ConfigListItem(Preferences.g_szAbout, null));
	}
	
	public int getCount() 
	{
		// TODO Auto-generated method stub
		return m_vItems.size();
	}

	public Object getItem(int arg0) 
	{
		// TODO Auto-generated method stub
		return null;
	}

	public long getItemId(int arg0) 
	{
		// TODO Auto-generated method stub
		return 0;
	}

	public View getView(int position, View convertView, ViewGroup parent) 
	{
		// TODO Auto-generated method stub
		ConfigListItem item = m_vItems.get(position);
		
		if(position >=0 && position < m_vItems.size() && item == null)
			return null;
		
		if(item.getKey().compareTo(Preferences.g_szPlayConfigTitle) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_title, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_title_text, R.string.configlist_item_title);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}
		}
		else if(item.getKey().compareTo(Preferences.g_szFitVideoMode) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_fitvideomode);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}
			String[] szArray = m_Context.getResources().getStringArray(R.array.select_config_fitvideomode);
			setItemText(convertView, R.id.listitem_text_extentext, szArray[Preferences.getInstance(m_Context).getFitVideoMode()]);
		}
		else if(item.getKey().compareTo(Preferences.g_szPlayMode) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_playmode);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}
			String[] szArray = m_Context.getResources().getStringArray(R.array.select_config_playmode);
			setItemText(convertView, R.id.listitem_text_extentext, szArray[Preferences.getInstance(m_Context).getPlayModeType()]);			
		}
		else if(item.getKey().compareTo(Preferences.g_szScreenDirection) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_screendirection);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}
			String[] szArray = m_Context.getResources().getStringArray(R.array.select_config_screendirection);
			setItemText(convertView, R.id.listitem_text_extentext, szArray[Preferences.getInstance(m_Context).getScreenDirection()]);						
		}
		else if(item.getKey().compareTo(Preferences.g_szSubCharset) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_subcharset);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}
			setItemText(convertView, R.id.listitem_text_extentext, Preferences.getInstance(m_Context).getSubCharset());						
		}
		else if(item.getKey().compareTo(Preferences.g_szDefaultPath) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_defaultpath);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}						
			setItemText(convertView, R.id.listitem_text_extentext, Preferences.getInstance(m_Context).getDefaultPath());						
		}
		else if(item.getKey().compareTo(Preferences.g_szAcquireThumbnail) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_acquirethumbnail);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}
			
			boolean bAcquireThumbnail = Preferences.getInstance(m_Context).getAcquireThumbnail();
			if(bAcquireThumbnail)
				setItemText(item.getView(), R.id.listitem_text_extentext, m_Context.getResources().getStringArray(R.array.select_config_acquirethumbnail)[0]);				
			else
				setItemText(item.getView(), R.id.listitem_text_extentext, m_Context.getResources().getStringArray(R.array.select_config_acquirethumbnail)[1]);				
		}
		else if(item.getKey().compareTo(Preferences.g_szDefaultSettings) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_defaultsettings);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}						
		}	
		else if(item.getKey().compareTo(Preferences.g_szAboutTitle) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_title, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_title_text, R.string.configlist_item_about);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}						
		}	
		else if(item.getKey().compareTo(Preferences.g_szAbout) == 0)
		{
			if(item.getView() == null)
			{
				LayoutInflater inflater = LayoutInflater.from(m_Context);
				View itemview = inflater.inflate(R.layout.listitem_config_text, parent, false);
				item.setView(itemview);
				
				setItemText(itemview, R.id.listitem_text_text, R.string.configlist_item_aboutjump);
				
				convertView = itemview;
			}
			else
			{
				convertView = item.getView();
			}			
			
		}			
		return convertView;
	}
	
	public void setItemText(View view, int id, int iTextResID)
	{
		TextView tv = (TextView)view.findViewById(id);
		tv.setText(iTextResID);
	}
	
	public void setItemText(View view, int id, String szText)
	{
		TextView tv = (TextView)view.findViewById(id);
		tv.setText(szText);
	}

	public void onItemClick(AdapterView<?> parent, View view, int position, long id) 
	{
		// TODO Auto-generated method stub
		if(position < 0 || position >= m_vItems.size())
			return;
		final ConfigListItem item = m_vItems.get(position);
		if(item == null)
			return ;

		if(item.getKey().compareTo(Preferences.g_szFitVideoMode) == 0)
		{
			int iFit = Preferences.getInstance(m_Context).getFitVideoMode();

			new AlertDialog.Builder(m_Context)
				.setTitle(R.string.dlg_fitvideomode_title)
				.setSingleChoiceItems(R.array.select_config_fitvideomode, iFit,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								dialog.dismiss();
								Preferences.getInstance(m_Context).setFitVideoMode(whichButton);
								
								String[] szArray = m_Context.getResources().getStringArray(R.array.select_config_fitvideomode);
								setItemText(item.getView(), R.id.listitem_text_extentext, szArray[whichButton]);
							}
						})
				.setNegativeButton(R.string.dialog_cancel, 
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
		
							}
					})
				.create()
				.show();			
		}
		else if(item.getKey().compareTo(Preferences.g_szPlayMode) == 0)
		{
			int iPM = Preferences.getInstance(m_Context).getPlayModeType();

			new AlertDialog.Builder(m_Context)
				.setTitle(R.string.configlist_item_playmode)
				.setSingleChoiceItems(R.array.select_config_playmode, iPM,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								dialog.dismiss();
								Preferences.getInstance(m_Context).setPlayModeType(whichButton);

								String[] szArray = m_Context.getResources().getStringArray(R.array.select_config_playmode);
								setItemText(item.getView(), R.id.listitem_text_extentext, szArray[whichButton]);
							}
						})
				.setNegativeButton(R.string.dialog_cancel, 
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
		
							}
					})
				.create()
				.show();						
		}
		else if(item.getKey().compareTo(Preferences.g_szScreenDirection) == 0)
		{
			int iSD = Preferences.getInstance(m_Context).getScreenDirection();

			new AlertDialog.Builder(m_Context)
				.setTitle(R.string.configlist_item_screendirection)
				.setSingleChoiceItems(R.array.select_config_screendirection, iSD,
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								dialog.dismiss();
								Preferences.getInstance(m_Context).setScreenDirection(whichButton);

								String[] szArray = m_Context.getResources().getStringArray(R.array.select_config_screendirection);
								setItemText(item.getView(), R.id.listitem_text_extentext, szArray[whichButton]);
							}
						})
				.setNegativeButton(R.string.dialog_cancel, 
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
		
							}
					})
				.create()
				.show();			
		}
		else if(item.getKey().compareTo(Preferences.g_szSubCharset) == 0)
		{
			new AlertDialog.Builder(m_Context)
				.setTitle(R.string.configlist_item_subcharset)
				.setSingleChoiceItems(Preferences.getInstance(m_Context).getAllCharset(), 
						Preferences.getInstance(m_Context).GetSubCharsetIndex(Preferences.getInstance(m_Context).getSubCharset()),
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
								dialog.dismiss();
								Preferences.getInstance(m_Context).setSubCharset(Preferences.getInstance(m_Context).getAllCharset()[whichButton]);
								setItemText(item.getView(), R.id.listitem_text_extentext, Preferences.getInstance(m_Context).getAllCharset()[whichButton]);
							}
						})
				.setNegativeButton(R.string.dialog_cancel, 
						new DialogInterface.OnClickListener() {
							public void onClick(DialogInterface dialog, int whichButton) {
		
							}
					})
				.create()
				.show();			
		}
		else if(item.getKey().compareTo(Preferences.g_szDefaultPath) == 0)
		{
			LayoutInflater inflater = LayoutInflater.from(m_Context);
			final View textEntryView = inflater.inflate(R.layout.dlg_config_edit, null);
			String path = Preferences.getInstance(m_Context).getDefaultPath();
			final EditText newDefaultDir = (EditText) textEntryView.findViewById(R.id.dlg_config_edit_editentry);
			newDefaultDir.setText(path);
			
			AlertDialog dialog = new AlertDialog.Builder(m_Context)
			.setTitle(R.string.dlg_defaultpath_title)
			.setView(textEntryView)
			.setPositiveButton(R.string.dialog_ok, 
					new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int whichButton) {
							/* User clicked OK so do some stuff */
							String szPath = newDefaultDir.getText().toString();
							File file = new File(szPath);
							if(!file.exists()|| !file.isDirectory()){
								Toast.makeText(m_Context, R.string.dlg_defaultpath_error, 1).show();
								Preferences.getInstance(m_Context).setDefaultPath(FileListAdapter.getExternalStoragePath());
							}else{
								Preferences.getInstance(m_Context).setDefaultPath(file.getAbsolutePath());
							}
							
							setItemText(item.getView(), R.id.listitem_text_extentext, Preferences.getInstance(m_Context).getDefaultPath());

						}
					})
			.setNegativeButton(R.string.dialog_cancel, 
					new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int whichButton) {
						}
					})
			.create();
			dialog.show();			
		}
		else if(item.getKey().compareTo(Preferences.g_szAcquireThumbnail) == 0)
		{
			boolean bAcquireThumbnail = Preferences.getInstance(m_Context).getAcquireThumbnail();
			if(bAcquireThumbnail)
			{
				Preferences.getInstance(m_Context).setAcquireThumbnail(false);
				setItemText(item.getView(), R.id.listitem_text_extentext, m_Context.getResources().getStringArray(R.array.select_config_acquirethumbnail)[1]);				
			}
			else
			{
				Preferences.getInstance(m_Context).setAcquireThumbnail(true);
				setItemText(item.getView(), R.id.listitem_text_extentext, m_Context.getResources().getStringArray(R.array.select_config_acquirethumbnail)[0]);				
			}
		}
		else if(item.getKey().compareTo(Preferences.g_szDefaultSettings) == 0)
		{
			Preferences.getInstance(m_Context).doUseDefaultSettings();
			notifyDataSetInvalidated();
		}
		else if(item.getKey().compareTo(Preferences.g_szAbout) == 0)
		{
			new AlertDialog.Builder(m_Context)
			.setTitle(R.string.dlg_about_title)
			.setMessage(R.string.dlg_about_content)
			.setPositiveButton(R.string.dialog_ok, 
					new DialogInterface.OnClickListener() {
						public void onClick(DialogInterface dialog, int id) {
						}
					})
			.create()
			.show();
		}
	}
}
