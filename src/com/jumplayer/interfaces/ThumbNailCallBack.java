package com.jumplayer.interfaces;


import com.jumplayer.object.MediaThumbnail;

public interface ThumbNailCallBack 
{
	public void setThumbNail(String szPath, MediaThumbnail tn);
	
	public void onThumbnailThreadPrepared();
	
	public void onThumbnailThreadException();
}
