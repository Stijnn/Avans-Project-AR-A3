#pragma once
class Game
{
private:
	const int MAX_STRIKES = 3;
	int strikes;
	bool gameOver;
public:
	Game();
	void AddStrike();
	bool IsGameOver();
	void StartNewGame();
	int GetStrikes();
	int GetMaxAmountOfStrikes();
};

