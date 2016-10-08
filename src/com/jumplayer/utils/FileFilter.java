package com.jumplayer.utils;

import java.io.File;
import java.io.FilenameFilter;
import java.util.Comparator;

import com.jumplayer.object.PlayerCore;

public class FileFilter implements FilenameFilter
{
	private PlayerCore mPlayerCore;

	public FileFilter(PlayerCore PlayerCore)
	{
		mPlayerCore = PlayerCore;
	}
	
	public boolean accept(File dir, String filename) 
	{
		// TODO Auto-generated method stub
		File file = new File(dir.getAbsolutePath() + "/" + filename);
		if (file.isDirectory())
			return true;
		String name = filename.toLowerCase();
		if(mPlayerCore.IsFileSupport(name))
		{
			return true;
		}
		
		return false;
	}
	
	public static final Comparator<File> FileNameComparator = new Comparator<File>() 
	{
		public int compare(File o1, File o2) 
		{
			int iCompare = o1.getName().compareToIgnoreCase(o2.getName()); 
			if(o1.isDirectory()&&o2.isFile())
				return -1;
			if(o2.isDirectory()&&o1.isFile())
				return 1;
			else if(iCompare == 0)
				return 0;
			else if(iCompare < 0)
				return -1;
			else 
				return 1;
		}
	};	
}
