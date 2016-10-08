package com.jumplayer.utils;

import com.jumplayer.interfaces.ThumbNailCallBack;
import com.jumplayer.object.MediaThumbnail;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

public class MediaInfoScannerHandler extends Handler
{
	public static final int g_sfi_MSG_THUMBNAIL_PREPARED = 0;
	public static final int g_sfi_MSG_THUMBNAIL_BEGIN = 1;
	public static final int g_sfi_MSG_THUMBNAIL_STEP = 2;
	public static final int g_sfi_MSG_THUMBNAIL_OVER = 3;
	public static final int g_sfi_MSG_THUMBNAIL_EXCEPTION = 4;
	
	private ThumbNailCallBack mCallback;
	
	public MediaInfoScannerHandler(ThumbNailCallBack callback)
	{
		mCallback = callback;
	}
	
	public void handleMessage(Message msg) 
	{
		// TODO Auto-generated method stub
		switch(msg.what)
		{
		case g_sfi_MSG_THUMBNAIL_STEP:
			{
				Bundle data = msg.getData();
				mCallback.setThumbNail(data.getString("filepath"), (MediaThumbnail)msg.obj);
			}
			break;
		case g_sfi_MSG_THUMBNAIL_PREPARED:
			{
				mCallback.onThumbnailThreadPrepared();
			}
			break;
		case g_sfi_MSG_THUMBNAIL_EXCEPTION:
			{
				mCallback.onThumbnailThreadException();
			}
			break;
		}
		super.handleMessage(msg);
	}
}
