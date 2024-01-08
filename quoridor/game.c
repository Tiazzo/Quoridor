#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"

#include "game_constants.h"
#include "game_functions.h"
#include "game_structures.h"
#include "game_draw.h"

//int walls_player_one = 9;
//int walls_player_two = 9;
int firstTimePlayer1 = 1;
int firstTimePlayer2 = 1;

GameStatus game;
GameMove gameMove;
unsigned int move;


void draw_boardgame(void){
	int i,j,k;
	int rect_width = 70;
  int rect_height = 40;
	int spacing = 7;
	int start_x = 7;
	int start_y = 260;
	
	
	for(i=0; i <= 204; i=i+34){
			for(j=0; j<=204; j=j+34){
			LCD_DrawArray(cell_background,30,30,i+3,j+3);
		}
	}
	
	//Draw rectangles
	for (k = 0; k < 3; k++) {
			int x0 = start_x + k * (rect_width + spacing);
			int y0 = start_y;
			int x1 = x0 + rect_width;
			int y1 = y0 + rect_height;
		
			LCD_DrawLine(x0, y0, x1, y0, Violet);  // Linea superiore
			LCD_DrawLine(x0, y0, x0, y1, Violet);  // Linea sinistra
			LCD_DrawLine(x1, y0, x1, y1, Violet);  // Linea destra
			LCD_DrawLine(x0, y1, x1, y1, Violet);  // Linea inferiore
			if(k==0){
				GUI_Text(x0+7, y0+2, (uint8_t *) "P1 Wall",Black, White );
			} else if (k==2){
				GUI_Text(x0+7, y0+2, (uint8_t *) "P2 Wall",Black, White );
			}
	}
	print_value_on_screen("Press INT0 to start the match",5,242);
}

void write_remaining_walls_player1(GameStatus *game){
	char str1[20];
	if(firstTimePlayer1)
		firstTimePlayer1--;
	else
		game->players.player1.walls--;
	
	sprintf(str1, "%d", game->players.player1.walls);
	GUI_Text(40, 280, (uint8_t *) str1, Black, White );
}
void write_remaining_walls_player2(GameStatus *game){
	char str2[20];
		if(firstTimePlayer2)
				firstTimePlayer2--;
	else
		game->players.player2.walls--;
	sprintf(str2, "%d", game->players.player2.walls);
	GUI_Text(192, 280,(uint8_t *) str2, Black, White );
}


void print_value_on_screen (char str2[], int x, int y){
	GUI_Text(x, y,(uint8_t *) str2, Red, White );
}
	
void cancel_value_on_screen(char str2[], int x, int y){
	GUI_Text(x, y,(uint8_t *) str2, White, White );
}


int is_cell_free(GameStatus *game, int x, int y, int up, int right){
	if ((x >= 0 && x < BOARD_SIZE) && (y >= 0 && y < BOARD_SIZE)){
		if(game->board.cells[x][y].type == EMPTY){
			if(up==1 && right==0){
				if(game->walls.walls[x][y].type % 7 != 0)
					return EMPTY;
				else
					return INVALID_MOVE;
			} else if(up==1 && right==1){
				if(game->walls.walls[x][y].type % 2 != 0)
					return EMPTY;
				else
					return INVALID_MOVE;
			} else if(up==0 && right==1){
				if(game->walls.walls[x][y].type % 5 != 0)
					return EMPTY;
				else
					return INVALID_MOVE;
			} else if(up==0 && right==0){
				if(game->walls.walls[x][y].type % 3 != 0)
					return EMPTY;
				else
					return INVALID_MOVE;
			}
		}else{	//another player
			if((x-1 >= 0 && x+1 < BOARD_SIZE) && (y-1 >= 0 && y+1 < BOARD_SIZE)){
				if(up==1 && right==0){ //up
					if (x-1 >= 0 && x-1 < BOARD_SIZE){
						if(game->walls.walls[x][y].type % 7 != 0 && game->walls.walls[x][y].type % 2 != 0)
							return ANOTHER_PLAYER;
					}else{
						return INVALID_MOVE;
					}
				}else if(up==1 && right==1){ //down
					if (x+1 >= 0 && x+1 < BOARD_SIZE){
						if(game->walls.walls[x][y].type % 2 != 0 && game->walls.walls[x][y].type % 7 != 0)
							return ANOTHER_PLAYER;
					}else{
						return INVALID_MOVE;
					}
				}else if (up==0 && right==1){ //right
					if (y+1>= 0 && y+1 < BOARD_SIZE){
						if(game->walls.walls[x][y].type % 5 != 0 && game->walls.walls[x][y].type % 3 != 0)
							return ANOTHER_PLAYER;
					}else{
						return INVALID_MOVE;
					}
				}else if(up==0 && right==0){	//left
					if(y-1 >= 0 && y-1 < BOARD_SIZE){
						if(game->walls.walls[x][y].type % 3 != 0 && game->walls.walls[x][y].type % 5 != 0)
							return ANOTHER_PLAYER;
					}else{
							return INVALID_MOVE;
					}
				}					
			}else{
			return INVALID_MOVE;
		}
		}
	}
	return INVALID_MOVE; //aggiunta per togliere un warnings	
}

void cancel_actual_token_position(GameStatus *game){
	if(game->currentPlayer == 1){
		LCD_DrawArray(cell_background, 30, 30, game->players.player1.pixelX, game->players.player1.pixelY);
	} else {
		LCD_DrawArray(cell_background, 30, 30, game->players.player2.pixelX, game->players.player2.pixelY);
	}
}

void draw_new_token_position(GameStatus *game, int x, int y){
	if(game->currentPlayer == 1){
		LCD_DrawArray(player1_draw, 30, 30, x, y);
	} else {
		LCD_DrawArray(player2_draw, 30, 30, x, y);
	}
}



//Da chiamare quando clicclo select 
void move_token_up(GameStatus *game){
	if(game->currentPlayer == 1){
		int currentX = game->players.player1.x;
		int currentY = game->players.player1.y;
		int newX = currentX - 1;
    int newY = currentY;

		int cellUp = is_cell_free(game,newX, newY, 1, 0);
		
		if(cellUp == EMPTY){
			//Restore cell to empty 
			game->board.cells[currentX][currentY].type = EMPTY;
			//Cancel token from actual position LCD
			cancel_actual_token_position(game);
			game-> players.player1.x = newX;
			game-> players.player1.y = newY;
			game->players.player1.pixelY = game->players.player1.pixelY - 34;
			//Update cell to Player1
			game->board.cells[game->players.player1.x][game->players.player1.y].type = PLAYER1;
			//TODO: Call token_drawing() function
			draw_new_token_position(game, game->players.player1.pixelX, game->players.player1.pixelY);
		} else if (cellUp == ANOTHER_PLAYER){	//poi modificare
			//Restore cell to empty 
			game->board.cells[currentX][currentY].type = EMPTY;
			game-> players.player1.x = newX-1;
			game-> players.player1.y = newY;
			//Update cell to Player1
			game->board.cells[game->players.player1.x][game->players.player1.y].type = PLAYER1;
			//TODO: Call token_drawing() function

		}
	} else {
		int currentX = game->players.player2.x;
		int currentY = game->players.player2.y;
		int newX = currentX-1;
    int newY = currentY;

		int cellUp = is_cell_free(game,newX, newY,1,0);
		
		if(cellUp == EMPTY){
			//Restore cell to empty 
			game->board.cells[currentX][currentY].type = EMPTY;
			game-> players.player2.x = newX;
			game-> players.player2.y = newY;
			//Update cell to Player2
			game->board.cells[game->players.player2.x][game->players.player2.y].type = PLAYER1;
			//TODO: Call token_drawing() function
		} else if (cellUp == ANOTHER_PLAYER){	//poi modificare
			//Restore cell to empty 
			game->board.cells[currentX][currentY].type = EMPTY;
			game-> players.player2.x = newX-1;
			game-> players.player2.y = newY;
			//Update cell to Player1
			game->board.cells[game->players.player2.x][game->players.player2.y].type = PLAYER1;
			//TODO: Call token_drawing() function
		}
	}
}

void highlight_cell(GameStatus *game,int cellUp, int cellDown, int cellLeft, int cellRight){
	if(game->currentPlayer == 1){
		if(cellUp == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
		} else if(cellUp == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-68);
		}
		if(cellDown == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX , game-> players.player1.pixelY+34);
		} else if(cellDown == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY+68);
		}
		if(cellLeft == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
		} else if(cellLeft == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX-68, game-> players.player1.pixelY);
		}
		if(cellRight == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
		} else if(cellRight == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player1.pixelX+68, game-> players.player1.pixelY);
		}
	}else{
		if(cellUp == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
		} else if(cellUp == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-68);
		}
		if(cellDown == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+34);
		} else if(cellDown == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+68);
		}
		if(cellLeft == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
		} else if(cellLeft == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX-38, game-> players.player2.pixelY);
		}
		if(cellRight == EMPTY){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
		} else if(cellRight == ANOTHER_PLAYER){
			LCD_DrawArray(highlighted_cell, 30, 30, game-> players.player2.pixelX+68, game-> players.player2.pixelY);
		}
	}
}

