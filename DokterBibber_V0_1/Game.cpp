#include "Game.h"

/**
* Beginning of the game class. Strikes is set to 0, gameOver at start is false.
*/
Game::Game()
{
	strikes = 0;
	gameOver = false;
}

/**
* Add a strike to the game. A strike means when the user hits part of the body of the displayed patient.
*/
void Game::AddStrike()
{
	strikes++;
}

/**
* Check if the game is over. Returns true if the game is over, false if not over.
* The game is over if the current strikes is greater of equal than the max amount of strikes.
*/
bool Game::IsGameOver()
{
	if (MAX_STRIKES >= strikes)
		gameOver = true;

	return gameOver;
}

/**
* Start a new game. GameOver is set to false and the stikes will reset.
*/
void Game::StartNewGame()
{
	strikes = 0;
	gameOver = false;
}

/**
* Returns the amount of strikes. A strike means when the user hits part of the body of the displayed patient.
* The user can only have a certain amount of strikes before the game is over.
*/
int Game::GetStrikes()
{
	return strikes;
}

/**
* Returns the max amount of strikes. A strike means when the user hits part of the body of the displayed patient.
* The user can only have a certain amount of strikes before the game is over.
*/
int Game::GetMaxAmountOfStrikes()
{
	return MAX_STRIKES;
}