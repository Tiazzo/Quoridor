#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include "game_structures.h"

//LOGIC FUNCTIONS
void start_game(void);
void initialize_game(GameStatus *game);
void move_token_up(GameStatus *game);
int is_cell_free(GameStatus *game, int x, int y, int up, int right);
void set_initial_player_positions(GameStatus *game);
void change_player_turn(GameStatus *);
void change_player_turn_after_confirm(GameStatus*);
void set_temp_cordinates_player(GameStatus *, int, int, int, int);
void conferm_player_move(GameStatus* );
void set_new_player_position(GameStatus *);

//DRAWING FUNCTIONS
void draw_boardgame(void);
void write_remaining_walls_player1(GameStatus*);
void write_remaining_walls_player2(GameStatus*);
void highlight_cell(GameStatus*,int, int, int, int);
void print_value_on_screen (int, int);
void preview_move_token(GameStatus*, int);

#endif
