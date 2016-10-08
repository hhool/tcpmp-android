package com.jumplayer.object;

public interface  PlayerListener {

	//返回当前播放器播放状态PLAY_STATE_PLAY，PLAY_STATE_PAUSE,等
	public abstract void PlayerNotify(int MsgId);
	//返回播放器媒体播放时的错误信息描述
	public abstract void ErrorNotify(String strMsg);	
	//字幕绘制，有无字符串信息时绘制，
	public abstract void SubtitleNotify(String strSubtitle);
}
