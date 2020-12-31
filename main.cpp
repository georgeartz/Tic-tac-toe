//
//  main.cpp
//  TicTacToe_gtest project
//
//  Created by George Artz on 10/25/18.
//
#include <iostream>
#include <ostream>
#include <gtest/gtest.h>

using namespace std;

typedef int player_t;

enum : player_t { Player_None = 0, Player_One, Player_Two};

class GameState
{
public:
    GameState() { clearGameState();}
    ~GameState() { if (pgs) *pgs = *this;}
    void clearGameState()
    {
        winner = Player_None;
        row = col = 0;
        dlr = drl = false;
        pgs = NULL;
    }
    player_t winner;
    GameState *pgs;
    int row, col;
    bool dlr, drl;
};


class TicTacToeBoard
{
public:
    TicTacToeBoard(int RowSize)
    {
        rowSize = RowSize;
        boardSize = rowSize * rowSize;
        board = new int [boardSize];
        clearBoard();
    }
    ~TicTacToeBoard() { delete [] board;};
    int checkGame(GameState *pgs=NULL);
    void printBoard();
    void clearBoard();
    void SetRow(int rowNum, int val);
    void SetCol(int colNum, int val);
    void SetDLR(int val);
    void SetDRL(int val);
    void SetAllNoWinner();
private:
    player_t *board;  // array of Player elements. Each element represents a square on a tic-tac-toe board[rowSize*rowSize]
    int rowSize;    // number of ints in a row
    int boardSize;  // number of ints in board
    enum CkType { CK_ROW, CK_COL, CK_DLR, CK_DRL};
    int checkRCD(CkType type, int num=0); // check a row,colun or diagonal for all 1s or all 2s
    void printRow(int row);
    void SetRCD(int start, int inc, player_t player)
    {
        clearBoard();
        player_t i = start;
        for (int j = 0; j < rowSize; i+=inc, j++)
            board[i] = player;
    }
};

void TicTacToeBoard::clearBoard()
{
    for (int i = 0; i < boardSize; i++)     // init board to zeros
        board[i] = Player_None;
}
void TicTacToeBoard::SetRow(int rowNum, player_t player)
{
    if ((rowNum < 1) || (rowNum > rowSize)) return;
    SetRCD( (rowNum-1)*rowSize, 1, player);
}
void TicTacToeBoard::SetCol(int colNum, player_t player)
{
    if ((colNum < 1) || (colNum > rowSize)) return;
    SetRCD( colNum-1, rowSize, player);
}
void TicTacToeBoard::SetDLR(player_t player)
{
    SetRCD( 0, rowSize+1, player);
}
void TicTacToeBoard::SetDRL(player_t player)
{
    SetRCD( rowSize-1, rowSize-1, player);
}
void TicTacToeBoard::SetAllNoWinner()
{
    if (rowSize < 3) return;
    if (rowSize == 3)
    {
        int i=0, j;
        for (j=0; j<2; j++) board[i++] = Player_One;
        for (j=0; j<2; j++) board[i++] = Player_Two;
        for (j=0; j<3; j++) board[i++] = Player_One;
        for (j=0; j<2; j++) board[i++] = Player_Two;
        return;
    }
    
    // Set all positions on board such that:
    // 1. Each position is set to Player_One or Player_Two
    // 2. The number of Player_One and Player_Two positons should be equal or differ by 1
    // 3. There is no winner (no winner by row, column, or diagonal)
    player_t player;
    for (int row=0, i=0; row < rowSize; row++)
    {
        player = (row & 1) ? Player_Two : Player_One;
        for (int j = 0; j < rowSize; j++, i++)
        {
            board[i] = player;
            player = (player == Player_One) ? Player_Two : Player_One;
        }
    }

    // Swap center of diagonals to prevent a winner on either diagonal
    int i = (rowSize/2) * (rowSize + 1);    // i = index of middle position of left to right diagonal
    player = board[i];
    board[i] = board[i-1];
    board[i-1] = player;
}


