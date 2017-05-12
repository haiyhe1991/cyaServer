<?php
	/*平台ID定义(svn://192.168.1.201/SGSource/Designer/版本信息/版本信息.xls) */
	define("PLAT_APPSTORE", "1000");
	define("PLAT_XY",       "1001");
	define("PLAT_I4",       "1002");
	define("PLAT_PP",       "1003");	
	define("PLAT_HAIMA",    "1004");
	/*define("PLAT_91",       "1002");
	define("PLAT_360",      "1004");	
	define("PLAT_BAIDU",    "1005");
	define("PLAT_MI",       "1006");*/
	define("PLAT_UCJY",     "1007");
	define("PLAT_TENCENT", 	"1010"); // 应用宝
	
	define("YUC_LOG_TYPE", "1,2,3,4,5,6"); 			//日志级别 1:Debug 2:Info/warn 3:Error 4:critical Error
	define("M_PRO_DIR", "/home/wwwroot/default"); 	//日志目录

	function writelog($msg, $level, $folder)
	{
		$arr_level = explode(',', YUC_LOG_TYPE);
		if (in_array($level, $arr_level))
		{
			$record = date('Y-m-d H:i:s') . " >>> " . number_format(microtime(TRUE), 5, ".", "") . ' ' . " : " . $level . "\t" . $msg;
			$base = M_PRO_DIR . "/log/" . $folder;
			$dest = $base . "/" . date("YmdH", time()) . 'log.php';
			if (!file_exists($dest))
			{
				@mkdir($base, 0777, TRUE);
				@file_put_contents($dest, "<?php die('Access Defined!');?>\r\n", FILE_APPEND);
			}
			if (file_exists($dest))
			{
				@file_put_contents($dest, $record .PHP_EOL, FILE_APPEND);
			}
		}
	}

	/*function mylog( $logthis )
	{
		file_put_contents('/home/www/logfile.log', date("Y-m-d H:i:s"). " " . $logthis. "\r\n", FILE_APPEND | LOCK_EX);
	}*/
	
	function request_uri() 
	{ 
		if (isset($_SERVER['REQUEST_URI'])) 
		{ 
			$uri = $_SERVER['REQUEST_URI']; 
			//echo "1 </br>";
		} 
		else 
		{ 
			if (isset($_SERVER['argv'])) 
			{ 
				$uri = $_SERVER['PHP_SELF'] .'?'. $_SERVER['argv'][0];
				//echo "2 </br>";				
			} 
			else 
			{ 
				$uri = $_SERVER['PHP_SELF'] .'?'. $_SERVER['QUERY_STRING'];
				//echo "3 </br>";				
			} 
		} 
		return $uri; 
	}
	
	function GetDateInterval($date, $num)
	{
		$start = 0;
		$year  = '';
		$month = '';
		$day   = '';
		if(!empty($date))
		{
			$len = strlen($date);
			if ($len == 8)
			{
				$start = date("Y-m-d", strtotime($date));		
			}
			else if ($len == 10)
			{
				$start = $date;	
			}
			else
			{
				$start = date("Y-m-d");		
			}
		}
		else
		{
			$start = date("Y-m-d");	
		}
		
		$days = array();
		$days[0] = $start;
		for ($i = 1; $i < $num; $i++)
		{
			$days[$i] = date("Y-m-d", strtotime("$start + $i days"));
		}
		
		return $days;
	} 	
	
	function PHP_BKDRHash($str)
	{
		$seed = 131; /* 31 131 1313 13131 131313 etc.. */
		$hash = 0;

		$n = strlen($str);	
		//echo $n."\n";		
		for ($i = 0; $i <$n; $i++)
		{
			$hash = $hash * $seed + ord($str[$i]);
			$hash &= 0xffffff;
			//echo $hash."\n";
		}
		
		return $hash;
	}
	
	//将string中的“-”编码为%2D
	/*function strip_convert($string)  
	{  
		$len = strlen($string);  
		$str = '';  
		for ($i = 0; $i < $len; $i++)  
		{  
			$ch = $string[$i];  
			if ('-' == $ch)  
			{    // “-”编码为%2D
				$str .= '%2D';  
			}  
			else  
			{  
				$str .= $ch;  
			}  
		}  
		return $str;  
	}*/  
	
	//将内容进行UNICODE编码，编码后的内容格式：\u56fe\u7247
	function unicode_encode($name)  
	{  
		$name = iconv('UTF-8', 'UCS-2', $name);  
		$len = strlen($name);  
		$str = '';  
		for ($i = 0; $i < $len - 1; $i = $i + 2)  
		{  
			$c = $name[$i];  
			$c2 = $name[$i + 1];  
			if (ord($c) > 0)  
			{    // 两个字节的文字  
				$str .= '\u'.base_convert(ord($c), 10, 16).base_convert(ord($c2), 10, 16);  
			}  
			else  
			{  
				$str .= $c2;  
			}  
		}  
		return $str;  
	}  
	  
	// 将UNICODE编码后的内容进行解码  
	function unicode_decode($name)  
	{  
		// 转换编码，将Unicode编码转换成可以浏览的utf-8编码  
		$pattern = '/([\w]+)|(\\\u([\w]{4}))/i';  
		preg_match_all($pattern, $name, $matches);  
		if (!empty($matches))  
		{  
			$name = '';  
			for ($j = 0; $j < count($matches[0]); $j++)  
			{  
				$str = $matches[0][$j];  
				if (strpos($str, '\\u') === 0)  
				{  
					$code = base_convert(substr($str, 2, 2), 16, 10);  
					$code2 = base_convert(substr($str, 4), 16, 10);  
					$c = chr($code).chr($code2);  
					$c = iconv('UCS-2', 'UTF-8', $c);  
					$name .= $c;  
				}  
				else  
				{  
					$name .= $str;  
				}  
			}  
		}  
		return $name;  
	}  	
	
	/**
	 * $str 原始中文字符串
	 * $encoding 原始字符串的编码，默认UTF-8
	 * $prefix 编码后的前缀，默认"\u"
	 * $postfix 编码后的后缀，默认";"
	 */
	function unicode_enc($str, $encoding = 'UTF-8', $prefix = '\u'/*, $postfix = ';'*/) {
		$str = iconv($encoding, 'UCS-2', $str);
		$arrstr = str_split($str, 2);
		$unistr = '';
		for($i = 0, $len = count($arrstr); $i < $len; $i++)
		{
			//$dec = hexdec(bin2hex($arrstr[$i])); //十进制值
			
			$dec = bin2hex($arrstr[$i]);	//十六进制值
			$unistr .= $prefix . $dec/* . $postfix*/;
		} 
		return $unistr;
	} 
	 
	/**
	 * $str Unicode编码后的字符串
	 * $decoding 原始字符串的编码，默认UTF-8
	 * $prefix 编码字符串的前缀，默认"\u"
	 * $postfix 编码字符串的后缀，默认";"
	 */
	function unicode_dec($unistr, $decoding = 'UTF-8', $prefix = '\u'/*, $postfix = ';'*/) {
		$arruni = explode($prefix, $unistr);
		$unistr = '';
		for($i = 1, $len = count($arruni); $i < $len; $i++)
		{
			/*if (strlen($postfix) > 0) {
				$arruni[$i] = substr($arruni[$i], 0, strlen($arruni[$i]) - strlen($postfix));
			}*/ 
			//$temp = intval($arruni[$i]);
			
			$temp = intval($arruni[$i], 16); //参数为十六进制值
			//$temp = intval(hexdec($arruni[$i]));
			$unistr .= ($temp < 256) ? chr(0) . chr($temp) : chr($temp / 256) . chr($temp % 256);
		} 
		return iconv('UCS-2', $decoding, $unistr);
	}	
	
	// 传递给函数unicodeDecode的参数$name的内容中一定不能包含单引号，否则就会导致解析失败, 所以有必要的话可以借助 str_replace()函数将非法字符格式化为合格字符
	function unicodeDecode($name)
	{
		$json = '{"str":"'.$name.'"}';
		$arr = json_decode($json,true);
		if(empty($arr)) return ''; 
		return $arr['str'];
	}
	
	function is_utf8($word)    
	{    
		if ( preg_match("/^([".chr(228)."-".chr(233)."]{1}[".chr(128)."-".chr(191)."]{1}[".chr(128)."-".chr(191)."]{1}){1}/", $word) == true
		  || preg_match("/([".chr(228)."-".chr(233)."]{1}[".chr(128)."-".chr(191)."]{1}[".chr(128)."-".chr(191)."]{1}){1}$/", $word) == true 
		  || preg_match("/([".chr(228)."-".chr(233)."]{1}[".chr(128)."-".chr(191)."]{1}[".chr(128)."-".chr(191)."]{1}){2,}/", $word) == true )    
		{    
			return true;    
		}    
		else    
		{    
			return false;    
		}      
	}  
	
	// 多种编码格式转换为UTF-8格式
	function ConvertToUTF8($data, $to = 'UTF-8') 
	{
		$encode_array = array('ASCII', 'UTF-8', 'GBK', 'GB2312', 'BIG5', 'CP936');
		$encode_type = mb_detect_encoding($data, $encode_array);
		$to = strtoupper($to);
		if ($encode_type != $to)
		{
			$data = mb_convert_encoding($data, $to, $encode_type);
		}
		return $data;
	}
	
	function fail_proc($db, $isjson, $errcode, $err, $fail = 'fail', $ok = 'success')
	{
		if (true === $isjson)
		{
			header('Content-type: text/json');
			echo json_encode(array("ret" => $errcode, "msg" => $err));	
		}
		else
		{
			//echo $err;
			if (0 === $errcode)
				echo $ok;
			else
				echo $fail;
		}
		
		$db->close();
		return ;
	}
	
	function param_check_fail($isjson, $errcode, $err, $fail = 'fail', $ok = 'success')
	{
		if (true === $isjson)
		{
			header('Content-type: text/json');
			echo json_encode(array("ret" => $errcode, "msg" => $err));	
		}
		else
		{
			//echo $err;
			if (0 === $errcode)
				echo $ok;
			else
				echo $fail;
		}
		
		return ;
	}
?>