void restore_highlighted_cells(GameStatus *game,int cellUp, int cellDown, int cellLeft, int cellRight, int byPassed, int confirm){
	if(game->currentPlayer == 1){
		/*
		if((game->players.player1.x == game->players.player1.tempX) && (game->players.player1.y == game->players.player1.tempY)){
			if(game->players.player1.y == 0){
				if(game->players.player2.x == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX , game-> players.player1.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				} else if(game->players.player2.x == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX , game-> players.player1.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX , game-> players.player1.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				}
			} else if(game->players.player1.y == BOARD_SIZE-1){
				if(game->players.player2.x == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				} else if(game->players.player2.x == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				}
			} else if(game->players.player1.x == 0) {
				if(game->players.player2.y == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				} else if(game->players.player2.y == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
				}
			}	else if(game->players.player1.x == BOARD_SIZE-1){
				if(game->players.player2.y == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
				} else if(game->players.player2.y == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY+34);
				}
			} 
		}
		*/
		if(cellUp == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-34);
		} else if(cellUp == ANOTHER_PLAYER){
			if(!(byPassed == UP)){
				LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY-68);
			} 
		}
		if(cellDown == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX , game-> players.player1.pixelY+34);
		} else if(cellDown == ANOTHER_PLAYER){
			if(!(byPassed == DOWN)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX, game-> players.player1.pixelY+68);
			}
		}
		if(cellLeft == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-34, game-> players.player1.pixelY);
		} else if(cellLeft == ANOTHER_PLAYER){
			if(!(byPassed == LEFT)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX-68, game-> players.player1.pixelY);
			}
		}
		if(cellRight == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+34, game-> players.player1.pixelY);
		} else if(cellRight == ANOTHER_PLAYER){
			if(!(byPassed == RIGHT)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player1.pixelX+68, game-> players.player1.pixelY);
			}
		}
	}else{
		/*
		if((game->players.player2.x == game->players.player2.tempX) && (game->players.player2.y == game->players.player2.tempY)){
			if(game->players.player2.y == 0){
				if(game->players.player2.x == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX , game-> players.player2.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				} else if(game->players.player2.x == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX , game-> players.player2.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX , game-> players.player2.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				}
			} else if(game->players.player2.y == BOARD_SIZE-1){
				if(game->players.player2.x == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				} else if(game->players.player2.x == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				}
			} else if(game->players.player2.x == 0) {
				if(game->players.player2.y == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				} else if(game->players.player2.y == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
				}
			}	else if(game->players.player2.x == BOARD_SIZE-1){
				if(game->players.player2.y == 0){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
				} else if(game->players.player2.y == BOARD_SIZE-1){
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
				}	else{			
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
					LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+34);
				}
			}	
		}
		*/
		if(cellUp == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-34);
		} else if(cellUp == ANOTHER_PLAYER){
			if(!(byPassed == UP)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY-68);
			}
		}
		if(cellDown == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+34);
		} else if(cellDown == ANOTHER_PLAYER){
			if(!(byPassed == DOWN)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX, game-> players.player2.pixelY+68);
			}
		}
		if(cellLeft == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-34, game-> players.player2.pixelY);
		} else if(cellLeft == ANOTHER_PLAYER){
			if(!(byPassed == LEFT)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX-38, game-> players.player2.pixelY);
			}
		}
		if(cellRight == EMPTY){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+34, game-> players.player2.pixelY);
		} else if(cellRight == ANOTHER_PLAYER){
			if(!(byPassed == RIGHT)){
			LCD_DrawArray(cell_background, 30, 30, game-> players.player2.pixelX+68, game-> players.player2.pixelY);
			}
		}
	}
}

