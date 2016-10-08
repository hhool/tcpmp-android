package com.jumplayer.object;
//视频编码信息
public class MediaVideoInfo {
	int 			mType;			//流类型，内部使用
	public int 		mStreamID;		//流ID
	public int 		mByteRate;		//码率
	public String 	mFormat;		//压缩格式
	public String 	mCodec;			//压缩格式详细描述
	public int 		mFps;			//帧率
	public int 		mWidth;			//宽
	public int 		mHeight;		//高
}
