<?php	
	header("Content-type:text/html;charset=utf-8");
	require("../../htbase/mysql_conf.php");
	require("../../htbase/func.php");
	require("notify_url.php");
	
	session_start();
	
	$ret_json = false;
	$plattype = PLAT_I4;
	$platid	  = "";
	if (isset($_REQUEST["billno"]))
	{
		//$msg = $_SERVER["REQUEST_URI"].'?'.$_SERVER['QUERY_STRING'];
		$msg = request_uri();
		writelog($msg, 2, "i4");
		
		$orderid   = $_REQUEST["billno"];
		$platid    = $_REQUEST["order_id"];		
		$amount    = $_REQUEST["amount"];
		$postdata  = "account=".$_REQUEST["account"]."&amount=".$_REQUEST["amount"]."&app_id=".$_REQUEST["app_id"]."&billno=".$_REQUEST["billno"];
		$postdata .= "&order_id=".$_REQUEST["order_id"]."&role=".$_REQUEST["role"]."&status=".$_REQUEST["status"]."&zone=".$_REQUEST["zone"];
		//echo $postdata."\n";	
	}
	else
	{
		$jsondata = file_get_contents("php://input");
		writelog($jsondata, 2, "i4");
		
		$data = json_decode($jsondata);	
		$orderid = $data->billno;
		$platid  = $data->order_id;
		$amount  = $data->amount;
		//echo $billno."\n";		
	}
	
	$hash = PHP_BKDRHash($orderid);
	$index = $hash % 5 + 1;
	$msg = "billno=".$orderid." hashindex=".$index;
	writelog($msg, 1, "i4");
	
	$verify = new DecodeController;
	if (!$verify->AppReturn())
	{
		writelog("verify fail", 2, "i4");
		param_check_fail($ret_json, 1, "sign verify fail");
		return;
	}
	
	$mysqli = new mysqli($dbhost, $dbuser, $dbpwd, $dbname);
	if (mysqli_connect_errno())
	{
		$msg = "Connect db failed!".mysqli_connect_error();
		writelog($msg, 2, "i4");
		param_check_fail($ret_json, 1, $msg);
		exit(); 
	}
	
	$sql_query = sprintf("select orderid, rmb, accountid, status, confirmtts from player_cash_%d where orderid='%s'", $index, $orderid);
	$res_query = $mysqli->query($sql_query);
	if (!$res_query)
	{
		$msg = "orderid=".$orderid.' no data';
		writelog($msg, 2, "i4");
		fail_proc($mysqli, $ret_json, 1, $msg);
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
		writelog($msg, 2, "i4");
		fail_proc($mysqli, $ret_json, 1, $msg);
		return;
	}

	if ($amount != $row['rmb']) //  !==
	{
		$msg = "orderid=".$orderid."amount=".$amount." not equal to rmb=".$row['rmb'];
		writelog($msg, 2, "i4");
		fail_proc($mysqli, $ret_json, 1, $msg);		
		return;
	}
	
	if (0 != $row['status'])
	{
		$msg  = "orderid=".$orderid;
		$msg .= " table status=".$row['status']." error";
		writelog($msg, 2, "i4");
		fail_proc($mysqli, $ret_json, 1, $msg);
		return;
	}

	$sql = sprintf("update player_cash_%d set status=1, platformid='%s', platformorderid='%s', confirmtts='%s' where orderid='%s'", $index, $plattype, $platid, date('Y-m-d H:i:s'), $orderid);
        $affect = $mysqli->query($sql);
	if (!$affect)
	{
		$msg = "orderid=".$orderid.' update table failed';
		writelog($msg, 2, "i4");
		fail_proc($mysqli, $ret_json, 1, $msg);
		return;
	}

	fail_proc($mysqli, $ret_json, 0, unicode_encode("confirm ok"));
	//writelog("confirm ok", 2, "i4");
?>
