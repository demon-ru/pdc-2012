<?php
//phpinfo();

require_once(dirname(__FILE__).'/games-dispatcher.inc');

try{
	require_once(dirname(__FILE__).'/.config-games.inc');

	GameDispatcher::Dispatch();
	GameManager::ClearStorage();
} catch (Exception $e) {
	GameDispatcher::printLine($e->getMessage());
}
