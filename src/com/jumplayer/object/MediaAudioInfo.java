package com.jumplayer.object;
//音频编码信息
public class MediaAudioInfo {
	int 	mType;				//流类型，内部使用
	public int 	mStreamID;		//流ID
	public int 	mByteRate;		//码率
	public String	mFormat;	//压缩格式
	public String 	mCodec;		//压缩格式详细描述
	public int 	mChannels;		//声道 1,Mono,2Stereo
	public int 	mSampleRate;	//采样率
}