// Check Board to see if either player has won
//
// Input:
// board... array that represents a tic-tac-toe board. Board[rowSize * rowSize]
//          The board represents a valid tic-tac-toe game.
//          Each element represents a square on the board and is set to Player_None, Player_One or Player_Two:
// pgs..... optional pointer to GameState object. If non null, will be set with game state
//
// Return:
// Player_None... no winner
// Player_one.... player 1 won. A row, column or diagonal is all Player_One
// Player_two.... player 2 won. A row, column or diagonal is all Player_Two
//
player_t TicTacToeBoard::checkGame(GameState *pgs)
{
    int i;
    GameState gs;
    gs.pgs = pgs;   // save caller's pointer. Destructor will copy to GameState to caller's GameState
    
    // Check each row and column for a winner (all 1s or all 2s)
    for (i = 0; i < rowSize; i++)
    {
        if ((gs.winner = checkRCD(CK_ROW, i))) {gs.row = i+1; break;};    // check a row. i is row rel to 0
        if ((gs.winner = checkRCD(CK_COL, i))) {gs.col = i+1; break;};    // check a column. i is col rel to 0
    }
    // Check diagonals for winner
    if (!gs.winner)
    {
        if ((gs.winner = checkRCD(CK_DLR))) gs.dlr = true;       // Check left to right diagonal for a winner
    }
    if (!gs.winner)
    {
        if ((gs.winner = checkRCD(CK_DRL))) gs.drl = true;       // Check right to left diagonal for a winner
    }
    return gs.winner;
}

// Check a row, column or diagonal for winner (all 1s or all 2s)
//
// Input:
// cktype... type of check: ckeck a row, column, or diagonal
// rowcol... number relative to 0 of row or column to be checked. Ignored for CK_DLR and CK_DRL
//
// Return:
// Player_None... row, col or diag not all Player_one or all Player_Two
// Player_One.... row, col or diag all Player_One
// Player_Two.... row, col or diag all Player_Two
//
player_t TicTacToeBoard::checkRCD(CkType cktype, int rowcol)
{
    int startIndex, inc;
    switch (cktype)
    {
        case CK_ROW:
            startIndex = rowcol * rowSize;
            inc = 1;
            break;
        case CK_COL:
            startIndex = rowcol;
            inc = rowSize;
            break;
        case CK_DLR:            // check left to right diagonal
            startIndex = 0;
            inc = rowSize+1;
            break;
        case CK_DRL:            // check right to left diagonal
            startIndex = rowSize-1;
            inc = rowSize-1;
            break;
        default:
            return Player_None;           // unsupported CkType
    }
    
    player_t first = board[startIndex];
    if ((first != Player_One) && (first != Player_Two)) return Player_None;
    
    int i = startIndex + inc;
    for (int j = 1; j < rowSize; j++, i+=inc)
        if (board[i] != first) return Player_None;
    return first;
}

void TicTacToeBoard::printRow(int row)
{
    int i = row * rowSize;
    for (int j = 0; j < rowSize; i++, j++)
        cout << board[i] << " ";
}

void TicTacToeBoard::printBoard()
{
    for (int i=0; i < rowSize; i++)
    {
        cout << "Board row " << i << " : ";
        printRow(i);
        cout << endl;
    }
}

TEST(TicTacToeTest, TestEmptyBoard)
{
    int rowSize = 6;
    TicTacToeBoard bd(rowSize);
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_EQ(gs.winner, Player_None);
}

TEST(TicTacToeTest, TestFullBoardNoWinner1)
{
    int rowSize = 3;
    TicTacToeBoard bd(rowSize);
    bd.SetAllNoWinner();
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_EQ(gs.winner, Player_None);
}

TEST(TicTacToeTest, TestFullBoardNoWinner2)
{
    int rowSize = 5;
    TicTacToeBoard bd(rowSize);
    bd.SetAllNoWinner();
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_EQ(gs.winner, Player_None);
}

TEST(TicTacToeTest, TestFullBoardNoWinner3)
{
    int rowSize = 6;
    TicTacToeBoard bd(rowSize);
    bd.SetAllNoWinner();
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_EQ(gs.winner, Player_None);
}

