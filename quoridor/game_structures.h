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
} Walls;

typedef struct {
	int x;
	int y;
	int pixelX;
	int pixelY;
	int walls;
} Player;

typedef struct {
    Player player1;
    Player player2;
} Players;

typedef struct {
	Board board;
	Walls walls;
	Players players;
	int currentPlayer;
} GameStatus;

extern GameStatus game;
#endif 
