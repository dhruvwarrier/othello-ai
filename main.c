#include <stdio.h>
#include <stdbool.h>
#include <math.h>

void createDefaultBoard(char [26][26], int);
void printBoard(char [26][26], int);

void executeComputerTurn(char [26][26], int, char, char, bool*, bool*, bool*);
void executePlayerTurn(char [26][26], int, char, char, bool*, bool*, bool*);

void playMove(char[26][26], int, char[3]);
void findGreedyMove(char [26][26], int, char, char [2]);
void findBestFutureMove(char [26][26], int, char, char[2]);
bool doMovesExist(char [26][26], int, char);
bool isLegalMove(char [26][26], int, char [3]);
bool checkLegalInDirection(char [26][26], int, char, char, char, int, int, int*);
int calculateScoreInFuture(char [26][26], int, char, int, int, int);

bool positionInBounds(int, char, char);
bool BoardFull(char [26][26], int);

int countTileOfColour(char [26][26], int, char);
char oppositeColour(char);
bool isEdge(int, int, int);
bool isCorner(int, int, int);

char numToChar(int); // returns a for 0, b for 1, and so on
int charToNum(char); // returns 0 for a, 1 for b, and so on

int main(void)
{
	int boardDimension;
	char computerColour, playerColour;
	bool movesAvailableForComputer = true;
	bool movesAvailableForPlayer = true;
	bool invalidMoveEntered = false;
	bool isComputerFirst;
	int computerFinalScore, playerFinalScore;
	
	char board[26][26];
	
	printf("Enter the board dimension: ");
	scanf("%d", &boardDimension);
	printf("Computer plays (B/W) : ");
	
	scanf(" %c", &computerColour);
	playerColour = oppositeColour(computerColour);
	
	if (computerColour == 'B') {
		isComputerFirst = true;
	} else {
		isComputerFirst = false;
	}
	
	createDefaultBoard(board, boardDimension);
	printBoard(board, boardDimension);
	
	while(!BoardFull(board, boardDimension) && (movesAvailableForComputer || movesAvailableForPlayer) && !invalidMoveEntered) {
		
		movesAvailableForComputer = true; // initially we do not know if moves are available for either, it is safer to assume they are
		movesAvailableForPlayer = true; // whether they are available or not are computed when turns are made
		
		// execute turns in correct order, depending on who is first
		if (isComputerFirst) {
			executeComputerTurn(board, boardDimension, playerColour, computerColour, &movesAvailableForComputer, &movesAvailableForPlayer, &invalidMoveEntered);
			executePlayerTurn(board, boardDimension, playerColour, computerColour, &movesAvailableForComputer, &movesAvailableForPlayer, &invalidMoveEntered);
		} else {
			executePlayerTurn(board, boardDimension, playerColour, computerColour, &movesAvailableForComputer, &movesAvailableForPlayer, &invalidMoveEntered);
			executeComputerTurn(board, boardDimension, playerColour, computerColour, &movesAvailableForComputer, &movesAvailableForPlayer, &invalidMoveEntered);
		}
	}
	// after the game has ended, print the appropriate message
	if (invalidMoveEntered) {
		printf("%c player wins.", computerColour);
	} else {
		playerFinalScore = countTileOfColour(board, boardDimension, playerColour);
		computerFinalScore = countTileOfColour(board, boardDimension, computerColour); // counts the number of tiles of each player at the end
		
		if (playerFinalScore > computerFinalScore) {
			printf("%c player wins.", playerColour);
		} else if (computerFinalScore > playerFinalScore) {
			printf("%c player wins.", computerColour);
		} else {
			printf("Draw!");
		}
	}
	return 0;
}

void createDefaultBoard(char board[26][26], int boardDimension) {
	int i, j, n;
	n = boardDimension;
	
	for (i=0; i<n; i++) {
		for (j=0; j<n; j++) {
			board[i][j] = 'U'; // creates a blank board with dimension n
		}
	}
	board[n/2-1][n/2-1]='W';
	board[n/2-1][n/2]='B'; // prints WBBW in the center of the board
	board[n/2][n/2-1]='B';
	board[n/2][n/2]='W';
}