TEST(TicTacToeTest, TestRow)
{
    int rowSize = 6;
    TicTacToeBoard bd(rowSize);
    int row = 2;
    player_t player = Player_One;
    bd.SetRow(row,player);   // set a row to player
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_TRUE((gs.winner == player) && (gs.row == row));
}

TEST(TicTacToeTest, TestCol)
{
    int rowSize = 6;
    TicTacToeBoard bd(rowSize);
    int col = 2;
    player_t player = Player_Two;
    bd.SetCol(col, player);   // set a column to player
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_TRUE((gs.winner == player) && (gs.col == col));
}

// Test left to right diagonal
TEST(TicTacToeTest, TestDLR)
{
    int rowSize = 6;
    TicTacToeBoard bd(rowSize);
    player_t player = Player_Two;
    bd.SetDLR(player);   // set left to right diagonal to player
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_TRUE((gs.winner == player) && (gs.dlr == true));
}

// Test right to left diagonal
TEST(TicTacToeTest, TestDRL)
{
    int rowSize = 6;
    TicTacToeBoard bd(rowSize);
    player_t player = Player_One;
    bd.SetDRL(player);   // set right to left diagonal to player
    bd.printBoard();
    GameState gs;
    bd.checkGame(&gs);
    EXPECT_TRUE((gs.winner == player) && (gs.drl == true));
}


// Sample output from tests
#if 0
Running main() from /Users/George/googletest/googletest/src/gtest_main.cc
[==========] Running 5 tests from 1 test case.
[----------] Global test environment set-up.
[----------] 5 tests from TicTacToeTest
[ RUN      ] TicTacToeTest.TestEmptyBoard
Board row 0 : 0 0 0 0 0 0
Board row 1 : 0 0 0 0 0 0
Board row 2 : 0 0 0 0 0 0
Board row 3 : 0 0 0 0 0 0
Board row 4 : 0 0 0 0 0 0
Board row 5 : 0 0 0 0 0 0
[       OK ] TicTacToeTest.TestEmptyBoard (0 ms)
[ RUN      ] TicTacToeTest.TestRow
Board row 0 : 0 0 0 0 0 0
Board row 1 : 1 1 1 1 1 1
Board row 2 : 0 0 0 0 0 0
Board row 3 : 0 0 0 0 0 0
Board row 4 : 0 0 0 0 0 0
Board row 5 : 0 0 0 0 0 0
[       OK ] TicTacToeTest.TestRow (1 ms)
[ RUN      ] TicTacToeTest.TestCol
Board row 0 : 0 1 0 0 0 0
Board row 1 : 0 1 0 0 0 0
Board row 2 : 0 1 0 0 0 0
Board row 3 : 0 1 0 0 0 0
Board row 4 : 0 1 0 0 0 0
Board row 5 : 0 1 0 0 0 0
[       OK ] TicTacToeTest.TestCol (0 ms)
[ RUN      ] TicTacToeTest.TestDLR
Board row 0 : 2 0 0 0 0 0
Board row 1 : 0 2 0 0 0 0
Board row 2 : 0 0 2 0 0 0
Board row 3 : 0 0 0 2 0 0
Board row 4 : 0 0 0 0 2 0
Board row 5 : 0 0 0 0 0 2
[       OK ] TicTacToeTest.TestDLR (35 ms)
[ RUN      ] TicTacToeTest.TestDRL
Board row 0 : 0 0 0 0 0 2
Board row 1 : 0 0 0 0 2 0
Board row 2 : 0 0 0 2 0 0
Board row 3 : 0 0 2 0 0 0
Board row 4 : 0 2 0 0 0 0
Board row 5 : 2 0 0 0 0 0
[       OK ] TicTacToeTest.TestDRL (0 ms)
[----------] 5 tests from TicTacToeTest (36 ms total)

[----------] Global test environment tear-down
[==========] 5 tests from 1 test case ran. (36 ms total)
[  PASSED  ] 5 tests.
Program ended with exit code: 0

#endif

