<?php
error_reporting(0); //抑制所有错误信息
@header("content-Type: text/html; charset=utf-8"); //语言强制
ob_start();

session_start();
?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<title>在线角色信息</title>
</head>

<body>
	<!-- 查询输入表单 -->
	<table border="0" align="left" width="220">
		<form method="post" action="">
		<tr>
			<td>查询时间: <input type="text" size=10 name="querytime" maxlength="10" value="<?php 
																								if (isset($_POST['querytime'])) 
																									echo $_POST['querytime'];
																								else if (isset($_GET['querytime']))
																									echo $_GET['querytime'];
																								else
																									echo date('Y-m-d');
																							?>" /></td>
			<td><input type="submit" name="query" value="查询" /></td>
		</tr>
		</form>
	</table>

<?php
	echo '<br/>';
	include("actorquery.php");
?>
</body>
</html>
