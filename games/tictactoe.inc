<?php

define('status_x_won', 'x won');
define('status_0_won', '0 won');

/**
 * "Проверятель" состояния игры крестики-нолики 3x3
 * Для простоты и быстроты использует готовые шаблоны выигрышных ситуаций
 * Для определения отсутствия выигрывших заполняет все свободные ходы каждым из игроков по очереди и проверяет отсутствие победы.
 * Использование: $status = game_tictactoe3x3_win_check::check($moves);
 */

class game_tictactoe3x3_win_check
{
	private $win_moves = array(
		array(1,2,3), array(4,8,9), array(5,6,7), array(1,7,8), array(2,6,9), array(3,4,5), array(1,5,9), array(3,7,9)
		);

	private $moves;
	
	function __construct(&$moves)
	{
		$this->moves = $moves;
	}

	private function getOneGamerMoves($shift)
	{
		// Взять ходы одного игрока
		$gamer_moves = array();
		for($i=$shift;$i<count($this->moves);$i=$i+2)
			$gamer_moves[]= $this->moves[$i];
		return $gamer_moves;
	}
	
	private static function addEmptyMoves($moves, $moves_busy)
	{
		$result = $moves;
		for($i=1;$i<10;$i++)
			if (!in_array($i, $moves_busy))
				$result[] = $i;
		return $result;
	}

	private function checkIsWin($moves)
	{
		foreach($this->win_moves as $win_comb)
			if (count(array_intersect($win_comb, $moves)) == count($win_comb))
				return TRUE;
		return FALSE;
	}

	private function checkStatus()
	{
		$movesX = $this->getOneGamerMoves(0);
		if ($this->checkIsWin($movesX))
			return status_x_won;
		$moves0 = $this->getOneGamerMoves(1);
		if ($this->checkIsWin($moves0))
			return status_0_won;
		
		if (!$this->checkIsWin(self::addEmptyMoves($movesX, $moves0)) && !$this->checkIsWin(self::addEmptyMoves($moves0, $movesX)))
			return status_no_winner;

		return status_active;
	}
	
	public static function check(&$moves)
	{
		$cheker = new self($moves);
		return $cheker->checkStatus();
	}

}

/**
 * Собственно класс гры крестики-нолики 3x3
 * Перекрывает только необходимый минимум методов, которые необходимы для реализации именно этой игры
 * При изменении размерности игры можно переработать данный класс (на "универсальные" методы) или создать нового наследника
 */

class game_tictactoe3x3 extends GameMoveSerializable //GameMoveAbstract
{
	/* Ходы игры. Каждый элемент массива - номер клетки от 1..9
	123
	894
	765
	*/

	public static function getGameName()
	{
		return 'tictactoe';
	}
	
	protected function checkMove($move)
	{
		if (!is_numeric($move) || ((int)$move < 1) || ((int)$move > 9))
			throw new GameMoveException(responce_rejected." (Move is incorrect. Range: 1..9)");
		
		parent::checkMove($move);
	}

	protected function checkStatus()
	{
		return game_tictactoe3x3_win_check::check($this->moves);
	}

	public function getGamePositions()
	{
		$content = '123894765';
		foreach($this->moves as $key => $move)
		{
			$who = (((int)$key % 2) == 0) ? 'x' : 'o';
			$content = str_replace($move, $who, $content);
		}
		return array(substr($content,0,3),substr($content,3,3),substr($content,6,3));
	}
	
}

// Регистрация типа игры в менеджере. Чтобы он знал какой класс использовать для игры по имени
game_tictactoe3x3::Register();
