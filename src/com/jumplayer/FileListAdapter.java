package com.jumplayer;

import java.io.File;
import java.util.Arrays;
import java.util.TimeZone;
import java.util.Vector;

import com.jumplayer.R;
import com.jumplayer.database.SqliteOpenHelper;
import com.jumplayer.interfaces.ThumbNailCallBack;
import com.jumplayer.object.FileListItem;
import com.jumplayer.object.MediaThumbnail;
import com.jumplayer.object.PlayerCore;
import com.jumplayer.utils.FileFilter;
import com.jumplayer.utils.MediaInfoScannerHandler;
import com.jumplayer.utils.MediaInfoScannerThread;
import com.jumplayer.utils.Preferences;
import com.jumplayer.utils.TimeUtil;


import android.content.Context;
import android.content.Intent;
import android.os.Environment;
import android.text.format.DateFormat;
//import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnKeyListener;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.AbsListView.OnScrollListener;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.Toast;

public class FileListAdapter extends BaseAdapter implements
			 OnItemClickListener, OnKeyListener
			 ,ListView.OnScrollListener, ThumbNailCallBack{
	private Context mContext;
	private File mFile;
	private Vector<FileListItem> mFileList;
	private String mCurrentPath;
	private PlayerCore mPlayer;
	private TextView PathView;
	private static final String   TAG = "FileListAdapter";
	private MediaInfoScannerHandler mMediaInfoScanner = null;
    private boolean mBusy = false;		// For Busy View

    
	public static String getExternalStoragePath() {
		boolean exists = Environment.getExternalStorageState().equals(
				android.os.Environment.MEDIA_MOUNTED);
		if (exists)
			return Environment.getExternalStorageDirectory().getAbsolutePath();
		else
			return "/";
	}

	public FileListAdapter(Context context, String path,TextView pathview,PlayerCore Player) 
	{
		mContext = context;
		mPlayer = Player;
		PathView = pathview;
		
		mMediaInfoScanner = new MediaInfoScannerHandler(this); 
		MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
		tnThread.start();
				
		setDirectory(path);
	}

	public void setDirectory(String path) 
	{
		if(path == null || path.length() == 0)
			return;
		
		mFile = new File(path);
		if (mFile == null)
			return;
		
		MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
		tnThread.removeAllMessage();

		
		File[] list = mFile.listFiles(new FileFilter(mPlayer));
		if (list != null)
		{
			Arrays.sort(list, FileFilter.FileNameComparator);
		}
		if ((list == null || list.length == 0)&& (!path.equals(getExternalStoragePath()))) 
		{
			mCurrentPath = mFile.getAbsolutePath();
			PathView.setText(path);
			if(mFileList != null)
				mFileList.clear();
			notifyDataSetChanged();
			return;
		}
		mFileList = new Vector<FileListItem>();
		for (File f : list) 
		{
			if (f.isDirectory()) 
			{
				addItem(f);
			}
			else if (f.isFile() && !f.isHidden()) 
			{
				addItem(f);
			}
		}
		mCurrentPath = mFile.getAbsolutePath();
		PathView.setText(path);
		if (mFileList.size() > 0)
		{
			notifyDataSetChanged();
		}
	}

	private void addItem(File f)
	{
		FileListItem item = new FileListItem();
		item.setFile(f);
		mFileList.add(item);
	}
	
	public int getCount() 
	{
		if(mFileList == null)
			return 0;
		
		int count = mFileList.size();
		return count;
	}

	public Object getItem(int position) {
		Object obj = mFileList.get(position);
		return obj;
	}

	public long getItemId(int position) {
		return position;
	}

	public View getView(int position, View convertView, ViewGroup parent) 
	{
		if(position <0 || position >= mFileList.size())
			return null;
		
		FileListItem item = mFileList.get(position);
		if(item.getFile() == null)
			return null;
		
		if(item.getView() == null)
		{
			LayoutInflater inflater = LayoutInflater.from(mContext);
			View view = inflater.inflate(R.layout.fileitem, parent, false);
			((TextView) view.findViewById(R.id.filename)).setText(item.getFile().getName());
			
			if(item.getFile().isDirectory())
			{
				((ImageView) view.findViewById(R.id.fileicon)).setImageResource(R.drawable.folder_48);				
			}
			else if(item.getFile().isFile())
			{
				((ImageView) view.findViewById(R.id.fileicon)).setImageResource(R.drawable.file_90);				
			}
			else
			{
				((ImageView) view.findViewById(R.id.fileicon)).setImageResource(R.drawable.unkown_file_48);								
			}				
			
			item.setView(view);
			
            // Non-null tag means the view still needs to load it's data
			view.setTag(this);
		}
		
		if(!mBusy 
				&& item.getFile().isFile()
				&& item.getView().getTag() != null
				&& item.getFile().getParent().compareToIgnoreCase(SqliteOpenHelper.g_szThumbnailDir) != 0)
		{
			MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
			tnThread.sendMessage(MediaInfoScannerThread.g_sfi_MSG_GET_THUMBNAIL
					, new String(item.getFile().getAbsolutePath())
					, Preferences.getInstance(mContext).getAcquireThumbnail());			

			// Null tag means the view has the media info data
			item.getView().setTag(null);
		}
		
		return item.getView();
	}

	public void onItemClick(AdapterView<?> parent, View view, int position, long id) 
	{
		File file = mFileList.get(position).getFile();
		if (file.isDirectory())
			setDirectory(file.getAbsolutePath());
		if (file.isFile()) 
		{
			MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
			if(tnThread != null)
			{
				tnThread.suspandScannerThread();
				tnThread.waitForScannerThread();
			}
			Intent intent = new Intent(mContext, PlayerActivity.class);
			intent.putExtra("file", file.getAbsolutePath());
			mContext.startActivity(intent);
		}
	}
	
	public boolean onKey(View v, int keyCode, KeyEvent event) {
		if (keyCode == KeyEvent.KEYCODE_BACK) {
			if (mCurrentPath.compareTo(Preferences.getInstance(mContext).getDefaultPath()) != 0
					&& mCurrentPath.compareTo("/") != 0) 
			{
				setDirectory(mFile.getParent());
				return true;
			} 
			else
			{
				return false;
			}
		}
		return false;
	}

	public void onScroll(AbsListView view, int firstVisibleItem,
			int visibleItemCount, int totalItemCount) {
		// TODO Auto-generated method stub
		
	}

	public void onScrollStateChanged(AbsListView view, int scrollState) {
		// TODO Auto-generated method stub
		switch (scrollState) 
		{
		case OnScrollListener.SCROLL_STATE_IDLE:
			{
				mBusy = false;
				//Log.d(TAG, "SCROLL_STATE_IDLE mBusy="+mBusy);
	            int first = view.getFirstVisiblePosition();
	            int last = view.getLastVisiblePosition();
	            for (int i=first; i<=last; i++) 
	            {
	                FileListItem item = (FileListItem)mFileList.get(i);
	                if (item.getFile().isFile() 
	                		&& item.getView() != null 
	                		&& item.getView().getTag() != null
	                		&& item.getFile().getParent().compareToIgnoreCase(SqliteOpenHelper.g_szThumbnailDir) != 0) 
	                {
	        			MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
	        			tnThread.sendMessage(MediaInfoScannerThread.g_sfi_MSG_GET_THUMBNAIL
	        					, new String(item.getFile().getAbsolutePath())
	        					, Preferences.getInstance(mContext).getAcquireThumbnail());

	        			// Null tag means the view has the media info data
						item.getView().setTag(null);
	                }
	            }				
			}
			break;
		case OnScrollListener.SCROLL_STATE_TOUCH_SCROLL:
			{
				mBusy = true;
				//Log.d(TAG, "SCROLL_STATE_TOUCH_SCROLL mBusy="+mBusy);
			}
			break;
		case OnScrollListener.SCROLL_STATE_FLING:
			{
				mBusy = false;
				//Log.d(TAG, "SCROLL_STATE_FLING mBusy="+mBusy);
			}
			break;
		}
	}

	public void setThumbNail(String szPath, MediaThumbnail tn) 
	{
		// TODO Auto-generated method stub
		for(int i = 0; i < mFileList.size(); i++)
		{
			FileListItem item = mFileList.get(i);
			if(item.getFile().getAbsolutePath().compareTo(szPath) == 0
					&& item.getView() != null)		// only object compare is done
			{
				//Log.d(TAG, "FileAdapter:setThumbNail"+szPath);
				((ImageView)item.getView().findViewById(R.id.fileicon)).setImageBitmap(tn.getBitmap());
			
				long lDuration = (long)tn.getDuration();
				String szDuration = TimeUtil.formatLongToTimeStr(lDuration);
				TextView tv = (TextView)item.getView().findViewById(R.id.duration);
				tv.setVisibility(View.VISIBLE);
				tv.setText(szDuration);
			}
		}
		//notifyDataSetInvalidated();
	}
	
	public void onThumbnailThreadPrepared() 
	{
		// TODO Auto-generated method stub
		Preferences pref = Preferences.getInstance(mContext.getApplicationContext());
		long cur=System.currentTimeMillis();
		if(cur - pref.getThumbnailCollationTime() >= 15*24*60*60*1000)
		{
			MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
			tnThread.sendMessage(MediaInfoScannerThread.g_sfi_MSG_THUMBNAIL_COLLATION, null, false);			
		
			pref.doSaveCollationTime(cur);
		}
	}
	
	public void onThumbnailThreadException() 
	{
		// TODO Auto-generated method stub
		Toast.makeText(mContext, R.string.thumbnail_exception, Toast.LENGTH_LONG).show();
		
		Preferences pref = Preferences.getInstance(mContext.getApplicationContext());
		pref.setAcquireThumbnail(false);
		
		MediaInfoScannerThread tnThread = MediaInfoScannerThread.getInstance(mContext.getApplicationContext(), mPlayer, mMediaInfoScanner);
		if(tnThread != null)
			tnThread.removeAllMessage();
	}
	public void UpdateAcquireSetting()
	{
		for(int i = 0; i < mFileList.size(); i++)
		{
			FileListItem item = mFileList.get(i);
			if(item.getView() != null)
				item.getView().setTag(this);
		}
		
		notifyDataSetInvalidated();
	}
}
