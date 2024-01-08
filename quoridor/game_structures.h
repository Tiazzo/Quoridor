#ifndef GAME_STRUCTURES_H
#define GAME_STRUCTURES_H
#include "game_constants.h"
#include <stdint.h>
typedef struct {
	int type;
} Cell;

typedef struct {
	Cell cells [BOARD_SIZE][BOARD_SIZE];
} Board;

typedef struct {
	int type;
} Wall;
typedef struct {
    Wall walls[BOARD_SIZE][BOARD_SIZE];
		int tempX;
		int tempY;
		int tempPixelX;
		int tempPixelY;
		int tempCellDirection;
		int wallVerse;
} Walls;

typedef struct {
	int x;
	int y;
	int pixelX;
	int pixelY;
	int tempX;
	int tempY;
	int tempPixelX;
	int tempPixelY;
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
	int gameMode;	
	uint8_t rountTimer;
} GameStatus;

typedef struct {
	unsigned int PlayerID;
	unsigned int PlayerMove_WallPlacement;
	unsigned int Vertical_Horizontal;
	unsigned int Y;
	unsigned int X;
} GameMove;

extern GameStatus game;
extern GameMove gameMove;
extern unsigned int move;
#endif 