void printBoard(char board[26][26], int boardDimension) {
	int row, col;
	
	printf("  "); // shifts a-z directly above the board
	for (col = 0; col<boardDimension; col++) {
		printf("%c", numToChar(col)); // prints a-z on the top of the board
	}
	printf("\n");
	for (row = 0; row<boardDimension; row++) {
		printf("%c ", numToChar(row)); // prints a-z on the left of the board
		for (col = 0; col<boardDimension; col++) {
			printf("%c", board[row][col]); // prints contents of board row
		}
		printf("\n"); // end of board row
	}
}

void executeComputerTurn(char board[26][26], int boardDimension, char playerColour, char computerColour, bool* movesAvailableForComputer, bool* movesAvailableForPlayer, bool* invalidMoveEntered) {
	
	char completeMove[3]; // this contains player colour, row and column e.g. Wab or Bbc
	char bestMove[2]; // this only contains the row and column for a move eg. ab or db
	bool noMovesAvailable;
	
	findBestFutureMove(board, boardDimension, computerColour, bestMove); // finds the best move using a custom algorithm that looks several moves ahead
	*movesAvailableForPlayer = doMovesExist(board, boardDimension, playerColour); // makes sure to update the player moves available
	
	if (bestMove[0] == '!') { // if no moves are available ! is returned
		*movesAvailableForComputer = false;
		noMovesAvailable = !(*movesAvailableForComputer) && !(*movesAvailableForPlayer);
		if (!BoardFull(board, boardDimension) && !noMovesAvailable) { // makes sure an extraneous message is not printed after the end of the game
			printf("%c player has no valid move.\n", computerColour);
		}
	} else {
		if (!(*invalidMoveEntered)) { // makes sure a move is not made by the computer after the end of the game
			*movesAvailableForComputer = true;
			completeMove[0] = computerColour;
			completeMove[1] = bestMove[0];
			completeMove[2] = bestMove[1];
			playMove(board, boardDimension, completeMove);
			printf("Computer places %c at %c%c.\n", computerColour, bestMove[0], bestMove[1]);
			printBoard(board, boardDimension);
		}
	}
}

void executePlayerTurn(char board[26][26], int boardDimension, char playerColour, char computerColour, bool* movesAvailableForComputer, bool* movesAvailableForPlayer, bool* invalidMoveEntered) {
	
	char rowColMove[2]; // this only contains the row and column for a move eg. ab or db
	char move[3]; // this contains player colour, row and column e.g. Wab or Bbc
	bool noMovesAvailable;
	
	*movesAvailableForComputer = doMovesExist(board, boardDimension, computerColour); // updates whether moves are available for computer
	
	if (doMovesExist(board, boardDimension, playerColour)) { // only if moves exist, ask for input
		*movesAvailableForPlayer = true;
		printf("Enter move for colour %c (RowCol): ", playerColour);
		scanf("%s", rowColMove);
		move[0] = playerColour;
		move[1] = rowColMove[0];
		move[2] = rowColMove[1];
		
		if (isLegalMove(board, boardDimension, move)) { // if the move is legal, play the move
			playMove(board, boardDimension, move);
			printBoard(board, boardDimension);
		} else {
			printf("Invalid move.\n");
			*invalidMoveEntered = true; // ends the game if an illegal move is entered
		}
	} else {
		*movesAvailableForPlayer = false;
		noMovesAvailable = !(*movesAvailableForComputer) && !(*movesAvailableForPlayer);
		if (!BoardFull(board, boardDimension) && !noMovesAvailable) { // makes sure an extraneous message is not printed after the end of the game
			printf("%c player has no valid move.\n", playerColour);
		}
	}
}

