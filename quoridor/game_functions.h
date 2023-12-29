#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include "game_structures.h"

//LOGIC FUNCTIONS
void start_game(void);
void initialize_game(GameStatus *game);
void move_token_up(GameStatus *game);
int isCellFree(GameStatus *game, int x, int y, int up, int right);
void setInitialPlayerPositions(GameStatus *game);

//DRAWING FUNCTIONS
void draw_boardgame(void);
void write_remaining_walls_player1(void);
void write_remaining_walls_player2(void);

#endif
