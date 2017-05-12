<?php	
	//header("Content-type:text/html;charset=utf-8");
	require("../htbase/mysql_conf.php");
	require("../htbase/func.php");
	
	//writelog("insert fail", 2, "ht");
	//http://112.74.66.187/ht/activate_device.php?dev_idfa=asfaf&dev_type=1&system=2&dev_ip=192.168.1.1&region=chongqi

	session_start();
	
	$ret_json = false;
	$dev_type = '';
	$system   = '';
	$region   = '';
	
	if (isset($_REQUEST["dev_idfa"]))
	{
		//$msg = $_SERVER["REQUEST_URI"].'?'.$_SERVER['QUERY_STRING'];
		$msg = request_uri();
		writelog($msg, 2, "ht");
		
		$dev_idfa  = $_REQUEST["dev_idfa"];
		$dev_type  = $_REQUEST["dev_type"];		
		$system    = $_REQUEST["system"];
		//$createtts = $_REQUEST["createtts"];
		$dev_ip    = $_SERVER['REMOTE_ADDR'];//$_REQUEST["dev_ip"];
		$region    = $_REQUEST["region"];
			
		/*$postdata  = "dev_idfa=".$_REQUEST["dev_idfa"]."&dev_ip=".$_REQUEST["dev_ip"]."&dev_type=".$_REQUEST["dev_type"];
		$postdata .= "&region=".$_REQUEST["region"]."&system=".$_REQUEST["system"];		
		echo $postdata."\n";*/	
	}
	else
	{
		$jsondata = file_get_contents("php://input");
		writelog($jsondata, 2, "ht");
		
		$data = json_decode($jsondata);
		$dev_idfa  = $data->dev_idfa;
		$dev_type  = $data->dev_type;	
		$system    = $data->system;
		$dev_ip    = $_SERVER['REMOTE_ADDR'];
		$region    = $data->region;
		//echo $jsondata."\n";		
	}
	
	$mysqli = new mysqli($dbhost, $dbuser, $dbpwd, $dbname);
	if (mysqli_connect_errno())
	{
		$msg = "Connect db failed!".mysqli_connect_error();
		writelog($msg, 2, "ht");
		param_check_fail($ret_json, 1, $msg);
		exit(); 
	}
	
	$hash = PHP_BKDRHash($dev_idfa);
	$index = $hash % 5 + 1;
	$msg = "dev_idfa=".$dev_idfa." hashindex=".$index;
	writelog($msg, 1, "ht");
	
	$sql  = sprintf("insert activated_device%d (dev_idfa, dev_type, system, createtts, dev_ip, region) ", $index);
	$sql .= sprintf("values ('%s', '%s', '%s', '%s', '%s', '%s')", $dev_idfa, $dev_type, $system, date('Y-m-d H:i:s'), $dev_ip, $region);
	//echo "index=".$index."  sql=".$sql."</br>";
	$affect = $mysqli->query($sql);
	if (!$affect)
	{
		writelog("insert fail", 2, "ht");
		fail_proc($mysqli, $ret_json, 0, unicode_encode("insert ok"));
		return;		
	}

    fail_proc($mysqli, $ret_json, 0, unicode_encode("insert ok"));
	//writelog("insert ok", 2, "ht");
?>