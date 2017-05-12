<?php
	header("Content-type:text/html;charset=utf-8");
	
	session_start();
	
	if (!isset($_REQUEST["CMD"]))
	{
		// http://123.59.67.84/thirdverify/ht/notice.php?CMD=10000&System=ios&PlatID=1001
		echo "Usage: "."http://ipaddr/notice.php?CMD=10000&System=ios&PlatID=1001"; 
		return;
	}
	
	//date_default_timezone_set('PRC');
	switch ($_REQUEST["CMD"]) 
	{ 
	case "10000" : 
		$str    = file_get_contents("./announcement.json");
		$assoc  = json_decode($str, true);
		$isopen = 0;

		// 二维关联数组解析为二维数组
		foreach($assoc as $arr)
		{
			// 二维数组解析为关联数组
			foreach($arr as $key => $value)
			{
				$isopen = $value["IsOpen"];
				//echo 'isopen='.$isopen.'</br>';
				if (1 == $isopen)
				{
					$start = strtotime($value["StartTime"]);
					$end   = strtotime($value["EndTime"]);
					$now   = time();
					if (($now > $start) && ($now < $end))
					{
						//print_r($value);
						$content = array();
						// 关联数组解析
						foreach($value as $keytmp => $valuetmp)
						{
							$content[urlencode($keytmp)] = urlencode($valuetmp);
						}
						echo urldecode(json_encode($content));	
					}
				}
			}
		}
		break;
		
	default:
		echo "Unknown command: ".$_REQUEST["CMD"];
	}
?>