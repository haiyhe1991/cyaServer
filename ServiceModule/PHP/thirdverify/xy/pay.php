<?php	
	header("Content-type:text/html;charset=utf-8");
	require("../../htbase/mysql_conf.php");
	require("../../htbase/func.php");
	
	session_start();
	
	//amount=1.00&extra=201509021741094318032888358001&orderid=100022730_26176109_1441186873_8832&serverid=0&ts=1441188002&uid=26176109
	//&sign=bb291e0be277e15c0cc077d35718b06e&sig=9815f7ab6de22a09b2d45ff0d049660d
	
	$ret_json = false;
	$sign = "";
	$sig  = "";
	$plattype = PLAT_XY;
	$platid	  = "";	
	if (isset($_REQUEST["extra"]))
	{
		//$msg = $_SERVER["REQUEST_URI"].'?'.$_SERVER['QUERY_STRING'];
		$msg = request_uri();
		writelog($msg, 2, "xy");
		
		$orderid   = $_REQUEST["extra"];
		$platid    = $_REQUEST["orderid"];		
		$amount    = $_REQUEST["amount"];
		$sign 	   = $_REQUEST["sign"];
		if (isset($_REQUEST["sig"]))
			$sig = $_REQUEST["sig"];
		
		$postdata  = "amount=".$_REQUEST["amount"]."&extra=".$_REQUEST["extra"]."&orderid=".$_REQUEST["orderid"]."&serverid=".$_REQUEST["serverid"];
		$postdata .= "&ts=".$_REQUEST["ts"]."&uid=".$_REQUEST["uid"];
		
		$postdata = urldecode($postdata);
		//echo $postdata."\n";
	}
	else
	{
		$jsondata = file_get_contents("php://input");
		writelog($jsondata, 2, "xy");
		
		$data = json_decode($jsondata);
		$orderid = $data->extra;
		$platid  = $data->orderid;
		$amount  = $data->amount;
		$sign    = $data->sign;
		if (isset($data->sig))
			$sig = $data->sig;	
		
		$postdata  = "amount=".$amount."&extra=".$orderid."&orderid=".$platid."&serverid=".$data->serverid;
		$postdata .= "&ts=".$data->ts."&uid=".$data->uid;
				
		//echo $orderid."\n";	
	}
	
	//appkey: b6da5900312eaaae2b3f78b9073106f0
	//paykey: tdTmL9KWaNBwsW40FH7FVQKEYkx9UYfk
	//AppKey：dGfotkgMnl5VXGsvpFhs9OBQKIelV8H5
	//PayKey：tZAkPmwDPukNao7yShJihI1BAELnVzJr 
	
	if ("" === $sign)
	{
		writelog("no sign error", 2, "xy");
		param_check_fail($ret_json, 6, unicode_encode("签名校验错误"));
		return;
	}
	$signstr = "dGfotkgMnl5VXGsvpFhs9OBQKIelV8H5".$postdata;
	if ($sign != md5($signstr))
	{
		writelog("sign verify fail", 2, "xy");
		param_check_fail($ret_json, 6, unicode_encode("签名校验错误"));
		return;			
	}
	
	if ("" !== $sig)
	{
		$sigstr = "tZAkPmwDPukNao7yShJihI1BAELnVzJr".$postdata;
		if ($sig != md5($sigstr))
		{
			writelog("sig verify fail", 2, "xy");
			param_check_fail($ret_json, 6, unicode_encode("签名校验错误"));
			return;			
		}
	}
	
	$mysqli = new mysqli($dbhost, $dbuser, $dbpwd, $dbname);
	if (mysqli_connect_errno())
	{
		$msg = "Connect db failed!".mysqli_connect_error();
		writelog($msg, 2, "xy");
		param_check_fail($ret_json, 7, unicode_encode("数据库错误"));
		exit(); 
	}	
	
	$hash = PHP_BKDRHash($orderid);
	$index = $hash % 5 + 1;
	$msg = "billno=".$orderid." hashindex=".$index;
	writelog($msg, 1, "xy");	
	
	$sql_query = sprintf("select orderid, rmb, accountid, status, confirmtts from player_cash_%d where orderid='%s'", $index, $orderid);
	$res_query = $mysqli->query($sql_query);
	if (!$res_query)
	{
		$msg = "orderid=".$orderid.' no data';
		writelog($msg, 2, "xy");
		fail_proc($mysqli, $ret_json, 5, unicode_encode("透传信息错误"));
		return;
	}
	
	$rows    = $res_query->num_rows;
	$columns = $res_query->field_count;
	
	// 获取一条记录
	$row = $res_query->fetch_assoc();
	// 关闭查询结果(因只取一条记录,可以关闭了)
	$res_query->close();	
	
	if ( !((1 === $rows) && (5 === $columns)) )
	{
		$msg = "orderid=".$orderid.' no match record error';
		writelog($msg, 2, "xy");
		fail_proc($mysqli, $ret_json, 8, unicode_encode("其它错误"));
		return;
	}

	if ($amount != $row['rmb']) //  !==
	{
		$msg = "orderid=".$orderid."amount=".$amount." not equal to rmb=".$row['rmb'];
		writelog($msg, 2, "xy");
		fail_proc($mysqli, $ret_json, 1, unicode_encode("参数错误"));
		return;
	}
	
	if (0 != $row['status'])
	{
		$msg  = "orderid=".$orderid;
		$msg .= " table status=".$row['status']." error";
		writelog($msg, 2, "xy");
		fail_proc($mysqli, $ret_json, 4, unicode_encode("订单已存在"));
		return;
	}

	$sql = sprintf("update player_cash_%d set status=1, platformid='%s', platformorderid='%s', confirmtts='%s' where orderid='%s'", $index, $plattype, $platid, date('Y-m-d H:i:s'), $orderid);
	$affect = $mysqli->query($sql);
	if (!$affect)
	{
		$msg = "orderid=".$orderid.' update table failed';
		writelog($msg, 2, "xy");
		fail_proc($mysqli, $ret_json, 7, unicode_encode("数据库错误"));
		return;		
	}

	fail_proc($mysqli, $ret_json, 0, unicode_encode("发货成功"));
	//writelog("confirm ok", 2, "xy");
?>
