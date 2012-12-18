<?php

require_once(dirname(__FILE__).'/games-core.inc');

define('gameParamName', 'name');
define('gameParamID', 'id');
define('gameParamMove', 'content');

/**
 * Класс Контроллер. 
 * Содержит методы для реализации всех действий, вызываемых "пользователем"
 * Единственная точка входа: GameDispatcher::Dispatch();
 */

class GameDispatcher
{
	public static function printLine($line)
	{
		print($line."\n");
	}

	private static function printGamePositions(&$game)
	{
		self::printLine("");
		$data = $game->getGamePositions();
		foreach($data as $row)
			self::printLine($row);
	}

	private static function gameNew($gameName)
	{
		$game = GameManager::newGame($gameName);
		self::printLine($game->getID());
		self::printGamePositions($game);
	}
	
	private static function gameMakeMove($gameName, $gameID, $move)
	{
		$game = GameManager::getGame($gameName, $gameID);
		$status = $game->makeMove($move);
		self::printLine($status);
		self::printGamePositions($game);
	}
	
	private static function gamePrintMoves($gameName, $gameID)
	{
		$game = GameManager::getGame($gameName, $gameID);
		self::printLine($game->listOfMoves());
	}

	private static function gamePrintGameList($gameName)
	{
		$result = GameManager::gamePrintGameList($gameName);
		self::printLine($result);
	}

	private static function DispatchGET($gameName, $gameID)
	{
		if ($gameID === FALSE)
			self::gamePrintGameList($gameName);
		else
		 	self::gamePrintMoves($gameName, $gameID);
	}

	private static function DispatchPOST($gameName, $gameID)
	{
		if ($gameID === FALSE)
			self::gameNew($gameName);
		else
		{
		 	if (!array_key_exists(gameParamMove,$_POST))
				throw new GameMoveException(responce_rejected." (Value of '".gameParamMove."' not found)");
			$move = $_POST[gameParamMove];
			self::gameMakeMove($gameName, $gameID, $move);
		}
	}

	public static function Dispatch()
	{
		$method = $_SERVER['REQUEST_METHOD'];
		$gameName = array_key_exists(gameParamName,$_GET) ? $_GET[gameParamName] : FALSE;
		$gameID = array_key_exists(gameParamID,$_GET) ? $_GET[gameParamID] : FALSE;

		//print("Dispatch: name:".$gameName." method:".$method." ID:".$gameID);

 		if ($gameName === FALSE)
			throw new GameMoveException(responce_rejected." (Value of '".gameParamName."' not found)");

 		if ($gameID !== FALSE) 
			GameManager::checkGameID($gameName, $gameID);

		switch($method)
		{
			case 'GET': self::DispatchGET($gameName, $gameID); break;
			case 'POST': self::DispatchPOST($gameName, $gameID); break;
		} 
		//print($status);
	}
}
