package com.jumplayer.database;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;
import android.os.Environment;

public class SqliteOpenHelper extends SQLiteOpenHelper
{
	public static final String g_Database_TableName = "thumbnail";
	
	public static final String[] g_szaDatabase_ThumbNail_FieldNames	= {"FilePath", "LastDate", "ThumbnailPath", "Duration", "ThumbnailState"};

	public static final int g_siDatabase_ThumbNail_State_Fails	= 0;
	public static final int g_siDatabase_ThumbNail_State_Success	= 1;

	public static final String g_szThumbnailDir = Environment.getExternalStorageDirectory().getAbsolutePath() + "/JumpThumbnails";
	
	public SqliteOpenHelper(Context context) 
	{
		super(context, "JumPlayer.db", null, 1);
	}
	
	public void onCreate(SQLiteDatabase db) 
	{
		db.execSQL("CREATE TABLE IF NOT EXISTS " + g_Database_TableName + " (" 
				+ g_szaDatabase_ThumbNail_FieldNames[0] + " varchar, " 
				+ g_szaDatabase_ThumbNail_FieldNames[1] + " datatime, " 
				+ g_szaDatabase_ThumbNail_FieldNames[2] + " verchar, " 
				+ g_szaDatabase_ThumbNail_FieldNames[3] + " time, "
				+ g_szaDatabase_ThumbNail_FieldNames[4] + " integer, "
				+ "primary key("+g_szaDatabase_ThumbNail_FieldNames[0]+", " + g_szaDatabase_ThumbNail_FieldNames[1] +"))");
	}
	
	public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
		// TODO Auto-generated method stub
	}
}
