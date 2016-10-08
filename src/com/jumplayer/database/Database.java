package com.jumplayer.database;

import java.util.LinkedList;
import com.jumplayer.object.MediaThumbnail;

import android.content.ContentValues;
import android.content.Context;
import android.database.Cursor;
import android.database.sqlite.SQLiteDatabase;

public class Database 
{
	private SqliteOpenHelper mHelper;
	
	public Database(Context context)
	{
		mHelper = new SqliteOpenHelper(context);
	}
	
	public void insertThumbNail(String szFilePath, long lLastDate, String szThumbnailPath, int iDuration, int iState)
	{
		SQLiteDatabase db = null;
		try
		{
			db = mHelper.getWritableDatabase();

//			String szSql = "insert into file (FilePath, LastDate, ThumbnailPath, Duration) values(?,?,?,?)";
//			db.execSQL(szSql,
//					new Object[] { szFilePath, iFileTime, szThumbnailPath, iDuration});
			ContentValues cv = new ContentValues();
			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[0], szFilePath);
			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[1], lLastDate);
			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[2], szThumbnailPath);
			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[3], iDuration);
			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[4], iState);
			
			db.insert(SqliteOpenHelper.g_Database_TableName, null, cv);
		}
		finally
		{
			closeDB(db);
		}		
	}
	
//	public void updateThumbNail(String szFilePath, int iFileTime, String szThumbnailPath, int iDuration)
//	{
//		SQLiteDatabase db = null;
//		try
//		{
//			db = mHelper.getWritableDatabase();
//
//			ContentValues cv = new ContentValues();
//			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[0], szFilePath);
//			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[1], iFileTime);
//			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[2], szThumbnailPath);
//			cv.put(SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[3], iDuration);			
//			
//			String szWhere = SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[0] + "=?," +
//								SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[1] + "=?";
//			
//			String args[] = {szFilePath, String.valueOf(iFileTime)};
//			
//			db.update(SqliteOpenHelper.g_Database_TableName, cv, szWhere, args);
//		}
//		finally
//		{
//			closeDB(db);
//		}				
//	}

	public int queryThumbNail(LinkedList<MediaThumbnail> vThumbnails)
	{
		MediaThumbnail tn = null;
		SQLiteDatabase db = null;
		try
		{
			db = mHelper.getWritableDatabase();

			Cursor cs = db.query(SqliteOpenHelper.g_Database_TableName
					, SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames
					, null, null, null, null, null);

			if(cs.getCount() > 0 && cs.moveToFirst())
			{
				while(cs.moveToNext())
				{
					tn = new MediaThumbnail();
					tn.setFilePath(cs.getString(0));
					tn.setLastDate(cs.getLong(1));
					tn.setThumbnailPath(cs.getString(2));
					tn.setDuration(cs.getInt(3));
					tn.setState(cs.getInt(4));
					
					vThumbnails.addLast(tn);
				}
			}
			
			cs.close();
		}
		finally
		{
			closeDB(db);
		}
		
		return vThumbnails.size();		
	}
	
	public MediaThumbnail queryThumbNail(String szFilePath, long lLastDate)
	{
		MediaThumbnail tn = null;
		SQLiteDatabase db = null;
		try
		{
			db = mHelper.getWritableDatabase();

			String szSelection = SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[0] + "=? and " + SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[1] + "=?";
			String szSelectionArgs[] = {szFilePath, String.valueOf(lLastDate)};
			
			Cursor cs = db.query(SqliteOpenHelper.g_Database_TableName
					, SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames
					, szSelection, szSelectionArgs, null, null, null);

			if(cs.getCount() > 0 && cs.moveToFirst())
			{
				tn = new MediaThumbnail();
				tn.setFilePath(cs.getString(0));
				tn.setLastDate(cs.getLong(1));
				tn.setThumbnailPath(cs.getString(2));
				tn.setDuration(cs.getInt(3));
				tn.setState(cs.getInt(4));
			}
			
			cs.close();
		}
		finally
		{
			closeDB(db);
		}
		
		return tn;
	}
	
	public void removeThumbnail(String szMediaPath, long lLastDate)
	{
		SQLiteDatabase db = null;
		
		try
		{
			db = mHelper.getWritableDatabase();
			
			String szSelection = SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[0] + "=? and " + SqliteOpenHelper.g_szaDatabase_ThumbNail_FieldNames[1] + "=?";
			String szSelectionArgs[] = {szMediaPath, String.valueOf(lLastDate)};
			db.delete(SqliteOpenHelper.g_Database_TableName, szSelection, szSelectionArgs);
		}
		finally
		{
			closeDB(db);
		}
	}
	
	private void closeDB(SQLiteDatabase db) 
	{
		if (db != null) 
		{
			try 
			{
				db.close();
				db = null;
			} 
			catch (Exception e) 
			{
				db = null;
			}
		}
	}
}
