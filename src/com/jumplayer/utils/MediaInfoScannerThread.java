package com.jumplayer.utils;

import java.io.File;

import com.jumplayer.database.SqliteOpenHelper;
import com.jumplayer.object.MediaThumbnail;
import com.jumplayer.object.PlayerCore;

import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Looper;
import android.os.Message;
//import android.util.Log;

public class MediaInfoScannerThread extends Thread
{
	///////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	// static
	private static final String   TAG = "JumPlayer";
	public static final int g_sfi_MSG_THUMBNAIL_COLLATION = 0;
	public static final int g_sfi_MSG_GET_THUMBNAIL = 1;

	private static MediaInfoScannerThread g_MediaInfo_ScannerThread_Object = null;
	public static MediaInfoScannerThread getInstance(Context context, PlayerCore player, MediaInfoScannerHandler notifyHandler)
	{
		if(g_MediaInfo_ScannerThread_Object == null)
			g_MediaInfo_ScannerThread_Object = new MediaInfoScannerThread(context, player, notifyHandler);
		else
			g_MediaInfo_ScannerThread_Object.setScannerHandler(notifyHandler);
		
		return g_MediaInfo_ScannerThread_Object;
	}
	
	public static MediaInfoScannerThread getInstance()
	{
		return g_MediaInfo_ScannerThread_Object;
	}
	
	public static void releaseInstance()
	{
		g_MediaInfo_ScannerThread_Object = null;
	}
	////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////
	
	private PlayerCore mPlayer = null;
	private Object mMutex = null;
	private Object mPanding = null;
	
	private boolean mSuspand = false;
	private boolean mScanning = false;;
	
	private Context mContext = null;
	//private Database mDatabase; 
	
	public void start()
	{
		if(g_MediaInfo_ScannerThread_Object!=null)
		{
			if(g_MediaInfo_ScannerThread_Object.getState()==State.NEW)
				super.start();
			
		}
	}
	
	public synchronized void setScanning(boolean b)
	{
		mScanning = b;
	}

	public synchronized boolean getScanning()
	{
		return mScanning;
	}
	
	public synchronized void setSuspand(boolean b)
	{
		mSuspand = b;
	}

	public synchronized boolean getSuspand()
	{
		return mSuspand;
	}
	
//	public synchronized Database getDatabase()
//	{
//		return mDatabase;
//	}
	
	public synchronized Context getContext()
	{
		return mContext;
	}
	
	public synchronized void removeAllMessage()
	{
		if(mLooperHandler != null)
		{
			while(mLooperHandler.hasMessages(g_sfi_MSG_GET_THUMBNAIL))
				mLooperHandler.removeMessages(g_sfi_MSG_GET_THUMBNAIL);			
		}
	}
	
	private MediaInfoScannerHandler mNotifyHandler = null;		// notify  for other threads
	private MediaInfoLooperHandler mLooperHandler = null;								// message for this
	
	private MediaInfoScannerThread(Context context, PlayerCore player, MediaInfoScannerHandler notifyHandler)
	{
		mPlayer = player;
		mNotifyHandler = notifyHandler;
		
		mMutex = new Object();
		mPanding = new Object();
		mSuspand = false;
		mScanning = false;
		
		mContext = context;
		//mDatabase = new Database(context);
	}
	
	public void setScannerHandler(MediaInfoScannerHandler notifyHandler)
	{
		mNotifyHandler = notifyHandler;
	}
	
	public void sendMessage(int iWhat, Object obj, boolean bAcquireThumbnail)
	{
		if(mLooperHandler != null)
		{
			Message msg = mLooperHandler.obtainMessage(iWhat);
			msg.obj = obj;
			if(bAcquireThumbnail)
				msg.arg1 = 1;
			else
				msg.arg1 = 0;
			mLooperHandler.sendMessage(msg);			
		}
	}

	public void waitForScannerThread()
	{
		synchronized(mMutex)
		{
			while(getScanning())
			{
				try {
					mMutex.wait();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}			
			}
		}
	}
	
	public void notifyScannerThread()
	{
		synchronized(mMutex)
		{
			mMutex.notify();
		}		
	}
	
	public void suspandScannerThread()
	{
		synchronized(mPanding)
		{
			setSuspand(true);
		}
	}
	
	public void resumeScannerThread()
	{
		synchronized(mPanding)
		{
			setSuspand(false);
			mPanding.notify();
		}
	}
		