void playMove(char board[26][26], int boardDimension, char move[3]) {
	
	char colour = move[0];
	int row, col; // iterate through board cells
	row = charToNum(move[1]);
	col = charToNum(move[2]);
	int rowDir, colDir; // iterate through the 8 possible directions
	int rowToEdit, colToEdit;
	
	board[row][col] = colour; // places tile of this colour
	for (rowDir = -1; rowDir<=1; rowDir++) {
		for (colDir = -1; colDir<=1; colDir++) { // looping without exiting assures all valid directions are flipped
			if (rowDir == 0 && colDir == 0) { // skips direction 0, 0
				continue;
			}
			if(checkLegalInDirection(board, boardDimension, move[1], move[2], colour, rowDir, colDir, NULL)) { // checks legality in every other direction
				rowToEdit = row + rowDir;
				colToEdit = col + colDir;
				while(board[rowToEdit][colToEdit] != colour) {
					board[rowToEdit][colToEdit] = colour; // if legal in that direction, flip all the tiles of opposite colour to this colour
					rowToEdit += rowDir;
					colToEdit += colDir;
				}
			}
		}
	}
}

void findGreedyMove(char board[26][26], int boardDimension, char colour, char bestMove[2]) {
	int row, col; // iterate through board cells
	int rowDir, colDir; // iterate through the 8 possible directions
	int scoreInThisDirection; // stores score in a particular direction from this position
	int currentScore = 0, highestScore = 0;
	int highestScoreRow = 0, highestScoreCol = 0; // stores the row and column of the highest score
	
	for (row = 0; row<boardDimension; row++) {
		for (col = 0; col<boardDimension; col++) {
			if (board[row][col] == 'U') {
				currentScore = 0;
				for (rowDir = -1; rowDir<=1; rowDir++) {
					for (colDir = -1; colDir<=1; colDir++) {
						if (rowDir == 0 && colDir == 0) {
							continue; // skips direction 0,0
						}
						// checks legality in every other direction
						if(checkLegalInDirection(board, boardDimension, numToChar(row), numToChar(col), colour, rowDir, colDir, &scoreInThisDirection)) {
							// add the score in this particular direction to the total current score for this position
							currentScore += scoreInThisDirection;
						}
					}
				}
				if (currentScore > highestScore) {
					highestScore = currentScore;
					highestScoreRow = row;
					highestScoreCol = col;
				} else if (currentScore == highestScore) {
					if (row < highestScoreRow) { // if the row is lower, this position is preferred
						highestScoreRow = row;
					} else if (row == highestScoreRow) {
						if (col < highestScoreCol) { // if the row is the same and the column is lower, this position is preferred
							highestScoreCol = col;
						}
					}
				}
			}
		}
	}
	if (highestScore == 0) {
		// if highest score is 0, no moves are available
		bestMove[0] = '!'; // send a unique error code, which must be checked when executing the computer turn
	} else {
		bestMove[0] = numToChar(highestScoreRow); // else send the highest move in RowCol form
		bestMove[1] = numToChar(highestScoreCol);
	}
}

void findBestFutureMove(char board[26][26], int boardDimension, char colour, char bestMove[2]) {
	int row, col; // iterate through board cells
	int rowDir, colDir; // iterate through the 8 possible directions
	int futureScore = 0; // holds actual board score for a computer move sometime in the future
	int numOfMovesToLookAhead = 4; // change this default value to check even further in the future, but this takes more time
	int highestScore = 0;
	int highestScoreRow = 0, highestScoreCol = 0; // stores the row and column of the highest score
	char move[3];
	bool exitCheck;
	
	for (row = 0; row<boardDimension; row++) {
		for (col = 0; col<boardDimension; col++) {
			if (board[row][col] == 'U') {
				futureScore = 0;
				move[0] = colour;
				move[1] = numToChar(row);
				move[2] = numToChar(col);
				if (isLegalMove(board, boardDimension, move)) {
					futureScore = calculateScoreInFuture(board, boardDimension, colour, row, col, numOfMovesToLookAhead);
				}
				if (futureScore > highestScore) {
					highestScore = futureScore;
					highestScoreRow = row;
					highestScoreCol = col;
				} else if (futureScore == highestScore) {
					if (row < highestScoreRow) { // if the row is lower, this position is preferred
						highestScoreRow = row;
					} else if (row == highestScoreRow) {
						if (col < highestScoreCol) { // if the row is the same and the column is lower, this position is preferred
							highestScoreCol = col;
						}
					}
				}
			}
		}
	}
	if (highestScore == 0) {
		// if highest score is 0, no moves are available
		bestMove[0] = '!'; // send a unique error code, which must be checked when executing the computer turn
	} else {
		bestMove[0] = numToChar(highestScoreRow); // else send the highest move in RowCol form
		bestMove[1] = numToChar(highestScoreCol);
	}
}

