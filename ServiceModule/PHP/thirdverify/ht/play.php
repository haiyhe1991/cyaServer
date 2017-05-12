<?php	
	header("Content-type:text/html;charset=utf-8");
	require("../htbase/mysql_conf.php");
	require("../htbase/func.php");

	//http://123.59.67.84/thirdverify/ht/play.php?CMD=100&DevID=34BC4077-2016-4116-BFDA-480D01F7370F&DevType=iPadMini2&System=100&sysVersion=8.1000&PlatID=1001&Region=china
	//http://123.59.67.84/thirdverify/ht/play.php?CMD=103&DevID=111222&User=Alice

	session_start();
	
	$logdir = 'play';
	$mysqli = new mysqli($dbhost, $dbuser, $dbpwd, $dbname_action);
	if (mysqli_connect_errno())
	{
		$msg = "Connect db failed!".mysqli_connect_error();
		writelog($msg, 4, $logdir);
		exit(); 
	}
	
	$sql = '';
	if (isset($_REQUEST["CMD"]))
	{
		$msg = request_uri();
		writelog($msg, 2, $logdir);
		
		switch ($_REQUEST["CMD"]) 
		{ 
		case "100" : 
			if (isset($_REQUEST["DevID"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				$dev_type  = $_REQUEST["DevType"];		
				$version   = $_REQUEST["sysVersion"];
				$system    = $_REQUEST["System"];
				$region    = $_REQUEST["Region"];	
				$platid    = $_REQUEST["PlatID"];
				$dev_ip    = $_SERVER['REMOTE_ADDR'];
				$sql = sprintf("insert playdevice (dev_idfa, dev_type, system, version, dev_ip, region, platid, create_tts) values ('%s', '%s', '%u', '%s', '%s', '%s', '%u', '%s')", 
							$dev_idfa, $dev_type, $system, $version, $dev_ip, $region, $platid, date('Y-m-d H:i:s'));
			}
            break; 
			
		case "101" : 
			if (isset($_REQUEST["DevID"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];	
				//$version   = $_REQUEST["appVersion"];
				$sql = sprintf("update playdevice set version_check_tts='%s' where dev_idfa='%s'", date('Y-m-d H:i:s'), $dev_idfa);
			}
            break;
			
		case "102" : 
			if (isset($_REQUEST["DevID"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];	
				$sql = sprintf("update playdevice set play_ready_tts='%s' where dev_idfa='%s'", date('Y-m-d H:i:s'), $dev_idfa);
			}
            break;
			
		case "103" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[3]);
				$sql = sprintf("insert playrecord (user, dev_idfa, login_tts) values ('%s', '%s', '%s')", $user, $dev_idfa, date('Y-m-d H:i:s'));
				/*$sql = "insert playrecord (user, dev_idfa, login_tts) values ('发成', '111222', '2015-10-13 16:35:00')";
				echo $sql."</br>";
				$len = strlen($user);
				echo "</br>";
				echo $len."</br>";
				for ($i = 0; $i < $len; $i++)  
				{  
					$dec = bin2hex($user[$i]);
					echo $dec." \n";	
				}				
				echo "</br>";
				return;*/
			}
            break;	

		case "104" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[3]);
				$sql = sprintf("update playrecord set load_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "105" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[3]);
				$sql = sprintf("update playrecord set newbie_guide_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "106" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[3]);
				$sql = sprintf("update playrecord set pass_stage_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "107" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[3]);
				$sql = sprintf("update playrecord set begin_cartoon_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "108" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[3]);
				$sql = sprintf("update playrecord set play_cartoon_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;
			
		case "109" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["RoleID"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				$roleid    = $_REQUEST["RoleID"];
			
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[4]);
				
				$sql = sprintf("update playrecord set chosen_roleid='%d' where user='%s' and dev_idfa='%s'", $roleid, $user, $dev_idfa);
			}
            break;				
			
		case "110" : 
			if (isset($_REQUEST["DevID"]) && isset($_REQUEST["DLC"]) && isset($_REQUEST["User"]))
			{
				$dev_idfa  = $_REQUEST["DevID"];
				$dlc       = $_REQUEST["DLC"];
			
				$tmp  = $_SERVER["QUERY_STRING"];
				$arr  = explode('=', $tmp);
				$user = ConvertToUTF8($arr[4]);
				
				$sql = sprintf("update playrecord set dlc='%s' where user='%s' and dev_idfa='%s'", $dlc, $user, $dev_idfa);
			}
            break;				
			
		default : 
			writelog("CMD:".$_REQUEST["CMD"]." is invalid!", 3, $logdir);;
		} 		
		//echo $postdata."\n";	
	}
	else
	{
		$jsondata = file_get_contents("php://input");
		writelog($jsondata, 2, $logdir);
		
		$data = json_decode($jsondata);
		switch ($data->CMD) 
		{ 
		case "100" : 
			if (isset($data->DevID))
			{
				$dev_idfa  = $data->DevID;
				$dev_type  = $data->DevType;		
				$version   = $data->Version;
				$sql = sprintf("insert playdevice (dev_idfa, dev_type, version) values ('%s', '%s', '%s')", $dev_idfa, $dev_type, $version);								
			}
            break;	
		
		case "101" : 
			if (isset($data->DevID))
			{
				$dev_idfa  = $data->DevID;	
				//$version   = $data->Version;
				$sql = sprintf("update playdevice set version_check_tts='%s' where dev_idfa='%s'", date('Y-m-d H:i:s'), $dev_idfa);
			}
            break;
			
		case "102" : 
			if (isset($data->DevID))
			{
				$dev_idfa  = $data->Version;	
				$sql = sprintf("update playdevice set play_ready_tts='%s' where dev_idfa='%s'", date('Y-m-d H:i:s'), $dev_idfa);
			}
            break;

		case "103" : 
			if (isset($data->DevID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$sql = sprintf("insert playrecord (user, dev_idfa, login_tts) values ('%s', '%s', '%s')", $user, $dev_idfa, date('Y-m-d H:i:s'));
			}
            break;	

		case "104" : 
			if (isset($data->DevID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$sql = sprintf("update playrecord set load_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "105" : 
			if (isset($data->DevID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$sql = sprintf("update playrecord set newbie_guide_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "106" : 
			if (isset($data->DevID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$sql = sprintf("update playrecord set pass_stage_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "107" : 
			if (isset($data->DevID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$sql = sprintf("update playrecord set begin_cartoon_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;	

		case "108" : 
			if (isset($data->DevID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$sql = sprintf("update playrecord set play_cartoon_tts='%s' where user='%s' and dev_idfa='%s'", date('Y-m-d H:i:s'), $user, $dev_idfa);
			}
            break;
			
		case "109" : 
			if (isset($data->DevID) && isset($data->RoleID) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$roleid    = $data->RoleID;
				$sql = sprintf("update playrecord set chosen_roleid='%d' where user='%s' and dev_idfa='%s'", $roleid, $user, $dev_idfa);
			}
            break;
			
		case "110" : 
			if (isset($data->DevID) && isset($data->dlc) && isset($data->User))
			{
				$dev_idfa  = $data->DevID;
				$user      = ConvertToUTF8($data->User);
				$dlc       = $data->dlc;
				$sql = sprintf("update playrecord set dlc='%s' where user='%s' and dev_idfa='%s'", $dlc, $user, $dev_idfa);
			}
            break;
			
        default : 
			writelog("CMD:".$data->CMD." is invalid!", 3, $logdir);;
		} 
		//echo $jsondata."\n";		
	}
	
	if (!empty($sql))
	{
		//writelog($sql, 1, $logdir);
		$affect = $mysqli->query($sql);
		if (!$affect)
		{
			writelog("update db fail!", 3, $logdir);
			$mysqli->close();
			return;		
		}
		//writelog("update db ok!", 1, $logdir);
	}

	$mysqli->close();
?>