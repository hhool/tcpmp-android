package com.jumplayer.utils;

import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.Iterator;
import java.util.LinkedList;
import com.jumplayer.database.Database;
import com.jumplayer.database.SqliteOpenHelper;
import com.jumplayer.object.MediaThumbnail;
import com.jumplayer.object.PlayerCore;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Bitmap.Config;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;

public class MediaInfoLooperHandler extends Handler
{
	Database mDatabase = null;
	PlayerCore mPlayer = null;
	private  final int THUMB_HEIGHT	= 90;
	private  final int THUMB_WIDTH 	= 90;
	
	public MediaInfoLooperHandler(Looper looper, PlayerCore core)
	{
		super(looper);
		mPlayer = core;
	}
	
	public void createDB(Context context)
	{
		mDatabase = new Database(context);
	}
	
	public Database getDatabase()
	{
		return mDatabase;
	}
	
	public boolean saveThumbnail(Database db, MediaThumbnail tn)
	{
		//Database db = mDatabase;//getDatabase();
		if(db != null)
		{
			Bitmap bmp = tn.getBitmap();
			if(bmp != null)
			{
				// thumbnail success 
				if(saveBmpToLoacal(bmp, tn.getThumbnailPath()))
				{
					// Success for save bmp to local
					db.insertThumbNail(tn.getFilePath(), tn.getLastDate(), tn.getThumbnailPath(), tn.getDuration(), SqliteOpenHelper.g_siDatabase_ThumbNail_State_Success);				
				}
				else
				{
					// exception happend
					return false;
				}
			}
			else
			{
				// thumbnail fails
				db.insertThumbNail(tn.getFilePath(), tn.getLastDate(), "", tn.getDuration(), SqliteOpenHelper.g_siDatabase_ThumbNail_State_Fails);				
			}
		}
		
		return true;
	}
	
	public void removeThumbnail(Database db, MediaThumbnail tn)
	{
		db.removeThumbnail(tn.getFilePath(), tn.getLastDate());
	}
	
	private boolean saveBmpToLoacal(Bitmap bmp, String path)
	{
		if(path == null)
			return false;
		
		File myCaptureFile = new File(path);
		if(myCaptureFile.exists())		// 重新截图
			myCaptureFile.delete();
		
		try {
			File dir = new File(myCaptureFile.getParent()+"/");
			dir.mkdirs();
			myCaptureFile.createNewFile();				
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
			return false;
		}
		
		////////////////////////////////////////
		// write bmp
        BufferedOutputStream bos;
		try {
			ByteArrayOutputStream baos = new ByteArrayOutputStream();
			
			bmp.compress(Bitmap.CompressFormat.PNG, 70, baos);
			baos.flush();
			bos = new BufferedOutputStream(new FileOutputStream(myCaptureFile));
			baos.writeTo(bos);
	        bos.flush();
	        bos.close();
		} 
		catch (FileNotFoundException e) 
		{
			e.printStackTrace();
			return false;
		}
		catch(IOException e)
		{
			e.printStackTrace();
			return false;
		}			
		///////////////////////////////////
		
		return true;
	}
	
	public MediaThumbnail queryThumbnail(Database db, String szMediaPath)
	{
		//Database db = mDatabase; //getDatabase();
		if(db != null && szMediaPath != null)
		{
			File fMedia = new File(szMediaPath);
			MediaThumbnail tn = db.queryThumbNail(szMediaPath, fMedia.lastModified());
			
			if(tn != null)
			{
				if(tn.getState() == SqliteOpenHelper.g_siDatabase_ThumbNail_State_Success)
				{
					Bitmap thumbnail = loadBmpFromLocal(tn.getThumbnailPath());
					if(thumbnail == null)
						return null;
					tn.setBitmap(thumbnail);
				}
				return tn;
			}
		}
		
		return null;
	}
	private Bitmap loadBmpFromLocal(String szThumbnailPath)
	{
		if(szThumbnailPath == null || szThumbnailPath.length() == 0)
			return null;
			
		File f = new File(szThumbnailPath);
		if(!f.exists())
			return null;
		
		try
	    {
	      BitmapFactory.Options options = new BitmapFactory.Options();
	      options.inSampleSize = 1;
	      return BitmapFactory.decodeFile(szThumbnailPath, options);
	    }
	    catch(Exception e)
	    {
	    //  Logger.e(e.toString());
	    }
	    
	    return null;
	}
	
	public Bitmap loadBmpFromPlayerCore(String szMediaPath)
	{
//		return loadBmpFromLocal("/mnt/sdcard/dongwei/icon_48.png");
		byte[] b = mPlayer.GetThumbnail(szMediaPath, THUMB_HEIGHT, THUMB_WIDTH,PlayerCore.THUMB_RGB565,1000);
		
		if (b == null) // We were not able to create a thumbnail for this item.
		{
			//get thumbnail at 0 ms position;
			b = mPlayer.GetThumbnail(szMediaPath, THUMB_HEIGHT, THUMB_WIDTH,PlayerCore.THUMB_RGB565,0);
			if(b == null)
				return null;
		}
		  
		Bitmap thumbnail = Bitmap.createBitmap(THUMB_HEIGHT, THUMB_WIDTH, Config.RGB_565);
//		int iDuration 	= mPlayer.GetDuration();//得到媒体时间长度
		thumbnail.copyPixelsFromBuffer(ByteBuffer.wrap(b));		
		
		return thumbnail;
	}

	public String genThumbnailPath(String szMediaPath)
	{
		boolean exists = Environment.getExternalStorageState().equals(android.os.Environment.MEDIA_MOUNTED);
		if(!exists)
			return null;
		
		File f = new File(szMediaPath);
		String szThumbnailPath = new String(SqliteOpenHelper.g_szThumbnailDir 
				+ "/"
				+ f.getName().substring(0, f.getName().indexOf("."))
				+ ".png");
		return szThumbnailPath;
	}
	
	public void collationThumbnails(Database db)
	{
		LinkedList<MediaThumbnail> vThumbnails = new LinkedList<MediaThumbnail>();
		db.queryThumbNail(vThumbnails);
		
		File fMedia = null;
		File fThumbnail = null;
		MediaThumbnail tn = null;
		Iterator<MediaThumbnail> iter = null;
		for(iter=vThumbnails.iterator(); iter.hasNext();)
		{
			tn = iter.next();

			fMedia = new File(tn.getFilePath());
			if(tn.getThumbnailPath().length() == 0)
				fThumbnail = null;
			else
				fThumbnail = new File(tn.getThumbnailPath());
			
			if(!fMedia.exists())
			{
				db.removeThumbnail(tn.getFilePath(), tn.getLastDate());
				if(fThumbnail != null)
					fThumbnail.delete();
			}
			else if(tn.getState() == SqliteOpenHelper.g_siDatabase_ThumbNail_State_Success
					&& !fThumbnail.exists())
			{
				db.removeThumbnail(tn.getFilePath(), tn.getLastDate());
			}
		}
	}
}