bool doMovesExist(char board[26][26], int boardDimension, char colour) {
	int row, col; // iterate through board cells
	int rowDir, colDir; // iterate through the 8 possible directions
	
	for (row = 0; row<boardDimension; row++) {
		for (col = 0; col<boardDimension; col++) {
			if (board[row][col] == 'U') {
				for (rowDir = -1; rowDir<=1; rowDir++) {
					for (colDir = -1; colDir<=1; colDir++) {
						if (rowDir == 0 && colDir == 0) {
							continue; // skips direction 0,0
						}
						// checks legality in every other direction
						if(checkLegalInDirection(board, boardDimension, numToChar(row), numToChar(col), colour, rowDir, colDir, NULL)) {
							return true; // returns back even if a single move exists
						}
					}
				}
			}
		}
	}
	return false; // it checked all the positions and did not find a move
}

bool isLegalMove(char board[26][26], int boardDimension, char move[3]) {
	
	char colour = move[0];
	int row, col;
	row = charToNum(move[1]);
	col = charToNum(move[2]);
	int rowDir, colDir; // iterate through the 8 possible directions
	
	if (board[row][col] == 'U') {
		for (rowDir = -1; rowDir<=1; rowDir++) {
			for (colDir = -1; colDir<=1; colDir++) {
				if (rowDir == 0 && colDir == 0) { // skips direction 0, 0
					continue;
				}
				if(checkLegalInDirection(board, boardDimension, numToChar(row), numToChar(col), colour, rowDir, colDir, NULL)) {
					// checks legality in every other direction
					return true;
				}
			}
		}
		return false;
	}
}

bool checkLegalInDirection(char board[26][26], int boardDimension, char row, char col, char colour, int deltaRow, int deltaCol, int* scoreInThisDirection) {
	
	int rowInt, colInt;
	rowInt = charToNum(row);
	colInt = charToNum(col);
	int rowToCheck, colToCheck;
	rowToCheck = rowInt + deltaRow; // need to be iterated initially to check if position is in bounds
	colToCheck = colInt + deltaCol;
	int numOfOppColourSeen = 0; // counts the number of opposite coloured tiles seen before reaching the same colour
	
	while(positionInBounds(boardDimension, numToChar(rowToCheck), numToChar(colToCheck)) && board[rowToCheck][colToCheck] == oppositeColour(colour)) { 
		// only advance if position in bounds and opposite colour exists in this direction
		numOfOppColourSeen++;
		rowToCheck += deltaRow;
		colToCheck += deltaCol;
	}
	
	if (!positionInBounds(boardDimension, numToChar(rowToCheck), numToChar(colToCheck))) {
		return false;
	} else if (board[rowToCheck][colToCheck] == colour && numOfOppColourSeen >= 1) {
		// if it reached this colour and saw more than 1 opposite colour, this is a legal direction
		if (scoreInThisDirection != NULL) {
		// score is only updated if computer is making a move
			*scoreInThisDirection = numOfOppColourSeen;
		}
		return true;
	} else if (board[rowToCheck][colToCheck] == 'U') {
		return false;
	} else {
		return false;
	}
}