	public void run() 
	{
		// TODO Auto-generated method stub
		Looper.prepare();
		
		mLooperHandler = new MediaInfoLooperHandler(Looper.myLooper(), mPlayer)
		{
			public void handleMessage(Message msg)
			{
				synchronized(mPanding)
				{
					while(getSuspand())		// Listen whether need to suspand
					{
						try {
							//Log.d(TAG, "mPanding.wait()");
							mPanding.wait();
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}			
					}					
				}					
				//Log.d(TAG, "mPanding.wait() over");
				switch(msg.what)
				{
				case -1:
					{
						mNotifyHandler.sendEmptyMessage(MediaInfoScannerHandler.g_sfi_MSG_THUMBNAIL_PREPARED);
					}
					break;
				case g_sfi_MSG_THUMBNAIL_COLLATION:
					{
						collationThumbnails(getDatabase());
					}
					break;
				case g_sfi_MSG_GET_THUMBNAIL:
					{
						setScanning(true);
						
						String szPath = (String)msg.obj;	
						
						MediaThumbnail tn = queryThumbnail(getDatabase(), szPath);
						if(tn == null)	
						{
							Bitmap thumbnail = null;
							int iDuration = 0;
							if(msg.arg1 == 1)	// 从数据库中没有查找到缩略图,并且,主选项要求从底层获取缩略图
							{
								thumbnail = loadBmpFromPlayerCore(szPath);
								iDuration 	= mPlayer.GetDuration();//得到媒体时间长度
								mPlayer.Stop(0);								
							}
							
							tn = new MediaThumbnail();
							tn.setFilePath(szPath);
							File fMedia = new File(szPath);
							tn.setLastDate(fMedia.lastModified());
							tn.setDuration(iDuration);
							tn.setBitmap(thumbnail);
							if(thumbnail == null)
							{
								tn.setThumbnailPath(null);
								tn.setState(SqliteOpenHelper.g_siDatabase_ThumbNail_State_Fails);
							}
							else
							{
								tn.setThumbnailPath(genThumbnailPath(szPath));
								tn.setState(SqliteOpenHelper.g_siDatabase_ThumbNail_State_Success);
							}
							
							if(msg.arg1 == 1)	// 只有主选项要求从底层获取缩略图时,才存入数据库
							{
								if(!saveThumbnail(getDatabase(), tn))
								{
									// exception happend when write to disk
									Message notify = mNotifyHandler.obtainMessage(MediaInfoScannerHandler.g_sfi_MSG_THUMBNAIL_EXCEPTION);
									mNotifyHandler.sendMessage(notify);
									break;
								}
							}
						}

						if(mNotifyHandler != null && tn.getState() == SqliteOpenHelper.g_siDatabase_ThumbNail_State_Success)
						{
							Message notify = mNotifyHandler.obtainMessage(MediaInfoScannerHandler.g_sfi_MSG_THUMBNAIL_STEP);
							Bundle data = new Bundle();
							data.putString("filepath", new String(szPath));
							notify.setData(data);
							notify.obj = tn;
							mNotifyHandler.sendMessage(notify);									
						}	
					}
					break;
				}
				setScanning(false);
				notifyScannerThread();
			}
		};
		
		mLooperHandler.createDB(getContext());
		mLooperHandler.sendEmptyMessage(-1);
		Looper.loop();
	}
	
//	private synchronized void insertThumbnail(Database db, String szMediaPath, Bitmap bmp, int iDuration)
//	{
//		//Database db = mDatabase;//getDatabase();
//		if(db != null)
//		{
//			String szThumbnailPath = genThumbnailPath(szMediaPath);
//			
//			saveBmpToLoacal(bmp, genThumbnailPath(szMediaPath));
//			
//			db.insertThumbNail(szMediaPath, 0, szThumbnailPath, iDuration);
//		}
//	}
//	
//	private void saveBmpToLoacal(Bitmap bmp, String path)
//	{
//		if(path == null)
//			return;
//		
//		boolean bExist = false;
//		File myCaptureFile = new File(path);
//		try {
//			File dir = new File(myCaptureFile.getParent()+"/");
//			dir.mkdirs();
//			bExist =myCaptureFile.createNewFile();				
//		} catch (IOException e1) {
//			// TODO Auto-generated catch block
//			e1.printStackTrace();
//		}
//		if(!bExist)
//		{
//	        BufferedOutputStream bos;
//			try {
//				bos = new BufferedOutputStream(new FileOutputStream(myCaptureFile));
//
//				bmp.compress(Bitmap.CompressFormat.PNG, 90, bos);
//		        bos.flush();
//		        bos.close();
//			} 
//			catch (FileNotFoundException e) 
//			{
//				e.printStackTrace();
//			}
//			catch(IOException e)
//			{
//				e.printStackTrace();
//			}			
//		}
//	}
//	
//	private synchronized Bitmap queryThumbnail(Database db, String szMediaPath)
//	{
//		//Database db = mDatabase; //getDatabase();
//		if(db != null)
//		{
//			MediaThumbnail tn = db.queryThumbNail(szMediaPath, 0);
//			
//			if(tn != null)
//				return loadBmpFromLocal(tn.getThumbnailPath());
//		}
//		
//		return null;
//	}
//	private Bitmap loadBmpFromLocal(String szThumbnailPath)
//	{
//		File f = new File(szThumbnailPath);
//		if(!f.exists())
//			return null;
//		
//		try
//	    {
//	      BitmapFactory.Options options = new BitmapFactory.Options();
//	      options.inSampleSize = 1;
//	      return BitmapFactory.decodeFile(szThumbnailPath, options);
//	    }
//	    catch(Exception e)
//	    {
//	    //  Logger.e(e.toString());
//	    }
//	    
//	    return null;
//	}
//	
//	private synchronized Bitmap loadBmpFromPlayerCore(String szMediaPath)
//	{
//		byte[] b = mPlayer.GetThumbnail(szMediaPath, 120, 90,PlayerCore.THUMB_RGB32,1000);
//		
//		if (b == null) // We were not able to create a thumbnail for this item.
//		{
//			//get thumbnail at 0 ms position;
//			b = mPlayer.GetThumbnail(szMediaPath, 120, 90,PlayerCore.THUMB_RGB32,0);
//			if(b == null)
//				return null;
//		}
//		  
//		Bitmap thumbnail = Bitmap.createBitmap(120, 90, Config.ARGB_8888);
////		int iDuration 	= mPlayer.GetDuration();//得到媒体时间长度
//		thumbnail.copyPixelsFromBuffer(ByteBuffer.wrap(b));		
//		
//		return thumbnail;
//	}
//
//	private String genThumbnailPath(String szMediaPath)
//	{
//		String szSD = FileListAdapter.getExternalStoragePath();
//		if(szSD.compareToIgnoreCase("/") == 0)
//			return null;
//		
//		File f = new File(szMediaPath);
//		String szThumbnailPath = new String(szSD 
//				+ "/Thumbnails/" 
//				+ f.getName().substring(0, f.getName().indexOf("."))
//				+ ".bmp");
//		return szThumbnailPath;
//	}
}
