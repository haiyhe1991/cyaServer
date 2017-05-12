<?php
	function EnumDir($directory, &$dirArray)
	{
		if(!is_dir($directory))
			return ;
			
		$dirObj = @dir($directory);
		if(false == is_object($dirObj))
			return ;
		
		while(false != ($file = $dirObj->read()))
		{ 
			if($file != "." && $file != ".." && is_dir("$directory/$file"))
			{
				$dirArray[$file] = "$directory/$file";
			}
		}
		$dirObj->close();
	}
	
	function EnumDirFiles($fileDir, &$fileArray)
	{
		if(!is_dir($fileDir))
			return ;
			
		$fileDirObj = @dir($fileDir);
		if(false == is_object($fileDirObj))
			return ;
		
		while(false != ($file = $fileDirObj->read()))
		{ 
			if($file != "." && $file != ".." && is_file("$fileDir/$file"))
			{
				$fileArray[$file] = "$fileDir/$file";
			}
		}
		$fileDirObj->close();
	}
	
	function GetClientDirInfo(&$info, $filename)
	{
		if(file_exists($filename) == false)
			return ;
			
		$pf = fopen($filename, "r");
		if(!$pf)
			return ;
			
		$fileSize = filesize($filename);
		if($fileSize <= 0)
			return ;
			
		while(!feof($pf))
		{
			$str = fgets($pf);
			$strArr = explode(' ', $str);
			$si = count($strArr);
			if($si < 2)
				continue;
			$info[trim($strArr[0])] = trim($strArr[1]);
		}
		fclose($pf);
	}
?>