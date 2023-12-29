#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H
#include "game_constants.h"

typedef struct {
	int type;
} Cell;

typedef struct {
	Cell cells [BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct {
    int walls[BOARD_SIZE - 1][BOARD_SIZE][2];  // Muri orizzontali e verticali
    int player1Walls;  // Numero di muri disponibili per il giocatore 1
    int player2Walls;  // Numero di muri disponibili per il giocatore 2
} Walls;

typedef struct {
	int x;
	int y;
} Player;

typedef struct {
    Player player1;
    Player player2;
    int player1Walls;  // Numero di muri disponibili per il giocatore 1
    int player2Walls;  // Numero di muri disponibili per il giocatore 2
} Players;

typedef struct {
	Board board;
	Walls walls;
	Player Players;
} GameStatus;

#endif 