void restore_available_player_cells_no_confirmation(GameStatus *game, int byPassed){
	int currentX;
	int currentY;

	int cellUp = EMPTY;
	int cellDown = EMPTY;
	int cellLeft = EMPTY;
	int cellRight = EMPTY;
	
	if(game->currentPlayer == 1){
		currentX = game->players.player1.x;
		currentY = game->players.player1.y;
		
		if(game->board.cells[game->players.player1.tempX][game->players.player1.tempY-1].type == PLAYER2){
			cellUp = ANOTHER_PLAYER;
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if (game->board.cells[game->players.player1.tempX][game->players.player1.tempY+1].type == PLAYER2){
			cellDown = ANOTHER_PLAYER;
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if(game->board.cells[game->players.player1.tempX-1][game->players.player1.tempY].type == PLAYER2) {
			cellLeft = ANOTHER_PLAYER;
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}	else if(game->board.cells[game->players.player1.tempX+1][game->players.player1.tempY].type == PLAYER2) {
			cellRight = ANOTHER_PLAYER;
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		} else {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}
		
		/*
		if((game->players.player1.tempX == currentX && game->players.player1.tempY == currentY-1) || (game->players.player1.tempX == currentX && game->players.player1.tempY == currentY-2)){
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if ((game->players.player1.tempX == currentX && game->players.player1.tempY == currentY+1) || (game->players.player1.tempX == currentX && game->players.player1.tempY == currentY+2)){
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if((game->players.player1.tempX == currentX-1 && game->players.player1.tempY == currentY) || (game->players.player1.tempX-2 == currentX && game->players.player1.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}	else if((game->players.player1.tempX == currentX+1 && game->players.player1.tempY == currentY) || (game->players.player1.tempX+2 == currentX && game->players.player1.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		}
		*/
		restore_highlighted_cells(game, cellUp, cellDown, cellLeft, cellRight, byPassed, 0);
	} else {
		currentX = game->players.player2.x;
		currentY = game->players.player2.y;
		
		if(game->board.cells[currentX][currentY-1].type == PLAYER1){
			cellUp = ANOTHER_PLAYER;
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if (game->board.cells[currentX][currentY+1].type == PLAYER1){
			cellDown = ANOTHER_PLAYER;
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if(game->board.cells[currentX-1][currentY].type == PLAYER1) {
			cellLeft = ANOTHER_PLAYER;
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}	else if(game->board.cells[currentX+1][currentY].type == PLAYER1) {
			cellRight = ANOTHER_PLAYER;
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		} else {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}
		
		/*
		if((game->players.player2.tempX == currentX && game->players.player2.tempY == currentY-1) || (game->players.player2.tempX == currentX && game->players.player2.tempY == currentY-2)){
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if ((game->players.player2.tempX == currentX && game->players.player2.tempY == currentY+1) || (game->players.player2.tempX == currentX && game->players.player2.tempY == currentY+2)){
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if((game->players.player2.tempX == currentX-1 && game->players.player2.tempY == currentY) || (game->players.player2.tempX-2 == currentX && game->players.player2.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}	else if((game->players.player2.tempX == currentX+1 && game->players.player2.tempY == currentY) || (game->players.player2.tempX+2 == currentX && game->players.player2.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		}
		*/
		
		restore_highlighted_cells(game, cellUp, cellDown, cellLeft, cellRight, byPassed, 0);
	}
}

void restore_available_player_cells(GameStatus *game, int byPassed){
	int currentX;
	int currentY;
	int cellUp = NOTHING;
	int cellDown = NOTHING;
	int cellLeft = NOTHING;
	int cellRight = NOTHING;
	
	if(game->currentPlayer == 1){
		currentX = game->players.player1.x;
		currentY = game->players.player1.y;
		
		if((game->players.player1.tempX == currentX && game->players.player1.tempY == currentY-1) || (game->players.player1.tempX == currentX && game->players.player1.tempY == currentY-2)){
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if ((game->players.player1.tempX == currentX && game->players.player1.tempY == currentY+1) || (game->players.player1.tempX == currentX && game->players.player1.tempY == currentY+2)){
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if((game->players.player1.tempX == currentX-1 && game->players.player1.tempY == currentY) || (game->players.player1.tempX-2 == currentX && game->players.player1.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}	else if((game->players.player1.tempX == currentX+1 && game->players.player1.tempY == currentY) || (game->players.player1.tempX+2 == currentX && game->players.player1.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		} else if ((game->players.player1.x == game->players.player1.tempX) && (game->players.player1.y == game->players.player1.tempY)){
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}
		restore_highlighted_cells(game, cellUp, cellDown, cellLeft, cellRight, byPassed, 1);
	} else {
		currentX = game->players.player2.x;
		currentY = game->players.player2.y;

		if((game->players.player2.tempX == currentX && game->players.player2.tempY == currentY-1) || (game->players.player2.tempX == currentX && game->players.player2.tempY == currentY-2)){
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if ((game->players.player2.tempX == currentX && game->players.player2.tempY == currentY+1) || (game->players.player2.tempX == currentX && game->players.player2.tempY == currentY+2)){
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		} else if((game->players.player2.tempX == currentX-1 && game->players.player2.tempY == currentY) || (game->players.player2.tempX-2 == currentX && game->players.player2.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}	else if((game->players.player2.tempX == currentX+1 && game->players.player2.tempY == currentY) || (game->players.player2.tempX+2 == currentX && game->players.player2.tempY == currentY)) {
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		} else if ((game->players.player2.x == game->players.player2.tempX) && (game->players.player2.y == game->players.player2.tempY)){
			cellUp = is_cell_free(game,currentX,currentY-1,1,0);
			cellDown = is_cell_free(game,currentX,currentY+1,1,1);
			cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
			cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		}
		
		restore_highlighted_cells(game, cellUp, cellDown, cellLeft, cellRight, byPassed, 1);
	}
}

void availablePlayerCells (GameStatus *game){
	int currentX;
	int currentY;
	int cellUp;
	int cellDown;
	int cellLeft;
	int cellRight;
	
	if(game->currentPlayer == 1){
		currentX = game->players.player1.x;
		currentY = game->players.player1.y;
	
		cellUp = is_cell_free(game,currentX,currentY-1,1,0);
		cellDown = is_cell_free(game,currentX,currentY+1,1,1);
		cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		highlight_cell(game, cellUp, cellDown, cellLeft, cellRight);
	} else {
		currentX = game->players.player2.x;
		currentY = game->players.player2.y;
		cellUp = is_cell_free(game,currentX,currentY-1,1,0);
		cellDown = is_cell_free(game,currentX,currentY+1,1,1);
		cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
		cellRight = is_cell_free(game,currentX+1,currentY,0,1);
		highlight_cell(game, cellUp, cellDown, cellLeft, cellRight);
	}
}

//Change turn due to end of timer for both modes
void change_player_turn(GameStatus *game){
	if(game->gameMode == MOVE_MODE){
		restore_available_player_cells_no_confirmation(game, 0);
		if(game->currentPlayer == 1){
			gameMove.PlayerID = 0;
			draw_new_token_position(game, game->players.player1.pixelX, game->players.player1.pixelY);
			//restore initial round values
			game->players.player1.tempX = game->players.player1.x;
			game->players.player1.tempPixelX = game->players.player1.pixelX;
			game->players.player1.tempY = game->players.player1.y;
			game->players.player1.tempPixelY = game->players.player1.pixelY;
			game->currentPlayer = 2;
		}else{
			gameMove.PlayerID = 1;
			draw_new_token_position(game, game->players.player2.pixelX, game->players.player2.pixelY);
			game->players.player2.tempX = game->players.player2.x;
			game->players.player2.tempPixelX = game->players.player2.pixelX;
			game->players.player2.tempY = game->players.player2.y;
			game->players.player2.tempPixelY = game->players.player2.pixelY;
			game->currentPlayer = 1;
		}
	}else{
		if(game->currentPlayer == 1){
			gameMove.PlayerID = 0;
			if (game->walls.wallVerse == HORIZONTAL_WALL){
				restore_horizontal_wall_movement(game,0);
			}else{
				restore_vertical_wall_movement(game,0);
			}
			game->currentPlayer = 2;
		}else{
			gameMove.PlayerID = 1;
			if (game->walls.wallVerse == HORIZONTAL_WALL){
				restore_horizontal_wall_movement(game,0);
			}else{
				restore_vertical_wall_movement(game,0);
			}
			game->currentPlayer = 1;
		}
	}
	gameMove.PlayerMove_WallPlacement = 0;
	gameMove.Vertical_Horizontal = 1;
	gameMove.X = game->players.player2.x;
	gameMove.Y = game->players.player2.y;
	save_move_into_variable();
	
	availablePlayerCells(game);
	game->gameMode = MOVE_MODE;
	//enable_timer(0);
}



void change_player_turn_after_confirm(GameStatus *game){
	if(game->gameMode == MOVE_MODE){
		if(game->currentPlayer == 1){
			draw_new_token_position(game, game->players.player1.pixelX, game->players.player1.pixelY);
			game->currentPlayer = 2;
		}else{
			draw_new_token_position(game, game->players.player2.pixelX, game->players.player2.pixelY);
			game->currentPlayer = 1;
		}
	} else {
		if(game->currentPlayer == 1){
			write_remaining_walls_player1(game);
			game->currentPlayer = 2;
		}
		else{ 
			write_remaining_walls_player2(game);
			game->currentPlayer = 1;	
		}
	}
	availablePlayerCells(game);
	game->gameMode = MOVE_MODE;
	game->rountTimer = 20;
	reset_timer(0);
	//enable_timer(0);
}

void start_game(){
	cancel_value_on_screen("Press INT0 to start the match",5,242);
	initialize_game(&game);
}

void set_new_board_player_position(GameStatus *game){
	
	if(game->currentPlayer == 1){
		game->board.cells[game->players.player1.x][game->players.player1.y].type = EMPTY;
		game->board.cells[game->players.player1.tempX][game->players.player1.tempY].type = PLAYER1;
	}else{
		game->board.cells[game->players.player2.x][game->players.player2.y].type = EMPTY;
		game->board.cells[game->players.player2.tempX][game->players.player2.tempY].type = PLAYER2;
	}
}


void set_initial_player_positions(GameStatus *game){
	//Initialization player 1
	game->players.player1.x = 3;
	game->players.player1.y = 6;
	game->players.player1.pixelX = 105;
	game->players.player1.pixelY = 207;
	game->players.player1.tempX = 3;
	game->players.player1.tempY = 6;
	game->players.player1.tempPixelX = 105;
	game->players.player1.tempPixelY = 207;
	game->board.cells[game->players.player1.x][game->players.player1.y].type = PLAYER1;
	
	//Initialization player 2
	game->players.player2.x = 3;
	game->players.player2.y = 0;
	game->players.player2.pixelX = 105;
	game->players.player2.pixelY = 3;
	game->players.player2.tempX = 3;
	game->players.player2.tempY = 0;
	game->players.player2.tempPixelX= 105;
	game->players.player2.tempPixelY = 3;
	game->board.cells[game->players.player2.x][game->players.player2.y].type = PLAYER2;
	
	//Drawing token players
	LCD_DrawArray(player1_draw, 30, 30, 105, 207);
	LCD_DrawArray(player2_draw, 30, 30, 105, 3);
	
	availablePlayerCells(game);
	/*
	LCD_DrawArray(highlighted_cell, 30, 30, 139, 3);
	LCD_DrawArray(highlighted_cell, 30, 30, 71, 3);
	LCD_DrawArray(highlighted_cell, 30, 30, 105, 37);
	
	
	LCD_DrawArray(highlighted_cell, 30, 30, 139, 207);
	LCD_DrawArray(highlighted_cell, 30, 30, 71, 207);
	LCD_DrawArray(highlighted_cell, 30, 30, 105, 173);
	*/
}

void conferm_player_move(GameStatus *game){
	int byPassed;
	if(game->currentPlayer == 1){
		if((game->players.player1.x != game->players.player1.tempX) || (game->players.player1.y != game->players.player1.tempY)){
			if(game->players.player1.tempY == 0 && game->board.cells[game->players.player1.tempX][game->players.player1.tempY].type != PLAYER2){
				winner_player(game);
			} else {
				if((game->players.player1.tempX == game->players.player1.x) && (game->players.player1.tempY == game->players.player1.y-2)){
					byPassed = UP;
				}else if((game->players.player1.tempX == game->players.player1.x) && (game->players.player1.tempY == game->players.player1.y+2)){
					byPassed = DOWN;
				} else if((game->players.player1.tempX == game->players.player1.x-2) && (game->players.player1.tempY == game->players.player1.y)){
					byPassed = LEFT;
				} else if((game->players.player1.tempX == game->players.player1.x+2) && (game->players.player1.tempY == game->players.player1.y)){
					byPassed = RIGHT;
				}
				restore_available_player_cells(game, byPassed);
				
				set_new_board_player_position(game);
				game->players.player1.x = game->players.player1.tempX;
				game->players.player1.y = game->players.player1.tempY;
				
				game->players.player1.pixelX = game->players.player1.tempPixelX;
				game->players.player1.pixelY = game->players.player1.tempPixelY;
				
				game->players.player1.tempX = game->players.player1.x;
				game->players.player1.tempY = game->players.player1.y;
				game->players.player1.tempPixelX = game->players.player1.pixelX; 
				game->players.player1.tempPixelY = game->players.player1.pixelY; 
				
				draw_new_token_position(game,game->players.player1.tempPixelX, game->players.player1.tempPixelY);
				gameMove.PlayerID = 0;
				gameMove.PlayerMove_WallPlacement = 0;
				gameMove.Vertical_Horizontal = 0;
				gameMove.X = game->players.player1.x;
				gameMove.X = game->players.player1.y;
				save_move_into_variable();
				change_player_turn_after_confirm(game);
			}
		}
	} else {
		if((game->players.player2.x != game->players.player2.tempX) || (game->players.player2.y != game->players.player2.tempY)){
			if(game->players.player2.tempY == BOARD_SIZE-1 && game->board.cells[game->players.player2.tempX][game->players.player2.tempY].type != PLAYER1){
				winner_player(game);
			} else {
				if((game->players.player2.tempX == game->players.player2.x) && (game->players.player2.tempY == game->players.player2.y-2)){
					byPassed = UP;
				}else if((game->players.player2.tempX == game->players.player2.x) && (game->players.player2.tempY == game->players.player2.y+2)){
					byPassed = DOWN;
				} else if((game->players.player2.tempX == game->players.player2.x-2) && (game->players.player2.tempY == game->players.player2.y)){
					byPassed = LEFT;
				} else if((game->players.player2.tempX == game->players.player2.x+2) && (game->players.player2.tempY == game->players.player2.y)){
					byPassed = RIGHT;
				}
				restore_available_player_cells(game, byPassed);
				set_new_board_player_position(game);
				game->players.player2.x = game->players.player2.tempX;
				game->players.player2.y = game->players.player2.tempY;
				game->players.player2.pixelX = game->players.player2.tempPixelX;
				game->players.player2.pixelY = game->players.player2.tempPixelY;
				
				game->players.player2.tempX = game->players.player2.x;
				game->players.player2.tempY = game->players.player2.y;
				game->players.player2.tempPixelX = game->players.player2.pixelX; 
				game->players.player2.tempPixelY = game->players.player2.pixelY; 
				
				draw_new_token_position(game,game->players.player2.tempPixelX, game->players.player2.tempPixelY);
				gameMove.PlayerID = 1;
				gameMove.PlayerMove_WallPlacement = 0;
				gameMove.Vertical_Horizontal = 0;
				gameMove.X = game->players.player2.x;
				gameMove.X = game->players.player2.y;
				save_move_into_variable();
				change_player_turn_after_confirm(game);
			}
		}
	}
}

void starting_player(GameStatus *game){
	//srand(time(NULL));
	//game->currentPlayer = rand() % 2 + 1;
	game->currentPlayer = 2;
}

void initialize_game(GameStatus *game){
	int i,j;
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			game->board.cells[i][j].type = EMPTY;
		}
	}
	for(i = 0; i < BOARD_SIZE; i++){
		for(j=0; j< BOARD_SIZE; j++){
			game->walls.walls[i][j].type = NO_WALL;
		}
	}
	game->gameMode = MOVE_MODE;
	game->players.player1.walls = 8;
	game->players.player2.walls = 8;
	starting_player(game);
	set_initial_player_positions(game);
	write_remaining_walls_player1(game);
	write_remaining_walls_player2(game);
	game->rountTimer = 20;
	//enable_timer(0);

}

void set_temp_cordinates_player(GameStatus *game, int x, int y, int pixelX, int pixelY){
	if (game->currentPlayer == 1){
		game->players.player1.tempX = x;
		game->players.player1.tempPixelX = pixelX;
		game->players.player1.tempY = y;
		game->players.player1.tempPixelY = pixelY;
	}else{
		game->players.player2.tempX = x;
		game->players.player2.tempPixelX = pixelX;
		game->players.player2.tempY = y;
		game->players.player2.tempPixelY = pixelY;
	}
}

void preview_move_token (GameStatus *game, int direction){
	int currentX;
	int currentY;
	int currentPixelX;
	int currentPixelY;
	int cellUp;
	int cellDown;
	int cellLeft;
	int cellRight;
	
	if(game->currentPlayer == 1){
		currentX = game->players.player1.x;
		currentY = game->players.player1.y;
		currentPixelX = game->players.player1.pixelX;
		currentPixelY = game->players.player1.pixelY;
		
			switch (direction) {
        case UP:
					cellUp = is_cell_free(game,currentX,currentY-1,1,0);
					if(cellUp == EMPTY){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX, currentPixelY-34);
						set_temp_cordinates_player(game, currentX, currentY-1, currentPixelX, currentPixelY-34);
					}else if(cellUp == ANOTHER_PLAYER){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX, currentPixelY-68);
						set_temp_cordinates_player(game, currentX, currentY-2, currentPixelX, currentPixelY-68);
					}
					break;
        case DOWN:
					cellDown = is_cell_free(game,currentX,currentY+1,1,1);
					if(cellDown == EMPTY){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX, currentPixelY+34);
						set_temp_cordinates_player(game, currentX, currentY+1, currentPixelX, currentPixelY+34);
					}else if(cellDown == ANOTHER_PLAYER){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX, currentPixelY+68);
						set_temp_cordinates_player(game, currentX, currentY+2, currentPixelX, currentPixelY+68);
					}
					break;
        case LEFT:
					cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
					if(cellLeft == EMPTY){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX-34, currentPixelY);
						set_temp_cordinates_player(game, currentX-1, currentY, currentPixelX-34, currentPixelY);
					}else if(cellLeft == ANOTHER_PLAYER){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX-68, currentPixelY);
						set_temp_cordinates_player(game, currentX-2, currentY, currentPixelX-68, currentPixelY);
					}
					break;
        case RIGHT:
					cellRight = is_cell_free(game,currentX+1,currentY,0,1); //CONTROLLARE
					if(cellRight == EMPTY){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX+34, currentPixelY);
						set_temp_cordinates_player(game, currentX+1, currentY, currentPixelX+34, currentPixelY);
					}else if(cellRight == ANOTHER_PLAYER){
						cancel_actual_token_position(game);
						availablePlayerCells(game);
						draw_new_token_position(game, currentPixelX+68, currentPixelY);
						set_temp_cordinates_player(game, currentX+2, currentY, currentPixelX+68, currentPixelY);
					}
					break;
        default:
					break;
    }
		
	}else{
		currentX = game->players.player2.x;
		currentY = game->players.player2.y;
		currentPixelX = game->players.player2.pixelX;
		currentPixelY = game->players.player2.pixelY;
		
		switch (direction) {
			case UP:
				cellUp = is_cell_free(game,currentX,currentY-1,1,0);
				if(cellUp == EMPTY){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX, currentPixelY-34);
					set_temp_cordinates_player(game, currentX, currentY-1, currentPixelX, currentPixelY-34);
				}else if(cellUp == ANOTHER_PLAYER){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX, currentPixelY-68);
					set_temp_cordinates_player(game, currentX, currentY-2, currentPixelX, currentPixelY-68);
				}
				break;
			case DOWN:
				cellDown = is_cell_free(game,currentX,currentY+1,1,1);
				if(cellDown == EMPTY){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX, currentPixelY+34);
					set_temp_cordinates_player(game, currentX, currentY+1, currentPixelX, currentPixelY+34);
				}else if(cellDown == ANOTHER_PLAYER){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX, currentPixelY+68);
					set_temp_cordinates_player(game, currentX, currentY+2, currentPixelX, currentPixelY+68);
				}
				break;
			case LEFT:
				cellLeft = is_cell_free(game,currentX-1,currentY,0,0);
				if(cellLeft == EMPTY){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX-34, currentPixelY);
					set_temp_cordinates_player(game, currentX-1, currentY, currentPixelX-34, currentPixelY);
				}else if(cellLeft == ANOTHER_PLAYER){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX-68, currentPixelY);
					set_temp_cordinates_player(game, currentX-2, currentY, currentPixelX-68, currentPixelY);
				}
				break;
			case RIGHT:
				cellRight = is_cell_free(game,currentX+1,currentY,0,1); //CONTROLLARE
				if(cellRight == EMPTY){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX+34, currentPixelY);
					set_temp_cordinates_player(game, currentX+1, currentY, currentPixelX+34, currentPixelY);
				}else if(cellRight == ANOTHER_PLAYER){
					cancel_actual_token_position(game);
					availablePlayerCells(game);
					draw_new_token_position(game, currentPixelX+68, currentPixelY);
					set_temp_cordinates_player(game, currentX+2, currentY, currentPixelX+68, currentPixelY);
				}
				break;
			default:
				break;
    }
	}
}