int calculateScoreInFuture(char currentBoard[26][26], int boardDimension, char colour, int rowToPlay, int colToPlay, int numOfMovesToLookAhead) {
	char futureBoard[26][26];
	int i,j;
	char moveToPlay[3];
	char rowColMove[2];
	char completeMoveByComputer[3];
	char completeMoveByProfAI[3];
	char moveByProfAI[2];
	
	for (i=0; i<boardDimension; i++) {
		for (j=0; j<boardDimension; j++) {
			futureBoard[i][j] = currentBoard[i][j]; // copies current board to future board
		}
	}
	moveToPlay[0] = colour;
	moveToPlay[1] = numToChar(rowToPlay);
	moveToPlay[2] = numToChar(colToPlay);
	
	for(i=0; i<numOfMovesToLookAhead; i++) {
		if (i == 0) {
			playMove(futureBoard, boardDimension, moveToPlay);
			findGreedyMove(futureBoard, boardDimension, oppositeColour(colour), moveByProfAI);
			if (moveByProfAI[0] == '!') {
				continue;
			} else {
				completeMoveByProfAI[0] = oppositeColour(colour);
				completeMoveByProfAI[1] = moveByProfAI[0];
				completeMoveByProfAI[2] = moveByProfAI[1];
				playMove(futureBoard, boardDimension, completeMoveByProfAI);
			}
		} else {
			findGreedyMove(futureBoard, boardDimension, colour, rowColMove);
			if (rowColMove[0] != '!') {
				completeMoveByComputer[0] = colour;
				completeMoveByComputer[1] = rowColMove[0];
				completeMoveByComputer[2] = rowColMove[1];
				playMove(futureBoard, boardDimension, completeMoveByComputer);
			}
			findGreedyMove(futureBoard, boardDimension, oppositeColour(colour), moveByProfAI);
			if (moveByProfAI[0] == '!') {
				continue;
			} else {
				completeMoveByProfAI[0] = oppositeColour(colour);
				completeMoveByProfAI[1] = moveByProfAI[0];
				completeMoveByProfAI[2] = moveByProfAI[1];
				playMove(futureBoard, boardDimension, completeMoveByProfAI);
			}
		}
	}
	
	return countTileOfColour(futureBoard, boardDimension, colour);
	
}

bool positionInBounds(int boardDimension, char row, char col) {
	// checks if within bounds i.e. 0 and dimension of board
	if (charToNum(row) >= 0 && charToNum(row) < boardDimension && charToNum(col) >= 0 && charToNum(col) < boardDimension) {
		return true;
	} else {
		return false;
	}
}

bool BoardFull(char board[26][26], int boardDimension) {
	int i, j;
	
	for (i=0; i<boardDimension; i++) {
		for (j=0; j<boardDimension; j++) {
			if (board[i][j] == 'U') {
				return false; // if any space is empty, board is not full
			}
		}
	}
	return true; // if it reaches end, board must be full
}

int countTileOfColour(char board[26][26], int boardDimension, char colour) {
	int row, col; // iterate through board cells
	int count = 0;
	
	for (row = 0; row<boardDimension; row++) {
		for (col = 0; col<boardDimension; col++) {
			if (board[row][col] == colour) {
				count++;
			}
		}
	}
	return count;
}

char oppositeColour(char colour) { // colour definitions can be provided here
	 if (colour == 'W') {
		return 'B'; 
	} else if (colour == 'B') {
		return 'W';
	}
}

bool isEdge(int row, int col, int boardDimension) {
	int n = boardDimension;
	if (((row > 1 || row < n-2) && (row == 0 && row == n-1)) || ((row > 1 || row < n-2) && (row == 0 || row == n-1))) {
		return true;
	} else {
		return false;
	}
}

bool isCorner(int row, int col, int boardDimension) {
	if ((row == 0 && col == 0) || (row == 0 && col == boardDimension - 1) || (row == boardDimension - 1 && col == 0) || (row == boardDimension - 1 && col == boardDimension - 1)) {
		return true;
	} else {
		return false;
	}
}

char numToChar(int number) {
	return ('a' + number);
}

int charToNum(char character) {
	return (character - 'a');
}