package com.jumplayer.object;

public interface  PlayerListener {

	//���ص�ǰ����������״̬PLAY_STATE_PLAY��PLAY_STATE_PAUSE,��
	public abstract void PlayerNotify(int MsgId);
	//���ز�����ý�岥��ʱ�Ĵ�����Ϣ����
	public abstract void ErrorNotify(String strMsg);	
	//��Ļ���ƣ������ַ�����Ϣʱ���ƣ�
	public abstract void SubtitleNotify(String strSubtitle);
}
