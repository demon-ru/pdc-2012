<?php

define('responce_accepted', 'accepted');
define('responce_rejected', 'rejected');
define('responce_invalid_game','invalid game');
define('responce_finished', 'game finished');

define('status_active', 'active');
define('status_no_winner', 'no winner');

class GameMoveException extends InvalidArgumentException { }

/**
 * Интерфейс для работы с хранилищем/БД (Protected Variations)
 * Содержит методы для серилизации одной игры
 */
interface IGameDataStorage
{
	function loadGameData($name, $id);
	function saveGameData($name, $id, $data);
}

/**
 * Интерфейс для работы с хранилищем/БД (Protected Variations)
 * Содержит методы для менеджера игр
 */
interface IGameManagerDataStorage
{
	/**
 	* Запросить идентификатор для новой игры с именем $name
	* @param string $name имя игры
	* @return int идентификатор для игры
 	*/
	function getNewGameID($name);
	/**
 	* Запросить интерфейс хранилища для одиночной игры
	* @return IGameDataStorage интерфейс к хранилищу
 	*/
	function getGameDataStorage();
	/**
 	* Запросить список данных по всем играм с заданным именем
	* @param string $name имя игры
	* @return array индексированный по идентификатору массив массивов с данными игр. Которые можно загрузить через IGameDataStorage.loadGameData
 	*/
	function getGamesData($name);
	/**
 	* Отключиться от хранилища. Завершить работу с ним.
 	*/
	function disconnect();
}

/**
 * Интерфейс для игр, которые могут работать с хранилищем
 */
interface IGameMoveSerializable
{
	/**
 	* Установить хранилище, переданное в параметрах, текущим.
 	* Загрузить данные по игре из этого хранилища
	* 
	* @param IGameDataStorage $Storage передаваемое хранилище
 	*/
	function setAndLoadFromStorage(IGameDataStorage $Storage);
}

/**
 * Абстрактная игра, которая состоит в том, чтобы делать ходы.
 * Игра имеет идентификатор, текущее состояние, выполненные ходы.
 * @abstract
 */
abstract class GameMoveAbstract
{
	/**
 	* Идентификатор игры
	* @var int $id
	* @access private
 	*/
	private $id;
	/**
 	* Текущий статус игры
	* @var string $status
	* @access protected
 	*/
	protected $status = status_active;
	/**
 	* Запомненные ходы игры. Целочисленные номера клеток как элементы массива
	* @var array $moves
	* @access protected
 	*/
	protected $moves = array();

	/**
 	* Запросить имя игры
	* @return string имя игры
	* @abstract
	* @access public
	* @static
 	*/
	abstract public static function getGameName();
	/**
 	* Вычислить новый текущий статус игры и вернуть его
	* @return string новый статус игры
	* @abstract
	* @access protected
 	*/
	abstract protected function checkStatus();
	/**
 	* Сформировать и вернуть наглядное пользовательское представление состояния игры. Для вывода.
	* @return array Массив строк, которые наглядно отображают состояние игры, занятых клеток.
	* @abstract
	* @access public
 	*/
	abstract public function getGamePositions();

	public function __construct($id)
	{
		$this->id = $id;
	}

	public function getID()
	{
		return $this->id;
	}

	protected function gameDataChanged() { }
	
	protected function updateStatus()
	{
		$this->status = $this->checkStatus();
		return (($this->status != status_active) ? $this->status : responce_accepted);
	}

	protected function addMove($move)
	{
		$this->moves[] = (int)$move;
	}

	protected function checkMove($move)
	{
		if ($this->status != status_active)
			throw new GameMoveException(responce_finished);
		if (in_array((int)$move, $this->moves))
			throw new GameMoveException(responce_rejected);
	}

	public function makeMove($move)
	{
		$this->checkMove($move);
		$this->addMove($move);
		$status = $this->updateStatus();
		$this->gameDataChanged();
		return $status;
	}
	
	public function listOfMoves()
	{
		return implode(",", $this->moves).":".$this->status;
	}
	
	public static function Register()
	{
		GameManager::registerGameClass(get_called_class());
	}
	
}

/**
 * Абстрактная игра, которая имеет работать с хранилищем: загружать и сохранять своё состояние,
 * реагировать на изменение игры
 * @abstract
 */
abstract class GameMoveSerializable extends GameMoveAbstract implements IGameMoveSerializable 
{
	// Хранилище для чтения и сохранения данных игры
	private $Storage;

	public function setAndLoadFromStorage(IGameDataStorage $Storage)
	{
		$this->Storage = $Storage;
		$data = $Storage->loadGameData(static::getGameName(), $this->getID());
		if ($data === FALSE)
			throw new GameMoveException(responce_invalid_game);
		$this->loadGameData($data);
	}

