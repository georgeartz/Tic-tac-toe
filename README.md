# Tic-tac-toe
Check tic-tac-toe board to see if either player has won. 

Input:
	board... array that represents a tic-tac-toe board. Board[rowSize * rowSize]
			 The board represents a valid tic-tac-toe game.
			 Each element represents a square on the board and is set to Player_None,
			 Player_One or Player_Two
	pgs..... optional pointer to GameState object. If non null, will be set with game state
	
Return:
	Player_None... no winner
	Player_one.... player 1 won. A row, column or diagonal is all Player_One
	Player_two.... player 2 won. A row, column or diagonal is all Player_Two
	
player_t TicTacToeBoard::checkGame(GameState *pgs)
{	
	...
}
	

