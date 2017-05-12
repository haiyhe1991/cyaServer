<?php
header("Content-Type: text/html; charset=UTF-8"); 
/*
	爱思充值平台：http://pay.i4.cn/
	以下为示例代码
*/
include('Rsa.php');
include('MyRsa.php');

DecodeController::AppReturn();

class DecodeController
{
	public function AppReturn()
	{
		$notify_data = $_REQUEST;//$_POST;
		$chkres = self::chk($notify_data);
		//error_log(date("Y-m-d h:i:s")."result ".serialize($chkres)."\r\n",3,'rsa.log');
		if($chkres)
		{
			//验证通过
			//echo "success";
			return true; 
		}
		else
		{
			//echo "fail";
			return false;
		}

	}
	
	//验证
	public function chk($notify_data)
	{
		$privatedata = $notify_data['sign'];
		//error_log(date("Y-m-d h:i:s").": ".serialize($privatedata)."\r\n",3,'rsa.log');
		
		//$privatedata = urldecode($notify_data['sign']);
		$privatebackdata = base64_decode($privatedata);
		//error_log(date("Y-m-d h:i:s")."base64_decode: ".serialize($privatebackdata)."\r\n",3,'rsa.log');
		
		$MyRsa = new MyRsa;
		// MyRsa.public_key 替换成自己的公钥（登录我们后台，点击顶部导航栏的开发者中心提取）
		//解密出来的数据
		$data = $MyRsa->rsa_decrypt($privatebackdata, MyRsa::public_key);
		//error_log(date("Y-m-d h:i:s")."publickey_decodeing: ".$data."\r\n",3,'rsa.log');
		//echo $data;
		
		//$rs = json_decode($data,true);
		//error_log(date("Y-m-d h:i:s")."rs ".serialize($rs)."\r\n",3,'rsa.log');
		//if(empty($rs)||empty($notify_data))return false;
		//将解密出来的数据转换成数组
		foreach(explode('&', $data) as $val)
		{
			$arr = explode('=', $val);
			$dataArr[$arr[0]] = $arr[1];
		}
		//error_log(date("Y-m-d h:i:s")."dataArr\t\t".var_export($dataArr, true)."\r\n",3,'rsa.log');
		
		//验证
		if($dataArr["billno"]==$notify_data['billno'] && $dataArr["amount"]==$notify_data['amount'] && $dataArr["status"]==$notify_data['status']) {
			return true; 
		}else{
			return false;
		}
	}
}
?>
 