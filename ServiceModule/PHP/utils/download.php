<?php
	function DownloadFile($filename, $showName)
	{
		if(file_exists($filename) == false)
			return ;
	
		$pf = fopen($filename, "r");
		if(!$pf)
			return ;
		$fileSize = filesize($filename);
	
		Header("Content-type: application/octet-stream"); 
		Header("Accept-Ranges: bytes"); 
		Header("Accept-Length:".$fileSize); 
		Header("Content-Disposition: attachment; filename=".$showName);
	
		$buffer = 1024; 
		$fileCount = 0; 
		//向浏览器返回数据 
		while(!feof($pf) && $fileCount < $fileSize)
		{ 
			$data = fread($pf, $buffer); 
			$fileCount += $buffer; 
			echo $data;
		}
		fclose($pf);
	}
	
	function GetDownloadFile(&$filepath)
	{
		foreach ($_GET as $key => $value) 
		{
			if($key == "file")
			{
				$filepath = $value;
				break;
			}
		}
	}
	
	$filepath = "";
	GetDownloadFile($filepath);
	if($filepath != "")
	{
		$filename = substr(strrchr($filepath, "/"), 1);
		DownloadFile($filepath, $filename);
	}
?>