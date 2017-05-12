<?php
require_once("../htbase/mysql_conf.php");
require_once("../htbase/func.php");
require_once("../htbase/mysql_query.php");

session_start();
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title></title>
<style type="text/css">
	<!-- body,td,th {font-size: 12px;} -->
	h2{text-align="center"; font-size:80%}
	td{text-align="left"; font-size:12px}
</style>
</head>

<body>
<p>
<?php
	$page       = 0;
	$page_count = 50;
	$logdir     = 'todaylogin';	 
	$querytime  = '';
	
	if (isset($_POST['querytime']))
	{
		$daysets   = GetDateInterval($_POST['querytime'], 1);
		$querytime = $daysets[0];
		$page = 1;
		//echo 'page2='.$page.' querytime='.$querytime.' posttime='.$_POST['querytime'].'<br/>';
	}	
	else if(isset($_GET['querytime']))
	{
		$daysets   = GetDateInterval($_GET['querytime'], 1);
		$querytime = $daysets[0];
		
		if(isset($_GET['page']))
		{
			$page=$_GET['page'];
		}	
		else
		{
			$page=1;
		}
		//echo 'page1='.$page.' querytime='.$querytime.'<br/>';
	}
	else
	{
		$querytime=date("Y-m-d");
		$page = 1;
		//echo 'page3='.$page.' querytime='.$querytime.'<br/>';
	}	
	
	$sgs = new mysqli($dbhost, $dbuser, $dbpwd, $dbname_sgs);
	if (mysqli_connect_errno())
	{
		$msg = "Connect sgs failed!".mysqli_connect_error();
		writelog($msg, 4, $logdir);
		return;
	}
	
	$page_page = 1;
	$offect    = 0;   	// 获取上一页的最后一条记录，从而计算下一页的起始记录	
	$currnum   = 0;		// 存储的记录数
	$actorcn   = 0;		// 查得的记录数
	$actorinfo = array();

	$sql_sgs = sprintf("select id, accountid, createtts, nick, actortype, level, exp, gold, online, lvtts from actor where date(lvtts)='%s' order by lvtts asc", $querytime);
	$res_actor = $sgs->query($sql_sgs);
	if ($res_actor)
	{
		// 获取记录数并计算页面数和显示数据的偏移
		$actorcn   = $res_actor->num_rows;
		$page_page = ceil($actorcn/$page_count);
		$offect    = ($page-1)*$page_count;	
		
		$index = 0;
		while ($rowObj = $res_actor->fetch_object())
		{
			if ($offect > $index)
			{
				$index++;
				continue;
			}
			else
			{
				//echo 'offect='.$offect.' index='.$index.' currnum='.$currnum.'<br/>';
				$actorinfo[$currnum] = $rowObj;
				$currnum++;
				if ($page_count <= $currnum)
				{
					break;
				}
				$index++;					
			}
		}
		$res_actor->close();
	}
	if (0 == $actorcn)
	{
		$msg = 'date['.$querytime.']'.' no actor information!';
		writelog($msg, 2, $logdir);
		
		// 关闭所有数据库连接
		$sgs->close();
		return;
	}	
		
	//echo 'offect='.$offect.' totalrows='.$actorcn.'<br/>';
	echo "<table width=1100 height=20 border=1 align=left bordercolor=#FFCC99>";
	echo "<caption><h2>在线角色信息</h2></caption>";
	echo "<tr>";
	echo "<td width=45 height=10>角色名</td><td width=25 height=10>用户名</td><td width=45 height=10>创建时间</td><td width=10 height=10>职业</td>";
	echo "<td width=10 height=10>等级</td><td width=15 height=10>经验</td><td width=15 height=10>金币</td>";
	echo "<td width=15 height=10>充值代币</td><td width=15 height=10>总代币</td><td width=15 height=10>在线时间</td><td width=45 height=10>最后离开时间</td>";
	echo "</tr>";	
	
	$db_verify = true;
	$db_cash   = true;
	$sgs_verify = new mysqli($dbhost, $dbuser, $dbpwd, $dbname_verify);
	if (mysqli_connect_errno())
	{
		$msg = "Connect sgsverify failed!".mysqli_connect_error();
		writelog($msg, 4, $logdir);
		$db_verify = false;
		
		// 关闭数据库连接
		//$sgs->close();
		//return;
	}
	$sgs_cash = new mysqli($dbhost, $dbuser, $dbpwd, $dbname_cash);
	if (mysqli_connect_errno())
	{
		$msg = "Connect sgscash failed!".mysqli_connect_error();
		writelog($msg, 4, $logdir);
		$db_cash = false;
		
		// 关闭数据库连接
		//$sgs->close();				
		//$sgs_verify->close();
		//return;
	}
	
	// 填充每个角色的信息
	for ($loop = 0; $loop < $currnum; $loop++)
	{
		$username  = '';
		$totalcash = 0;
		$buycash   = 0;
		
		$obj = $actorinfo[$loop];
		if ($db_verify)
			GetAccountID($sgs_verify, $obj->accountid, $username);
		if ($db_cash)
			GetCash($sgs_cash, $obj->accountid, $totalcash, $buycash);
		
		echo "<tr>";
		echo "<td width=45 height=10>".$obj->nick."</td><td width=25 height=10>".$username."</td><td width=45 height=10>".$obj->createtts."</td><td width=10 height=10>".$obj->actortype."</td>";
		echo "<td width=10 height=10>".$obj->level."</td><td width=15 height=10>".$obj->exp."</td><td width=15 height=10>".$obj->gold."</td>";
		echo "<td width=15 height=10>".$buycash."</td><td width=15 height=10>".$totalcash."</td><td width=15 height=10>".$obj->online."</td><td width=45 height=10>".$obj->lvtts."</td>";
		echo "</tr>";
		
		// 写表项数据日志
		//$msg = '[nick]'.$obj->nick.' [username]'.$username.' [accountid]'.$obj->accountid.' [createtts]'.$obj->createtts.' [actortype]'.$obj->actortype.' [level]'.$obj->level.' [exp]'.$obj->exp.' [gold]'.$obj->gold.' [online]'.$obj->online.' [lvtts]'.$obj->lvtts.' [buycash]'.$buycash.' [totalcash]'.$totalcash;
		//writelog($msg, 2, $logdir);
		//echo $msg.'<br/>';
	}
	echo "</table>";

	// 关闭所有数据库连接
	$sgs->close();
	if ($db_verify)
		$sgs_verify->close();
	if ($db_cash)
		$sgs_cash->close();	
?>

<table width=1100 height=20 border=1 align=left bordercolor=#FFCC99>
  <tr>
    <td colspan="3"><div align="center">
      <div align="right">共<?php echo $page_page;?>页 每页<?php echo $page_count;?>条 当前第<?php  echo $page; ?>页  
	  <a href="actorinfo.php?page=1&querytime=<?php if (!empty($querytime)){echo $querytime;}else{echo date('Ymd');}?>">首页</a>
	  <a href="actorinfo.php?page=<?php if($page==1){echo $page=1; }else{ echo $page-1; }?>&querytime=<?php if (!empty($querytime)){echo $querytime;}else{echo date('Ymd');}?>">上一页</a> 
	  <a href="actorinfo.php?page=<?php if($page<$page_page){echo $page+1;}else{ echo $page_page;}?>&querytime=<?php if (!empty($querytime)){echo $querytime;}else{echo date('Ymd');}?>">下一页</a> 
    <a href="actorinfo.php?page=<?php echo $page_page; ?>&querytime=<?php if (!empty($querytime)){echo $querytime;}else{echo date('Ymd');}?>">尾页</a></div></td>
  </tr>
</table>

</body>
</html>
