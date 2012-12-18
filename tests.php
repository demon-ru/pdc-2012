<?php

require_once(dirname(__FILE__).'/test-lib.inc');

define('testUrl', 'http://test.talk37.ru/pdc-2012/');
define('prmContent', 'content');

$tests = array(
	'newgame1' => array('url'=>testUrl."\${gameName}/games", 'method'=>'POST', 'code'=>200, 'content2macro'=>'gameID'),
	'move11' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>9), 'code'=>200, 'content'=>'accepted'),
	'move12' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>3), 'code'=>200, 'content'=>'accepted'),
	'move13' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>1), 'code'=>200, 'content'=>'accepted'),
	'move14' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>1), 'code'=>200, 'content'=>'rejected'),
	'move15' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>4), 'code'=>200, 'content'=>'accepted'),
	'move16' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>5), 'code'=>200, 'content'=>'x won'),
	'move17' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>5), 'code'=>200, 'content'=>'game finished'),

	'list1' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'GET', 'code'=>200, 'content'=>'9,3,1,4,5:x won'),

	'newgame2' => array('url'=>testUrl."\${gameName}/games", 'method'=>'POST', 'code'=>200, 'content2macro'=>'gameID'),
	'move21' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>1), 'code'=>200, 'content'=>'accepted'),
	'move22' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>9), 'code'=>200, 'content'=>'accepted'),
	'move23' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>4), 'code'=>200, 'content'=>'accepted'),
	'move24' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>8), 'code'=>200, 'content'=>'accepted'),
	'move25' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>7), 'code'=>200, 'content'=>'accepted'),
	'move26' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>5), 'code'=>200, 'content'=>'accepted'),
	'move27' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>6), 'code'=>200, 'content'=>'accepted'),
	'move28' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'POST','POST'=>array(prmContent=>3), 'code'=>200, 'content'=>'no winner'),

//	'list2' => array('url'=>testUrl."\${gameName}/games/\${gameID}", 'method'=>'GET', 'code'=>200, 'content'=>'9,3,1,4,5:x won'),

);

$macros = array('gameName'=>'tictactoe');

foreach($tests as $name => $data)
{
	$post = array_key_exists('POST', $data) ? $data['POST'] : NULL;
	urlTester::getContent($data['url'], $data['method'], $post, $macros, $code, $content);
	$code_requested = $data['code'];
	print($name."... ");
	if ($code_requested != $code)
		print("Error! Code request: ".$code_requested." returned: ".$code.lineBreak);
	elseif (array_key_exists('content',$data) && (($content_requested = $data['content']) != $content))
	{
		print("Error! Content request: ".$content_requested." returned: ".$content.lineBreak);
	} else
	{
		print("OK".lineBreak);
		if (array_key_exists('content2macro',$data))
			$macros[$data['content2macro']] = $content;
		//print_r($macros);
		//print("content:".$content);
	}
}
