package com.jumplayer.utils;

public class TimeUtil {

	public static String formatLongToTimeStr(long l) {
        String str = "";
        long minute = 0;
        long second = 0;

        if(l < 0)
        	l = 0;
        
        second = l / 1000;

        if (second > 60) {
            minute = second / 60;
            second = second % 60;
        }
        if(minute<10)
        	str = "0";
        str += minute;
        
        str +=":";
        
        if(second<10)
        	str += "0";
        str +=second;
        
        return str;
    }
}
