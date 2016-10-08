package com.jumplayer.object;

import android.graphics.Bitmap;

public class MediaThumbnail 
{
	private String 	mFilePath;
	private long	mLastDate;
	private String	mThumbnailPath;
	private int 	mDuration;
	private int 	mState;
	private Bitmap  mBitmap;
	
	
	public MediaThumbnail()
	{
		setFilePath(null);
		setLastDate(0);
		setThumbnailPath(null);
		setDuration(0);
	}


	public void setFilePath(String szFilePath) 
	{
		mFilePath = szFilePath;
	}


	public String getFilePath() 
	{
		return mFilePath;
	}


	public void setLastDate(long LastDate) 
	{
		this.mLastDate = LastDate;
	}


	public long getLastDate() 
	{
		return mLastDate;
	}


	public void setThumbnailPath(String ThumbnailPath) 
	{
		this.mThumbnailPath = ThumbnailPath;
	}


	public String getThumbnailPath() 
	{
		return mThumbnailPath;
	}


	public void setDuration(int Duration) 
	{
		this.mDuration = Duration;
	}


	public int getDuration() 
	{
		return mDuration;
	}


	public void setState(int iState) {
		this.mState = iState;
	}


	public int getState() {
		return mState;
	}


	public void setBitmap(Bitmap Bitmap) {
		this.mBitmap = Bitmap;
	}


	public Bitmap getBitmap() {
		return mBitmap;
	}
}
