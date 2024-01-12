#ifndef GAME_FUNCTIONS_H
#define GAME_FUNCTIONS_H

#include "game_structures.h"
#include <stdbool.h>

// LOGIC FUNCTIONS
void start_game(void);
void initialize_game(GameStatus *);
void move_token_up(GameStatus *);
int is_cell_free(GameStatus *, int, int, int, int);
void restore_available_player_cells_no_confirmation(GameStatus *, int);
void restore_available_player_cells(GameStatus *, int);
void set_initial_player_positions(GameStatus *);
void availablePlayerCells(GameStatus *);
void change_player_turn(GameStatus *);
void change_player_turn_after_confirm(GameStatus *);
void set_new_board_player_position(GameStatus *);
void set_temp_cordinates_player(GameStatus *, int, int, int, int);
void conferm_player_move(GameStatus *);
void set_new_board_player_position(GameStatus *);
void starting_player(GameStatus *);
void winner_player(GameStatus *);
void change_game_mode(GameStatus *);
void save_move_into_variable(void);
void decode_move(void);

// DRAWING FUNCTIONS
void draw_boardgame(void);
void write_remaining_walls_player1(GameStatus *);
void write_remaining_walls_player2(GameStatus *);
void highlight_cell(GameStatus *, int, int, int, int);
void restore_highlighted_cells(GameStatus *, int, int, int, int, int, int);
void print_value_on_screen(char[], int, int);
void cancel_value_on_screen(char[], int, int);
void cancel_actual_token_position(GameStatus *);
void draw_new_token_position(GameStatus *, int, int);
void preview_move_token(GameStatus *, int);
void spawn_center_wall(GameStatus *);
void draw_wall(GameStatus *, int, int);
void draw_wall_preview(GameStatus *, int, int);
void restore_wall(GameStatus *, int, int);
void restore_empty_wall(GameStatus *, int, int);

// WALLS FUNCTIONS
void walls_mode(GameStatus *);
void move_preview_horizontal_wall(GameStatus *, int);
void move_preview_vertical_wall(GameStatus *, int);
void confirm_move_wall(GameStatus *game);
void restore_empty_wall(GameStatus *, int, int);
void rotate_wall(GameStatus *);
int check_wall_presence(GameStatus *game, int, int, int);
void restore_horizontal_wall_movement(GameStatus *, int);
void restore_vertical_wall_movement(GameStatus *, int);
bool can_player_reach_opposite_border(GameStatus *, int, int, int);
#endif
