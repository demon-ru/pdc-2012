<?php

/**
 * Класс, реализующий хранилище через PEER:MDB2 - интерфейс работы с многими СУБД
 * Реализует интерфейсы серилизации как одной игры, так и менеджера игр.
 */

class GameDataStorage_MDB2 implements IGameDataStorage, IGameManagerDataStorage
{
	private $mdb2;

	private function checkError(&$res, $message)
	{
		if (PEAR::isError($res)) {
    		throw new GameMoveException($message.": ".$res->getMessage());
		}
	}
	
	function __construct($dsn, $options = false)
	{
		if (!@include_once('MDB2.php'))
    		throw new GameMoveException('DB initialization error: no pear/MDB2 installed.');
		$this->mdb2 =& MDB2::connect($dsn, $options);
		$this->checkError($this->mdb2, "DB initialization error");
		$this->mdb2->setFetchMode(MDB2_FETCHMODE_ASSOC);
	}
	
	function disconnect()
	{
		$this->mdb2->disconnect();
		$this->mdb2 = NULL;
	}
	
	function getGameDataStorage()
	{
		return $this;
	}

	private function getGameTableName($name)
	{
		return "game_".$name;
	}
	
	function getNewGameID($name)
	{
		/*
		$sql = "insert into ".$this->getGameTableName($name)." (created) values (:created)";
		$sth = $this->mdb2->prepare($sql, array('timestamp'));
		$this->checkError($affected, 'DB saveGameData error');
		$affected = $sth->execute(array(time()));
		*/
		$sql = "insert into ".$this->getGameTableName($name)." (created) values (now())";
		$affected =& $this->mdb2->exec($sql);
		$this->checkError($affected, 'DB getNewGameID error');
		return $this->mdb2->lastInsertID();
	}
	
	function getGamesData($name)
	{
		$res =& $this->mdb2->query("SELECT * FROM ".$this->getGameTableName($name)." order by created");
		$this->checkError($res, 'DB getGamesStatuses error');
		$rows = array();
		while ($row = $res->fetchRow())
		    $rows[$row['id']] = $row;
		$res->free();
		return $rows;
	}

	function loadGameData($name, $id)
	{
		$res =& $this->mdb2->query("SELECT * FROM ".$this->getGameTableName($name)." where ID=".$this->mdb2->escape($id));
		$this->checkError($res, 'DB loadGameData error');
		$row = $res->fetchRow();
		$res->free();
		return $row;
	}
	
	function saveGameData($name, $id, $data)
	{
		$values = array();
		foreach($data as $field => $value)
			$values[] = $field."='".$this->mdb2->escape($value)."'";
		$sql = "update ".$this->getGameTableName($name)." set ".implode(",", $values)." where ID=".$this->mdb2->escape($id);
		$affected = $this->mdb2->exec($sql);
		$this->checkError($affected, 'DB saveGameData error');
	}
	
}

