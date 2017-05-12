<?php	
	header("Content-type:text/html;charset=utf-8");
	require_once("../../htbase/mysql_conf.php");
	require_once("../../htbase/func.php");
	require_once("SnsSigCheck.php");
	
	session_start();
	//writelog('session_start', 1, "tencent");
	
	$ret_json  = true;
	$platname  = "tencent";
	$platid    = PLAT_TENCENT;
	$platorder = "";
	$orderid   = "";
	$amount    = "";
	$sig       = "";
	
	if (isset($_REQUEST["billno"]))
	{
		//$msg = $_SERVER["REQUEST_URI"].'?'.$_SERVER['QUERY_STRING'];
		$msg = request_uri();
		writelog($msg, 2, $platname);
		
		$openid  = $_REQUEST["openid"];     //根据APPID以及QQ号码生成，即不同的appid下，同一个QQ号生成的OpenID是不一样的。  
		$appid   = $_REQUEST["appid"];      //应用的唯一ID。可以通过appid查找APP基本信息。  
		$ts      = $_REQUEST["ts"];         //linux时间戳。 注意开发者的机器时间与腾讯计费开放平台的时间相差不能超过15分钟。  
		$payitem = $_REQUEST["payitem"];    //接收标准格式为ID*price*num   G001*10*1  
		$token   = $_REQUEST["token"];      //应用调用v3/pay/buy_goods接口成功返回的交易token	
		$billno  = $_REQUEST["billno"];     //支付流水号（64个字符长度。该字段和openid合起来是唯一的）。  
		$version = $_REQUEST["version"];    //协议版本 号，由于基于V3版OpenAPI，这里一定返回“v3”。  
		$zoneid  = $_REQUEST["zoneid"];     //在支付营销分区配置说明页面，配置的分区ID即为这里的“zoneid”。  如果应用不分区，则为0。  
		$providetype  = $_REQUEST["providetype"];   //发货类型: 0表示道具购买，1表示营销活动中的道具赠送，2表示交叉营销任务集市中的奖励发放。  		
		$amt          = $_REQUEST["amt"];  //Q点/Q币消耗金额或财付通游戏子账户的扣款金额。可以为空 若传递空值或不传本参数则表示未使用Q点/Q币/财付通游戏子账户。注意，这里以0.1Q点为单位。           
		$payamt_coins = $_REQUEST["payamt_coins"];  				//扣取的游戏币总数，单位为Q点。  
		$pubacct_payamt_coins = $_REQUEST["pubacct_payamt_coins"];  //扣取的抵用券总金额，单位为Q点。
		  
		$appmeta      = $_REQUEST["appmeta"]; 
		$clientver    = $_REQUEST["clientver"];  
		$sig          = $_REQUEST["sig"]; 

		$param = array(
				'amt' => $amt,
				'appid' => $appid,
				'billno' => $billno,
				'openid' => $openid,
				'payamt_coins' => $payamt_coins,
				'payitem' => $payitem,
				'ts' => $ts,
				'providetype' => $providetype,
				'pubacct_payamt_coins' => $pubacct_payamt_coins,
				'token' => $token,
				'version' => $version,
				'zoneid' => $zoneid	
			);			
	}
	else
	{
		$jsondata = file_get_contents("php://input");
		writelog($jsondata, 2, $platname);
		
		$data    = json_decode($jsondata);	
		/*
		$openid  = $data->openid;     //根据APPID以及QQ号码生成，即不同的appid下，同一个QQ号生成的OpenID是不一样的。  
		$appid   = $data->appid;      //应用的唯一ID。可以通过appid查找APP基本信息。  
		$ts      = $data->ts;         //linux时间戳。 注意开发者的机器时间与腾讯计费开放平台的时间相差不能超过15分钟。  
		$payitem = $data->payitem;    //接收标准格式为ID*price*num   G001*10*1
		*/
		$token   = $data->token;      //应用调用v3/pay/buy_goods接口成功返回的交易token	
		$billno  = $data->billno;     //支付流水号（64个字符长度。该字段和openid合起来是唯一的）。
		$amt     = $data->amt;  //Q点/Q币消耗金额或财付通游戏子账户的扣款金额。可以为空 若传递空值或不传本参数则表示未使用Q点/Q币/财付通游戏子账户。注意，这里以0.1Q点为单位。           
		/*		
		$version = $data->version;    //协议版本 号，由于基于V3版OpenAPI，这里一定返回“v3”。  
		$zoneid  = $data->zoneid;     //在支付营销分区配置说明页面，配置的分区ID即为这里的“zoneid”。  如果应用不分区，则为0。  
		$providetype  = $data->providetype;   //发货类型: 0表示道具购买，1表示营销活动中的道具赠送，2表示交叉营销任务集市中的奖励发放。  		
		$payamt_coins = $data->payamt_coins;  				//扣取的游戏币总数，单位为Q点。  
		$pubacct_payamt_coins = $data->pubacct_payamt_coins;  //扣取的抵用券总金额，单位为Q点。
		  
		$appmeta      = $data->appmeta; 
		$clientver    = $data->clientver;
		*/  
		$sig          = $data->sig;

        foreach ($data as $key => $val ) 
        { 
			if ('sig' != $key)
				$param[$key] = $val;
        }    		
	}
	
	$url = $_SERVER["REQUEST_URI"];
	$appkey = PlatformUtil.QQ_APPKEY+"&";
	
	$SigCheck = new SnsSigCheck;
	if (!$SigCheck->verifySig($_SERVER['REQUEST_METHOD'], $url, $param, $appkey, $sig))
	{  
		param_check_fail($ret_json, 4, unicode_encode("请求参数错误").": (sig)");
		return;
	}
	
	$platorder = $token; //  ??????
	$orderid   = $billno;
	$amount    = $amt;
	
	$hash = PHP_BKDRHash($orderid);
	$index = $hash % 5 + 1;
	$msg = "billno=".$orderid." hashindex=".$index;
	writelog($msg, 1, $platname);
	
	$mysqli = new mysqli($dbhost, $dbuser, $dbpwd, $dbname);
	if (mysqli_connect_errno())
	{
		$msg = "Connect db failed!".mysqli_connect_error();
		writelog($msg, 4, $platname);
		param_check_fail($ret_json, 5, $msg);
		exit(); 
	}
	
	$sql_query = sprintf("select orderid, rmb, accountid, status, confirmtts from player_cash_%d where orderid='%s'", $index, $orderid);
	//echo $sql_query."\n";
	$res_query = $mysqli->query($sql_query);
	if (!$res_query)
	{
		$msg = "orderid=".$orderid.' no data';
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 4, unicode_encode("请求参数错误").": (billno)");
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
		fail_proc($mysqli, $ret_json, 4, unicode_encode("请求参数错误").": (billno)");
		return;
	}

	if ($amount != $row['rmb']) //  !==
	{
		$msg = "orderid=".$orderid."amount=".$amount." not equal to rmb=".$row['rmb'];
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 4, unicode_encode("请求参数错误").": (amt)");
		return;
	}
	
	//对同一个订单，如果先接收到支付失败，再接收到支付成功的通知，应以成功的支付结果为准，替换原接收到的失败的支付结果。
	// 一旦通知支付成功，不会再发生通知支付失败的情形。
	if (0 != $row['status'])
	{
		$msg  = "orderid=".$orderid;
		$msg .= " table status=".$row['status']." error";
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 0, "OK");
		return;
	}

	$sql = sprintf("update player_cash_%d set status=1, platformid='%s', platformorderid='%s', confirmtts='%s' where orderid='%s'", $index, $platid, $platorder, date('Y-m-d H:i:s'), $orderid)
	$affect = $mysqli->query($sql);
	if (!$affect)
	{
		$msg = "orderid=".$orderid.' update table failed';
		writelog($msg, 3, $platname);
		fail_proc($mysqli, $ret_json, 6, $msg);
		return;
	}

	fail_proc($mysqli, $ret_json, 0, "OK");
	//writelog("confirm ok", 1, $platname);
?>
