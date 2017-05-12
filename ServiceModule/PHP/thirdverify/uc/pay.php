<?php	
	header("Content-type:text/html;charset=utf-8");
	require_once("../../htbase/mysql_conf.php");
	require_once("../../htbase/func.php");
	
	session_start();
	//writelog('session_start', 1, "uc");
	
	$ret_json  = false;
	$platname  = "uc";
	$platid    = PLAT_UCJY;
	$platorder = "";
	$sign      = "";
	$cpordid   = "";	
		
	if (isset($_REQUEST["callbackInfo"]))
	{
		//$msg = $_SERVER["REQUEST_URI"].'?'.$_SERVER['QUERY_STRING'];
		$msg = request_uri();
		writelog($msg, 2, $platname);
		
		$orderid   = $_REQUEST["callbackInfo"];
		$platorder = $_REQUEST["orderId"];
		$amount    = $_REQUEST["amount"];
		$sign      = $_REQUEST["sign"];
		$status    = $_REQUEST["orderStatus"];
		$desc      = $_REQUEST["failedDesc"];
		if (isset($_REQUEST["cpOrderId"]))
		{
			$cpordid = $_REQUEST["cpOrderId"];
		}	
	}
	else
	{
		$jsondata = file_get_contents("php://input");
		writelog($jsondata, 2, $platname);
		
		$post = json_decode($jsondata);	
		if (isset($post->sign))
		{
			$sign = $post->sign;
		}	

		$data      = $post->data;	// 支付结果数据
		$orderid   = $data->callbackInfo;
		$platorder = $data->orderId; 		
		$amount    = $data->amount;
		$status    = $data->orderStatus;
		$desc      = $data->failedDesc;
		if (isset($data->cpOrderId))
		{
			$cpordid = $data->cpOrderId;
		}
		/*else
		{
			echo "no cpOrderId 111"."\n";
		}*/	

		//仅当客户端调用支付方法传入了transactionNumCP 参数时, 才会将原内容通过cpOrderId 参数透传回游戏服务端.
		//cpOrderId仅当回调时具备该参数时, 才需加入签名, 如无,则不需要加入签名(注意：长度不超过30)
		$postdata = "accountId=".$data->accountId."amount=".$amount."callbackInfo=".$orderid;
		if ("" != $cpordid)
		{
			$postdata .= "cpOrderId=".$data->cpOrderId;
		}
		$postdata .= "creator=".$data->creator."failedDesc=".$data->failedDesc."gameId=".$data->gameId;
		$postdata .= "orderId=".$data->orderId."orderStatus=".$data->orderStatus."payWay=".$data->payWay;
		//echo $postdata."\n";		
	}
	
	//只要业务逻辑正常, 验证签名成功, 都应该返回SUCCESS, 表示不需要“SDK 服务器”再次发起通知.
	//当业务逻辑异常(如:收到的SDK 服务器的充值结果通知内容的签名不正确,充值结果与提交的充值请求不符等), 认为需要再次通知,才返回FAILURE.
	if ("" === $sign)
	{
		writelog("no sign error", 3, $platname);
		param_check_fail($ret_json, 6, "sign verify fail", 'FAILURE');
		return;
	}
	
	$appkey = "202cb962234w4ers2aaa";
	$signstr = $postdata.$appkey;
	if ($sign != md5($signstr))
	{
		writelog("sign verify fail", 3, $platname);
		param_check_fail($ret_json, 6, "sign verify fail", 'FAILURE');
		return;			
	}
	
	$mysqli = new mysqli($dbhost, $dbuser, $dbpwd, $dbname);
	if (mysqli_connect_errno())
	{
		$msg = "Connect db failed!".mysqli_connect_error();
		writelog($msg, 4, $platname);
		param_check_fail($ret_json, 1, $msg, 'FAILURE');
		exit(); 
	}
	
	//对于充值结果为失败的，只要能成功接收，也应返回SUCCESS
	if ('F' === $status)
	{
		$msg = "pay fail, cause:".$desc;
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 0, $msg, 'FAILURE', 'SUCCESS');
		return;		
	}
	
	$hash = PHP_BKDRHash($orderid);
	$index = $hash % 5 + 1;
	$msg = "billno=".$orderid." hashindex=".$index;
	writelog($msg, 1, $platname);
	
	$sql_query = sprintf("select orderid, rmb, accountid, status, confirmtts from player_cash_%d where orderid='%s'", $index, $orderid);
	//echo $sql_query."\n";
	
	$res_query = $mysqli->query($sql_query);
	if (!$res_query)
	{
		$msg = "orderid=".$orderid.' no data';
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 1, $msg, 'FAILURE');
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
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 1, $msg, 'FAILURE');
		return;
	}

	if ($amount != $row['rmb']) //  !==
	{
		$msg = "orderid=".$orderid."amount=".$amount." not equal to rmb=".$row['rmb'];
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 1, $msg, 'FAILURE');	
		return;
	}
	
	//对同一个订单，如果先接收到支付失败，再接收到支付成功的通知，应以成功的支付结果为准，替换原接收到的失败的支付结果。
	// 一旦通知支付成功，不会再发生通知支付失败的情形。
	if (0 != $row['status'])
	{
		$msg  = "orderid=".$orderid;
		$msg .= " table status=".$row['status']." error";
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 0, $msg, 'FAILURE', 'SUCCESS');
		return;
	}

	$sql = sprintf("update player_cash_%d set status=1, platformid='%s', platformorderid='%s', confirmtts='%s' where orderid='%s'", $index, $platid, $platorder, date('Y-m-d H:i:s'), $orderid)
	$affect = $mysqli->query($sql);
	if (!$affect)
	{
		$msg = "orderid=".$orderid.' update table failed';
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 1, $msg, 'FAILURE');
		return;
	}

	fail_proc($mysqli, $ret_json, 0, "confirm ok", 'FAILURE', 'SUCCESS');
	//writelog("confirm ok", 1, $platname);
?>
