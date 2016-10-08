package com.jumplayer.object;

import java.io.File;

import android.graphics.Bitmap;
import android.view.View;

public class FileListItem 
{
	private File mFile;
	private View mView;
	private Bitmap bmp;
	
	public FileListItem()
	{
		mFile = null;
		mView = null;
		bmp   = null;
	}
	
	public void setFile(File File) 
	{
		this.mFile = File;
	}
	public File getFile() 
	{
		return mFile;
	}
	
	public void setBitmap(Bitmap bm)
	{
		bmp = bm;
	}
	public Bitmap getBitmap()
	{
		return bmp;
	}
	public void setView(View View) 
	{
		this.mView = View;
	}
	public View getView() 
	{
		return mView;
	}
	
	
	
}