void winner_player(GameStatus *game){
	if(game->currentPlayer == 1) {
		GUI_Text(70, 242,(uint8_t *) "Player 1 win", Green, White );
	}else {
		GUI_Text(70, 242,(uint8_t *) "Player 2 win", Green, White );
	}
	
	//TODO stop the program
}


void change_game_mode(GameStatus *game){
	//Passo da move a modalità WALL
	if(game->gameMode == MOVE_MODE){
		if(game->currentPlayer == 1){
			if(game->players.player1.walls > 0){
				restore_available_player_cells_no_confirmation(game, 0);
				draw_new_token_position(game, game->players.player1.pixelX, game->players.player1.pixelY);
				game->players.player1.tempX = game->players.player1.x;
				game->players.player1.tempPixelX = game->players.player1.pixelX;
				game->players.player1.tempY = game->players.player1.y;
				game->players.player1.tempPixelY = game->players.player1.pixelY;
				game->gameMode = WALLS_MODE;
				walls_mode(game);
			} else {	
				print_value_on_screen("No walls available, move the token",5,242);
			}
		} else {
			if(game->players.player2.walls > 0){
				restore_available_player_cells_no_confirmation(game, 0);
				draw_new_token_position(game, game->players.player2.pixelX, game->players.player2.pixelY);
				game->players.player2.tempX = game->players.player2.x;
				game->players.player2.tempPixelX = game->players.player2.pixelX;
				game->players.player2.tempY = game->players.player2.y;
				game->players.player2.tempPixelY = game->players.player2.pixelY;
				game->gameMode = WALLS_MODE;
				walls_mode(game);
			} else {
				print_value_on_screen("No walls available, move the token",5,242);
			}
		}
	} else {	//Passo da wall a modalità move
		if (game->walls.wallVerse == HORIZONTAL_WALL)
				restore_horizontal_wall_movement(game,0);
		else
				restore_vertical_wall_movement(game,0);
		game->gameMode = MOVE_MODE;
		availablePlayerCells(game);
	}
}

