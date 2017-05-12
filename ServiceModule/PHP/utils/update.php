<?php
	require_once 'enum_dir_file.php';
	//枚举更新文件
	function EnumUpdateFile($ver, $directory, $cltDirFile)
	{
		$dirArray = array();
		EnumDir($directory, $dirArray);
		$dirNum = count($dirArray);
		if($dirNum <= 0)
		{
			echo "{\"version\":" . "\"" . $ver . "\"}";
			return ;
		}
	
		//列举需要更新文件
		$newVer = $ver;
		$fileArray = array();
		foreach ($dirArray as $key => $value) 
		{
			if(strcmp($key, $ver) <= 0)
				continue;
			
			EnumDirFiles($value, $fileArray);
		
			if(strcmp($key, $newVer) > 0)
				$newVer = $key;
		}
	
		//发送需要更新文件(json格式)
		$num = count($fileArray);
		if($num <= 0)
		{
			echo "{\"version\":" . "\"" . $ver . "\"}";
			return ;
		}

		$cltDirArr = array();
		GetClientDirInfo($cltDirArr, $cltDirFile);
		$si = count($cltDirArr);
		if($si <= 0)
		{
			echo "{\"version\":" . "\"" . $ver . "\"}";
			return ;
		}
		
		$data = "{\"version\":" . "\"". $newVer . "\"," . "\"files\":[";
		foreach ($fileArray as $key => $value) 
		{
			$dirPath = @$cltDirArr[$key];
			if($dirPath == "")
				continue;
			$data = $data . "{\"file\":" . "\"" . $value. "\",";
			$data = $data . "\"dir\":" . "\"" . $dirPath . "\"";
			$data = $data . "},";
		}
		$data = substr($data, 0, strlen($data) - 1);
		$data = $data . "]}";
		echo $data;
	}
	
	//获取当前版本号, 平台
	$ver = "";
	$upPlat = "";
	$upGpu = "";
	$keyArray = $_GET;
	$ver = @$keyArray["version"];
	if($ver == "")
	{
		echo "{\"version\":" . "\"" . "unknown" . "\"}";
		return ;
	}
	foreach($keyArray as $key => $value)
	{
		if($key == "plat")
			$upPlat = $value;
		else if($key == "gpu")
			$upGpu = $value;
			
		if($upPlat != "" && $upGpu != "")
			break;
	}
	
	if($upPlat != "" && $upGpu != "")	//更新资源文件
	{
		$directory = $_SERVER['DOCUMENT_ROOT'] . "/resource/" . $upGpu;
		$cltDirFile = $_SERVER['DOCUMENT_ROOT'] . "/conf/" . $upPlat . "_client_dir.conf";
		EnumUpdateFile($ver, $directory, $cltDirFile);
	}
	else if($upPlat != "")	//更新普通文件
	{
		$directory = $_SERVER['DOCUMENT_ROOT'] . "/sg/" . $upPlat;
		$cltDirFile = $_SERVER['DOCUMENT_ROOT'] . "/conf/" . $upPlat . "_client_dir.conf";
		EnumUpdateFile($ver, $directory, $cltDirFile);
	}
	else
	{
		echo "{\"version\":" . "\"" . $ver . "\"}";
	}
?>