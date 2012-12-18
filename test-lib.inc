<?php

define('lineBreak','<br/>');

class urlTester
{
	private static function translateMacro($value, &$macro)
	{
		if (isset($macro))
			foreach($macro as $mkey => $mvalue)
				$value = str_replace("\${".$mkey."}", $mvalue, $value);
		return $value;
	}

	private static function getParamsAsString(&$params, &$macro)
	{
		return self::translateMacro(http_build_query($params), $macro);
	}
	
	public static function getContent($url, $method, $post, $macro, &$code, &$content)
	{
		$url = self::translateMacro($url, $macro);
		//print(lineBreak.$url.lineBreak);
		$post_str = (isset($post)) ? self::getParamsAsString($post, $macro) : "";
		
  	 	$curl = curl_init();
   		curl_setopt($curl, CURLOPT_URL, $url);
		//включаем вывод заголовков
		//curl_setopt($curl, CURLOPT_HEADER, 1);

   		if ($post_str != "")
		{
			curl_setopt($curl, CURLOPT_POST, count($post));
   			curl_setopt($curl, CURLOPT_POSTFIELDS, $post_str);
		} elseif ($method == 'POST')
		{
			curl_setopt($curl, CURLOPT_POST, 1);
   			curl_setopt($curl, CURLOPT_POSTFIELDS, "nothing=1");
		}
   		curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);

 		$content = curl_exec($curl);

		//проверяем, если ошибка, то получаем номер и сообщение
		if($content !== FALSE)
		{
			$code = 200;
			if (($pos = strpos($content,"\n")) !== FALSE)
			    $content = substr($content, 0, $pos);
		}
		else
		{
			$code = curl_errno($curl);
			$content = curl_error($curl).'('.$code.')';
		}
   		curl_close($curl);
	}
}