void decode_move(void){
	gameMove.PlayerID = (move >> 24);
	gameMove.PlayerMove_WallPlacement = (move >> 20) & 0x0000000F;
	gameMove.Vertical_Horizontal = (move >> 16) & 0x0000000F;
	gameMove.Y = (move >> 8) & 0x000000FF;
	gameMove.X = move & 0x000000FF;
}

void save_move_into_variable(void){
	move = ((gameMove.PlayerID) << 24) + ((gameMove.PlayerMove_WallPlacement) << 20) + ((gameMove.Vertical_Horizontal) << 16) + ((gameMove.Y) << 8) + ((gameMove.X) << 0);
}


/********* WALLS FUNCTIONS *********/
void rotate_wall(GameStatus *game){
	//Da orizzontale a verticale
	if (game->walls.wallVerse == HORIZONTAL_WALL){
		//controllo che non ruoto sopra a un muro posizionato
		/*
		if(game->walls.walls[game->walls.tempX][game->walls.tempY].type % 2 == 0 && game->walls.walls[game->walls.tempX+1][game->walls.tempY].type % 2 == 0){
			restore_wall(game, game->walls.tempPixelX,game->walls.tempPixelY);
			game->walls.wallVerse = VERTICAL_WALL;
			draw_wall_preview(game, game->walls.tempPixelX+30,game->walls.tempPixelY-30);
			game->walls.tempPixelX = game->walls.tempPixelX+30;
			game->walls.tempPixelY = game->walls.tempPixelY-30;
			game->walls.tempX = game->walls.tempX+1;
			game->walls.tempY = game->walls.tempY-1;
		} else {
			restore_empty_wall(game, game->walls.tempPixelX,game->walls.tempPixelY);
			game->walls.wallVerse = VERTICAL_WALL;
			draw_wall_preview(game, game->walls.tempPixelX+30,game->walls.tempPixelY-30);
			game->walls.tempPixelX = game->walls.tempPixelX+30;
			game->walls.tempPixelY = game->walls.tempPixelY-30;
			game->walls.tempX = game->walls.tempX+1;
			game->walls.tempY = game->walls.tempY-1;
		}*/	
			restore_horizontal_wall_movement(game, 0);
			game->walls.wallVerse = VERTICAL_WALL;
			draw_wall_preview(game, game->walls.tempPixelX+30,game->walls.tempPixelY-30);
			game->walls.tempPixelX = game->walls.tempPixelX+30;
			game->walls.tempPixelY = game->walls.tempPixelY-30;
			game->walls.tempX = game->walls.tempX+1;
			game->walls.tempY = game->walls.tempY-1;
	}else{	//Da verticale a orizzontale
		/*
		if(game->walls.walls[game->walls.tempX][game->walls.tempY].type % 5 == 0 && game->walls.walls[game->walls.tempX][game->walls.tempY+1].type % 5 == 0){
			restore_wall(game, game->walls.tempPixelX,game->walls.tempPixelY);
			game->walls.wallVerse = HORIZONTAL_WALL;
			draw_wall_preview(game, game->walls.tempPixelX-30,game->walls.tempPixelY+30);
			game->walls.tempPixelX = game->walls.tempPixelX-30;
			game->walls.tempPixelY = game->walls.tempPixelY+30;
			game->walls.tempX = game->walls.tempX-1;
			game->walls.tempY = game->walls.tempY+1;
		}else {
			restore_empty_wall(game, game->walls.tempPixelX,game->walls.tempPixelY);
			game->walls.wallVerse = HORIZONTAL_WALL;
			draw_wall_preview(game, game->walls.tempPixelX-30,game->walls.tempPixelY+30);
			game->walls.tempPixelX = game->walls.tempPixelX-30;
			game->walls.tempPixelY = game->walls.tempPixelY+30;
			game->walls.tempX = game->walls.tempX-1;
			game->walls.tempY = game->walls.tempY+1;
		}*/
		restore_vertical_wall_movement(game,0);
		game->walls.wallVerse = HORIZONTAL_WALL;
		draw_wall_preview(game, game->walls.tempPixelX-30,game->walls.tempPixelY+30);
		game->walls.tempPixelX = game->walls.tempPixelX-30;
		game->walls.tempPixelY = game->walls.tempPixelY+30;
		game->walls.tempX = game->walls.tempX-1;
		game->walls.tempY = game->walls.tempY+1;
	}
}

void spawn_center_wall(GameStatus *game){
	LCD_DrawArray(preview_horizontal_wall,4, 64, game->walls.tempPixelX, game->walls.tempPixelY);
}

void draw_wall(GameStatus *game, int x, int y){
	if(game->walls.wallVerse == HORIZONTAL_WALL){
		LCD_DrawArray(horizontal_wall,4, 64, x,y);
	} else {
		LCD_DrawVerticalArray(vertical_wall,64, 4, x, y);
	}
}

void draw_wall_preview(GameStatus *game, int x, int y){
	if(game->walls.wallVerse == HORIZONTAL_WALL){
		LCD_DrawArray(preview_horizontal_wall,4, 64, x, y);
	} else {
		LCD_DrawVerticalArray(preview_vertical_wall,64, 4, x, y);
	}
}

void restore_wall(GameStatus *game, int x, int  y){
	if(game->walls.wallVerse == HORIZONTAL_WALL){
		LCD_DrawArray(horizontal_wall,4, 64, x, y);
	} else {
		LCD_DrawVerticalArray(vertical_wall,64, 4, x, y);
	}
}

void restore_empty_wall(GameStatus *game, int x, int  y){
	if(game->walls.wallVerse == HORIZONTAL_WALL){
		LCD_DrawArray(empty_horizontal_wall,4, 64, x, y);
	} else {
		LCD_DrawVerticalArray(empty_vertical_wall,64, 4, x, y);
	}
}

void walls_mode(GameStatus *game){	
	game->walls.tempX = 3;
	game->walls.tempY = 3;
	game->walls.tempPixelX = game->walls.tempX*34+3;
	game->walls.tempPixelY = game->walls.tempY*34-1;
	game->walls.wallVerse = HORIZONTAL_WALL;
	spawn_center_wall(game);
}

void set_temp_cordinates_wall(GameStatus *game, int tempX, int tempY, int tempPixelX, int tempPixelY, int direction){
	game->walls.tempX = tempX;
	game->walls.tempY = tempY;
	game->walls.tempPixelX = tempPixelX;
	game->walls.tempPixelY = tempPixelY;
	game->walls.tempCellDirection = direction;
}	