	protected function loadGameData($data)
	{
		$this->status = $data['status'];
		if (($moves = $data['moves']) != "")
		    $this->moves = explode(",", $moves);
	}

	protected function saveGameData(&$data)
	{
		$data['status'] = $this->status;
		$data['moves'] = implode(",", $this->moves);
	}
	
	protected function gameDataChanged()
	{
		$data = array();
		$this->saveGameData($data);
		$this->Storage->saveGameData(static::getGameName(), $this->getID(), $data);
	}
	
}

/**
 * Класс, реализующий интерфейс хранилища данных игры.
 * Используется для быстрой массовой загрузки одноимённых игр.
 * Суть ускорения в том, что реальное хранилище одним запросом (IGameManagerDataStorage.getGamesData) выдаёт данные 
 * по всем играм. Объект данного класса, используя эти данные, представляется хранилищем игр и выдаёт данные для каждой игры
 * без отдельных запросов к реальному хранилищу (не делая отдельных SQL запросов для каждой игры)
 */
class GameDataReadOnlyMassStorage implements IGameDataStorage
{
	private $games_data;
	
	function __construct(&$data)
	{
		$this->games_data = $data;
	}
	
	function loadGameData($name, $id)
	{
		if (array_key_exists($id, $this->games_data))
			return $this->games_data[$id];
		else
			throw new GameMoveException("GameID is incorrect");
	}
	function saveGameData($name, $id, $data)
	{
		throw new GameMoveException("Read only mode");
	}
}

/**
 * Singleton класс (класс с только статическими методами) - менеджер игр.
 * Класс ответственен за работу с типами игр (их загрузку), загрузку игр, создание новых игр, формирование списка игр по имени
 * При этом он пользуется хранилищем.
 * TODO: не слишком ли он многое умеет? Возможно, стоит выделить функции загрузки типов игр и регистраций в отдельный класс
 * Abstract Factory
 */
class GameManager
{
	private static $GameClasses = array();
	private static $Storage;

	public static function SetStorage(IGameManagerDataStorage $Storage)
	{
		self::ClearStorage();
		self::$Storage = $Storage;
	}

	public static function ClearStorage()
	{
		if (isset(self::$Storage))
			self::$Storage->disconnect();
		self::$Storage = NULL;
	}

	private static function getStorage()
	{
		if (!isset(self::$Storage))
			throw new GameMoveException(responce_rejected." (Storage not initialized)");
		return self::$Storage;
	}
	
	private static function getGameDataStorage()
	{
		return self::getStorage()->getGameDataStorage();
	}

	private static function getFileNameByGameName($name)
	{
		$name = escapeshellcmd(str_replace('..','',$name));
		return dirname(__FILE__).'/games/'.$name.'.inc';
	}

	public static function registerGameClass($class)
	{
		self::$GameClasses[$class::getGameName()] = $class;
	}

	private static function getClassNameByGameName($name)
	{
		return self::$GameClasses[$name];
	}

	private static function loadGameByName($name)
	{
		$file_name = self::getFileNameByGameName($name);
		if (!file_exists($file_name))
			throw new GameMoveException(responce_invalid_game);
		require_once($file_name);
		return self::getClassNameByGameName($name);
	}

	private static function CreateGameObject($class_name, $id)
	{
		self::checkGameID($class_name::getGameName(), $id);
		$game = new $class_name($id);
		if ($game instanceof IGameMoveSerializable)
			$game->setAndLoadFromStorage(self::getGameDataStorage());
		return $game;
	}

	public static function checkGameID($name, $id, $toRaise = TRUE)
	{
		$is_correct = (is_numeric($id));
		if ($toRaise && !$is_correct)
			throw new GameMoveException("GameID is incorrect");
	}

	public static function newGame($name)
	{
		$class_name = self::loadGameByName($name);
		$id = self::getStorage()->getNewGameID($name);
		return self::CreateGameObject($class_name, $id);
	}
	
	public static function getGame($name, $id)
	{
		$class_name = self::loadGameByName($name);
		return self::CreateGameObject($class_name, $id);
	}
	
	public static function gamePrintGameList($name)
	{
		$class_name = self::loadGameByName($name);
		$data =& self::getStorage()->getGamesData($name);
		$storage = new GameDataReadOnlyMassStorage($data);

		$statuses = array();
		foreach($data as $row)
		{
			$game = new $class_name($row['id']);
			if ($game instanceof IGameMoveSerializable)
				$game->setAndLoadFromStorage($storage);
			$statuses[] = $game->listOfMoves();
		}
		return implode(";", $statuses);
	}

}
