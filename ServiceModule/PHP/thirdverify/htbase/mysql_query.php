<?php
function GetAccountID($db, $userid, &$username)
{
	// 查询用户信息
	for ($i = 1; $i < 6; $i++)
	{
		$sql_user = sprintf("select user from player_account_%d where id='%s'", $i, $userid);
		$res_query = $db->query($sql_user);
		if ($res_query)
		{
			$rows    = $res_query->num_rows;
			$columns = $res_query->field_count;
			if ((1 === $rows) && (1 === $columns))
			{
				// 获取一条记录
				$user     = $res_query->fetch_assoc();
				$username = $user['user'];
				
				$res_query->close();
				return;
			}
			$res_query->close();
		}
	}
}

function GetCash($db, $userid, &$totalcash, &$buycash)
{
	for ($no = 1; $no < 6; $no++)
	{
		$sql_cash = sprintf("select cashcount, cash from cash_%d where accountid='%d'", $no, $userid);
		$res_cash = $db->query($sql_cash);
		if ($res_cash)
		{
			$rows    = $res_cash->num_rows;
			$columns = $res_cash->field_count;
			if ((1 === $rows) && (2 === $columns))
			{
				$cash = $res_cash->fetch_assoc();
				$buycash   = $cash['cashcount'];
				$totalcash = $cash['cash'];
				//echo 'accountid='.$userid.' buycash='.$buycash.' totalcash='.$totalcash.'</br>';
				
				$res_cash->close();
				return;
			}
			$res_cash->close();
		}				
	}
}
?>