int check_wall_presence(GameStatus *game, int x, int y, int direction){		
	if(game->walls.wallVerse == HORIZONTAL_WALL){
		// Coordinate non valide
		if (x < 0 || x >= BOARD_SIZE-1 || y < 0 || y >= BOARD_SIZE)
			return INVALID_MOVE;
			
		if(direction == UP){
			if((y != 0 && x >= 0) || (y != 0 && x < BOARD_SIZE)){
				if(((game->walls.walls[x][y].type % 7) != 0) && ((game->walls.walls[x+1][y].type % 7) != 0) && ((game->walls.walls[x][y].type % 3) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			}	else {
				return INVALID_MOVE;
			}
		} else if (direction == DOWN){
			if(y < BOARD_SIZE && x >= 0 && x < BOARD_SIZE){
				if(((game->walls.walls[x][y].type % 2) != 0) && ((game->walls.walls[x+1][y].type % 2) != 0) && ((game->walls.walls[x][y].type % 3) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			} else {
				return INVALID_MOVE;
			}
		} else if (direction == LEFT){
			if((x >= 0 && y < BOARD_SIZE) || (x >= 0 && y >= 0)){
				if((game->walls.walls[x][y].type % 2) != 0 && ((game->walls.walls[x-1][y].type % 2) != 0) && ((game->walls.walls[x][y].type % 3) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			} else {
				return INVALID_MOVE;
			}
		} else if (direction == RIGHT) {
			if((x < BOARD_SIZE && y < BOARD_SIZE) || (x < BOARD_SIZE && y >= 0)){
				if((game->walls.walls[x][y].type % 2) != 0 && ((game->walls.walls[x+1][y].type % 2) != 0) && ((game->walls.walls[x][y].type % 3) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			} else {
				return INVALID_MOVE;
			}
		}
	} else { //VERTICAL DIRECTION
		// Coordinate non valide		
		if (x <= 0 || x > BOARD_SIZE-1 || y < 0 || y >= BOARD_SIZE-1)
			return INVALID_MOVE;
		if(direction == UP){
			if((y >= 0 && x > 0) || (y >= 0 && x < BOARD_SIZE)){
				if(((game->walls.walls[x][y].type % 5) != 0) && ((game->walls.walls[x][y+1].type % 5) != 0) && ((game->walls.walls[x][y].type % 7) != 0) && ((game->walls.walls[x-1][y].type % 7) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			}	else {
				return INVALID_MOVE;
			}
		} else if (direction == DOWN){
			if((y < BOARD_SIZE && x > 0) || (y < BOARD_SIZE && x < BOARD_SIZE)){
				if(((game->walls.walls[x][y].type % 5) != 0) && ((game->walls.walls[x][y+1].type % 5) != 0) && ((game->walls.walls[x][y].type % 7) != 0) && ((game->walls.walls[x-1][y].type % 7) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			} else {
				return INVALID_MOVE;
			}
		} else if (direction == LEFT){
			if((x > 0 && y < BOARD_SIZE) || (x > 0 && y >= 0)){
				if((game->walls.walls[x][y].type % 5) != 0 && ((game->walls.walls[x][y+1].type % 5) != 0) && ((game->walls.walls[x][y].type % 7) != 0) && ((game->walls.walls[x-1][y].type % 7) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			} else {
				return INVALID_MOVE;
			}
		} else if (direction == RIGHT) {
			if((x < BOARD_SIZE && y < BOARD_SIZE) || (x < BOARD_SIZE && y >= 0)){
				if((game->walls.walls[x][y].type % 5) != 0 && ((game->walls.walls[x][y+1].type % 5) != 0) && ((game->walls.walls[x][y].type % 7) != 0) && ((game->walls.walls[x-1][y].type % 7) != 0))
					return NO_WALL;
				else 
					return WALL_ALREADY_PRESENT;
			} else {
				return INVALID_MOVE;
			}
		}
	}
}

int confirm_move_wall(GameStatus *game){
	int i;
	char str2[20];
	bool player1CanWin;
	bool player2CanWin;
	if(game->currentPlayer == 1){
		gameMove.PlayerID = 0;
	}else{ 
		gameMove.PlayerID = 1;
	}
	gameMove.PlayerMove_WallPlacement = 1;
	
	
	if(game->walls.wallVerse == HORIZONTAL_WALL){
		if((((game->walls.walls[game->walls.tempX][game->walls.tempY].type % 2) == 0) || ((game->walls.walls[game->walls.tempX+1][game->walls.tempY].type % 2) == 0)) || (((game->walls.walls[game->walls.tempX][game->walls.tempY].type % 3) == 0) && ((game->walls.walls[game->walls.tempX][game->walls.tempY-1].type % 3)== 0))){
			print_value_on_screen("Wall already present",40,242);
			for(i=0; i < 3000; i++){
			}
			cancel_value_on_screen("Wall already present",40,242);
		} else{
			
			game->walls.walls[game->walls.tempX][game->walls.tempY].type = game->walls.walls[game->walls.tempX][game->walls.tempY].type * WALL_TOP;
			game->walls.walls[game->walls.tempX][game->walls.tempY-1].type = game->walls.walls[game->walls.tempX][game->walls.tempY-1].type * WALL_BOTTOM;
			game->walls.walls[game->walls.tempX+1][game->walls.tempY].type = game->walls.walls[game->walls.tempX+1][game->walls.tempY].type * WALL_TOP;
			game->walls.walls[game->walls.tempX+1][game->walls.tempY-1].type = game->walls.walls[game->walls.tempX+1][game->walls.tempY-1].type * WALL_BOTTOM;
			
			player1CanWin = can_player_reach_opposite_border(game, 1, game->players.player1.x, game->players.player1.y);
			player2CanWin = can_player_reach_opposite_border(game, 2, game->players.player2.x, game->players.player2.y);
			
			if(player1CanWin && player2CanWin){
				draw_wall(game, game->walls.tempPixelX, game->walls.tempPixelY);
				gameMove.Vertical_Horizontal = 1;
				gameMove.X = game->walls.tempX;
				gameMove.Y = game->walls.tempY;
				save_move_into_variable();
				change_player_turn_after_confirm(game);
			} else {
				game->walls.walls[game->walls.tempX][game->walls.tempY].type = game->walls.walls[game->walls.tempX][game->walls.tempY].type / WALL_TOP;
				game->walls.walls[game->walls.tempX][game->walls.tempY-1].type = game->walls.walls[game->walls.tempX][game->walls.tempY-1].type / WALL_BOTTOM;
				game->walls.walls[game->walls.tempX+1][game->walls.tempY].type = game->walls.walls[game->walls.tempX+1][game->walls.tempY].type / WALL_TOP;
				game->walls.walls[game->walls.tempX+1][game->walls.tempY-1].type = game->walls.walls[game->walls.tempX+1][game->walls.tempY-1].type / WALL_BOTTOM;
			}
			
			
		}
	} else { // CASO MURO VERTICALE
		if((((game->walls.walls[game->walls.tempX][game->walls.tempY].type % 5) == 0) || ((game->walls.walls[game->walls.tempX][game->walls.tempY+1].type % 5) == 0)) || (((game->walls.walls[game->walls.tempX][game->walls.tempY].type % 7) == 0) && ((game->walls.walls[game->walls.tempX-1][game->walls.tempY].type % 7)== 0))){
			print_value_on_screen("Wall already present",40,242);
			for(i=0; i < 3000; i++){
			}
			cancel_value_on_screen("Wall already present",40,242);
		} else{
			game->walls.walls[game->walls.tempX][game->walls.tempY].type = game->walls.walls[game->walls.tempX][game->walls.tempY].type * WALL_LEFT;
			game->walls.walls[game->walls.tempX-1][game->walls.tempY].type = game->walls.walls[game->walls.tempX-1][game->walls.tempY].type * WALL_RIGHT;
			game->walls.walls[game->walls.tempX][game->walls.tempY+1].type = game->walls.walls[game->walls.tempX][game->walls.tempY+1].type * WALL_LEFT;
			game->walls.walls[game->walls.tempX-1][game->walls.tempY+1].type = game->walls.walls[game->walls.tempX-1][game->walls.tempY+1].type * WALL_RIGHT;
			
			player1CanWin = can_player_reach_opposite_border(game, 1, game->players.player1.x, game->players.player1.y);
			player2CanWin = can_player_reach_opposite_border(game, 2, game->players.player2.x, game->players.player2.y);
			
			if(player1CanWin && player2CanWin){
				draw_wall(game, game->walls.tempPixelX, game->walls.tempPixelY);
				gameMove.Vertical_Horizontal = 0;
				gameMove.X = game->walls.tempX;
				gameMove.Y = game->walls.tempY;
				save_move_into_variable();
				change_player_turn_after_confirm(game);
			} else {
			game->walls.walls[game->walls.tempX][game->walls.tempY].type = game->walls.walls[game->walls.tempX][game->walls.tempY].type / WALL_LEFT;
			game->walls.walls[game->walls.tempX-1][game->walls.tempY].type = game->walls.walls[game->walls.tempX-1][game->walls.tempY].type / WALL_RIGHT;
			game->walls.walls[game->walls.tempX][game->walls.tempY+1].type = game->walls.walls[game->walls.tempX][game->walls.tempY+1].type / WALL_LEFT;
			game->walls.walls[game->walls.tempX-1][game->walls.tempY+1].type = game->walls.walls[game->walls.tempX-1][game->walls.tempY+1].type / WALL_RIGHT;				
			}
		}
	}
}

void move_preview_horizontal_wall(GameStatus *game, int direction){
	int currentWallX = game->walls.tempX;
	int	currentWallY = game->walls.tempY;
	int currentWallPixelX = game->walls.tempPixelX;
	int	currentWallPixelY = game->walls.tempPixelY;
	int cellUp;
	int cellDown;
	int cellLeft;
	int cellRight;
	
	switch (direction) {
		case UP:
			cellUp = check_wall_presence(game, currentWallX, currentWallY-1, UP);
			restore_horizontal_wall_movement(game, UP);
			if(cellUp == NO_WALL){
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY-34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY-1, currentWallPixelX, currentWallPixelY-34, UP);
			} else if(cellUp == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY-34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY-1, currentWallPixelX, currentWallPixelY-34, UP);
			}
			break;
		case DOWN:
			cellDown = check_wall_presence(game, currentWallX, currentWallY+1, DOWN);
			restore_horizontal_wall_movement(game, DOWN);
			if(cellDown == NO_WALL){
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY+34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY+1, currentWallPixelX, currentWallPixelY+34, DOWN);
			} else if(cellDown == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY+34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY+1, currentWallPixelX, currentWallPixelY+34, DOWN);
			}
			break;
		case LEFT:
			cellLeft = check_wall_presence(game, currentWallX-1, currentWallY, LEFT);
			restore_horizontal_wall_movement(game, LEFT);
			if(cellLeft == NO_WALL){
				draw_wall_preview(game, currentWallPixelX-34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX-1, currentWallY, currentWallPixelX-34, currentWallPixelY, LEFT);
			} else if(cellLeft == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX-34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX-1, currentWallY, currentWallPixelX-34, currentWallPixelY, LEFT);
			}
			break;
		case RIGHT:
			cellRight = check_wall_presence(game, currentWallX+1, currentWallY, RIGHT);
			restore_horizontal_wall_movement(game, RIGHT);
			if(cellRight == NO_WALL){
				draw_wall_preview(game, currentWallPixelX+34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX+1, currentWallY, currentWallPixelX+34, currentWallPixelY, RIGHT);
			} else if(cellRight == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX+34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX+1, currentWallY, currentWallPixelX+34, currentWallPixelY, RIGHT);
			}
			break;
		default:
			break;
	}
}

void move_preview_vertical_wall(GameStatus *game, int direction){
	int currentWallX = game->walls.tempX;
	int	currentWallY = game->walls.tempY;
	int currentWallPixelX = game->walls.tempPixelX;
	int	currentWallPixelY = game->walls.tempPixelY;
	int cellUp;
	int cellDown;
	int cellLeft;
	int cellRight;
	
	switch (direction) {
		case UP:
			cellUp = check_wall_presence(game, currentWallX, currentWallY-1, UP);
			restore_vertical_wall_movement(game, UP);
			if(cellUp == NO_WALL){
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY-34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY-1, currentWallPixelX, currentWallPixelY-34, UP);
			} else if(cellUp == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY-34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY-1, currentWallPixelX, currentWallPixelY-34, UP);
			}
			break;
		case DOWN:
			cellDown = check_wall_presence(game, currentWallX, currentWallY+1, DOWN);
			restore_vertical_wall_movement(game, DOWN);

			if(cellDown == NO_WALL){
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY+34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY+1, currentWallPixelX, currentWallPixelY+34, DOWN);
			} else if(cellDown == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX, currentWallPixelY+34);
				set_temp_cordinates_wall(game, currentWallX, currentWallY+1, currentWallPixelX, currentWallPixelY+34, DOWN);
			}
			break;
		case LEFT:
			cellLeft = check_wall_presence(game, currentWallX-1, currentWallY, LEFT);
			restore_vertical_wall_movement(game, LEFT);

			if(cellLeft == NO_WALL){
				draw_wall_preview(game, currentWallPixelX-34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX-1, currentWallY, currentWallPixelX-34, currentWallPixelY, LEFT);
			} else if(cellLeft == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX-34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX-1, currentWallY, currentWallPixelX-34, currentWallPixelY, LEFT);
			}
			break;
		case RIGHT:
			cellRight = check_wall_presence(game, currentWallX+1, currentWallY, RIGHT);
			restore_vertical_wall_movement(game, RIGHT);

			if(cellRight == NO_WALL){
				draw_wall_preview(game, currentWallPixelX+34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX+1, currentWallY, currentWallPixelX+34, currentWallPixelY, RIGHT);
			} else if(cellRight == WALL_ALREADY_PRESENT) {
				draw_wall_preview(game, currentWallPixelX+34, currentWallPixelY);
				set_temp_cordinates_wall(game, currentWallX+1, currentWallY, currentWallPixelX+34, currentWallPixelY, RIGHT);
			}
			break;
		default:
			break;
	}
}


void restore_horizontal_wall_movement(GameStatus *game, int direction){
	int currentWallX = game->walls.tempX;
	int	currentWallY = game->walls.tempY;
	int currentWallPixelX = game->walls.tempPixelX;
	int	currentWallPixelY = game->walls.tempPixelY;
	int previousWall = 0;
	int i;
	if ((direction == UP && game->walls.tempY-1 > 0) || (direction == DOWN && game->walls.tempY+1 < BOARD_SIZE) || (direction == LEFT && game->walls.tempX-1 >= 0) || (direction == RIGHT && game->walls.tempX+2 < BOARD_SIZE) || direction == 0){
		if(game->walls.walls[currentWallX][currentWallY].type % 2 != 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2){
			if (game->walls.walls[currentWallX][currentWallY].type % 3 == 0 && game->walls.walls[currentWallX][currentWallY-1].type % 3 == 0){
				for(i=0;i<currentWallY-1;i++){
					if(game->walls.walls[currentWallX][i].type % 3 == 0)
						previousWall++;
				}
				if(previousWall % 2 == 0)
					LCD_DrawArray(restore_central_space, 4, 64,  currentWallPixelX, currentWallPixelY);
				else
					LCD_DrawArray(empty_horizontal_wall, 4, 64, currentWallPixelX, currentWallPixelY);
			}else{
				LCD_DrawArray(empty_horizontal_wall, 4, 64, currentWallPixelX, currentWallPixelY);
			}
		} else if (game->walls.walls[currentWallX][currentWallY].type % 2 == 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2 != 0){
				LCD_DrawArray(restore_left_wall,4, 64, currentWallPixelX, currentWallPixelY);
		} else if (game->walls.walls[currentWallX][currentWallY].type % 2 != 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2 == 0){
				LCD_DrawArray(restore_right_wall,4, 64, currentWallPixelX, currentWallPixelY);	
		} else {
			for(i=0;i<currentWallX;i++){
					if(game->walls.walls[i][currentWallY].type % 2 == 0)
						previousWall++;
				}
			if(previousWall % 2 == 0)
				LCD_DrawArray(horizontal_wall,4, 64, currentWallPixelX, currentWallPixelY);
			else 
				LCD_DrawArray(restore_two_separate_walls,4, 64, currentWallPixelX, currentWallPixelY);
		}
		
		/*
		if ((direction == UP && game->walls.tempY-1 > 0) || (direction == DOWN && game->walls.tempY+1 < BOARD_SIZE) || (direction == LEFT && game->walls.tempX-1 >= 0) || (direction == RIGHT && game->walls.tempX+2 < BOARD_SIZE) || direction == 0){
			if(game->walls.walls[currentWallX][currentWallY].type % 2 == 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2 == 0){
				for(i=0;i<currentWallX;i++){
					if(game->walls.walls[i][currentWallY].type % 2 == 0)
						previousWall++;
				}
				if(previousWall % 2 == 0)
					LCD_DrawArray(horizontal_wall,4, 64, currentWallPixelX, currentWallPixelY);
				else 
					LCD_DrawArray(restore_two_separate_walls,4, 64, currentWallPixelX, currentWallPixelY);			
			} 
			if (game->walls.walls[currentWallX][currentWallY].type % 2 != 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2 != 0){
				LCD_DrawArray(empty_horizontal_wall,4, 64, currentWallPixelX, currentWallPixelY);
			} 		
			if (game->walls.walls[currentWallX][currentWallY].type % 2 == 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2 != 0){
				LCD_DrawArray(restore_left_wall,4, 64, currentWallPixelX, currentWallPixelY);
			}  
			if (game->walls.walls[currentWallX][currentWallY].type % 2 != 0 && game->walls.walls[currentWallX+1][currentWallY].type % 2 == 0){
				LCD_DrawArray(restore_right_wall,4, 64, currentWallPixelX, currentWallPixelY);	
			}
			if (game->walls.walls[currentWallX][currentWallY].type % 3 == 0 && game->walls.walls[currentWallX][currentWallY-1].type % 3 == 0){
				if(game->walls.walls[currentWallX][currentWallY].type % 2 == 0){
					LCD_DrawArray(restore_left_wall,4, 64, currentWallPixelX, currentWallPixelY);
					LCD_DrawVerticalArray(vertical_wall,64, 4, currentWallPixelX+30, currentWallPixelY-30);
				} else if(game->walls.walls[currentWallX+1][currentWallY].type % 2 == 0){
					LCD_DrawArray(restore_right_wall,4, 64, currentWallPixelX, currentWallPixelY);
					LCD_DrawVerticalArray(vertical_wall,64, 4, currentWallPixelX+30, currentWallPixelY-30);
				} else {
					LCD_DrawArray(empty_horizontal_wall,4, 64, currentWallPixelX, currentWallPixelY);
					LCD_DrawVerticalArray(vertical_wall,64, 4, currentWallPixelX+30, currentWallPixelY-30);
				}
			}  		
		}
		*/
		}
}
//TODO: fixare i casi riportati su notion
void restore_vertical_wall_movement(GameStatus *game, int direction){
	int currentWallX = game->walls.tempX;
	int	currentWallY = game->walls.tempY;
	int currentWallPixelX = game->walls.tempPixelX;
	int	currentWallPixelY = game->walls.tempPixelY;
	int previousWall = 0;
	int i;
	
	if((direction == UP && game->walls.tempY > 0) || (direction == DOWN && game->walls.tempY < BOARD_SIZE-2) || (direction == LEFT && game->walls.tempX > 1) || (direction == RIGHT && game->walls.tempX < BOARD_SIZE-1) || direction == 0){
		if(game->walls.walls[currentWallX][currentWallY].type % 5 != 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 != 0){
			if(game->walls.walls[currentWallX][currentWallY].type % 7 == 0 && game->walls.walls[currentWallX-1][currentWallY].type % 7 == 0){
				for(i=0;i<currentWallX-1;i++){
					if(game->walls.walls[i][currentWallY].type % 7 == 0)
						previousWall++;
				}
				if(previousWall % 2 == 0)
					LCD_DrawVerticalArray(restore_central_space,64, 4, currentWallPixelX, currentWallPixelY);
				else 
					LCD_DrawVerticalArray(empty_vertical_wall,64, 4, currentWallPixelX, currentWallPixelY);
			}else{
				LCD_DrawVerticalArray(empty_vertical_wall,64, 4, currentWallPixelX, currentWallPixelY);
			}
		} else if(game->walls.walls[currentWallX][currentWallY].type % 5 == 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 != 0){
				LCD_DrawVerticalArray(restore_left_wall,64, 4, currentWallPixelX, currentWallPixelY);
		} else if(game->walls.walls[currentWallX][currentWallY].type % 5 != 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 == 0){
				LCD_DrawVerticalArray(restore_right_wall,64, 4, currentWallPixelX, currentWallPixelY);	
		} else {
				for(i=0;i<currentWallY;i++){
					if(game->walls.walls[currentWallX][i].type % 5 == 0)
						previousWall++;
				}
				if(previousWall % 2 == 0)
					LCD_DrawVerticalArray(vertical_wall,64, 4, currentWallPixelX, currentWallPixelY);
				else 
					LCD_DrawVerticalArray(restore_two_separate_walls,64, 4, currentWallPixelX, currentWallPixelY);
		}
		
	/*
	if((direction == UP && game->walls.tempY > 0) || (direction == DOWN && game->walls.tempY < BOARD_SIZE-2) || (direction == LEFT && game->walls.tempX > 1) || (direction == RIGHT && game->walls.tempX < BOARD_SIZE-1) || direction == 0){
		if(game->walls.walls[currentWallX][currentWallY].type % 5 == 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 == 0){
			for(i=0;i<currentWallY;i++){
				if(game->walls.walls[currentWallX][i].type % 5 == 0)
					previousWall++;
			}
			if(previousWall % 2 == 0)
				LCD_DrawVerticalArray(vertical_wall,64, 4, currentWallPixelX, currentWallPixelY);
			else 
				LCD_DrawVerticalArray(restore_two_separate_walls,64, 4, currentWallPixelX, currentWallPixelY);
		}
		if (game->walls.walls[currentWallX][currentWallY].type % 5 != 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 != 0){
			LCD_DrawVerticalArray(empty_vertical_wall,64, 4, currentWallPixelX, currentWallPixelY);
		} 		

		if (game->walls.walls[currentWallX][currentWallY].type % 5 == 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 != 0){
			LCD_DrawVerticalArray(restore_left_wall,64, 4, currentWallPixelX, currentWallPixelY);
		} 
		if (game->walls.walls[currentWallX][currentWallY].type % 5 != 0 && game->walls.walls[currentWallX][currentWallY+1].type % 5 == 0){
			LCD_DrawVerticalArray(restore_right_wall,64, 4, currentWallPixelX, currentWallPixelY);	
		}
		if (game->walls.walls[currentWallX][currentWallY].type % 7 == 0 && game->walls.walls[currentWallX-1][currentWallY].type % 7 == 0){
			LCD_DrawArray(horizontal_wall,4, 64, currentWallPixelX-30, currentWallPixelY+30);
			if(game->walls.walls[currentWallX][currentWallY].type % 5 == 0){
				LCD_DrawVerticalArray(restore_left_wall,64, 4, currentWallPixelX, currentWallPixelY);
				//LCD_DrawArray(horizontal_wall,4, 64, currentWallPixelX-30, currentWallPixelY+30);
			}else if(game->walls.walls[currentWallX][currentWallY+1].type % 5 == 0){
				LCD_DrawVerticalArray(restore_right_wall,64, 4, currentWallPixelX, currentWallPixelY);
				//LCD_DrawArray(horizontal_wall,4, 64, currentWallPixelX-30, currentWallPixelY+30);			
			} else {
				LCD_DrawVerticalArray(empty_vertical_wall,64, 4, currentWallPixelX, currentWallPixelY);
				LCD_DrawArray(horizontal_wall,4, 64, currentWallPixelX-30, currentWallPixelY+30);
			}
		} 		
	}*/
	}
}


bool can_player_reach_opposite_border(GameStatus *game, int target, int x, int y){
	int i, j, cellUp, cellDown, cellLeft, cellRight;
	int board[BOARD_SIZE][BOARD_SIZE];
	bool flag = true;
	for ( i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if(i == x && j == y)
				board[i][j] = 1;
			else
				board[i][j] = 0;
		}	
	}
	while(flag){
		flag = false; 
		for (i = 0; i < BOARD_SIZE; i++){
			for (j = 0; j < BOARD_SIZE; j++){
				if(board[i][j] == 1){
					if(j == 0 && target == 1)
						return true;
					else if (j == BOARD_SIZE-1 && target == 2)
						return true;
					
					cellUp = is_cell_free(game,i,j-1,1,0);
					if((cellUp == EMPTY || cellUp == ANOTHER_PLAYER) && board[i][j-1] == 0){
						board[i][j-1] = 1;
						flag = true;
					}
					cellDown = is_cell_free(game,i,j+1,1,1);
					if((cellDown == EMPTY || cellDown == ANOTHER_PLAYER) && board[i][j+1] == 0){
						board[i][j+1] = 1;
						flag = true;
					}
					cellLeft = is_cell_free(game,i-1,j,0,0);
					if((cellLeft == EMPTY || cellLeft == ANOTHER_PLAYER) && board[i-1][j] == 0){
						board[i-1][j] = 1;
						flag = true;
					}
					cellRight = is_cell_free(game,i+1,j,0,1);
					if((cellRight == EMPTY || cellRight == ANOTHER_PLAYER) && board[i+1][j] == 0){
						board[i+1][j] = 1;
						flag = true;
					}
				}
			}	
		}
	}
	return false;
} 
