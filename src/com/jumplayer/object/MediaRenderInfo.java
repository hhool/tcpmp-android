package com.jumplayer.object;
//当前播放帧数情况
public class MediaRenderInfo {
	int mTotal;					//当前文件播放渲染帧数的总数
	int mDropped;				//当前文件播放渲染帧数的丢失总数
	float mPlayFPS;				//当前文件播放动态帧率
}
