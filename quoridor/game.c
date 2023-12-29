#include <stdio.h>
#include <stdint.h>
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


void draw_boardgame(void){
	int i,j,k;
	int rect_width = 70;
  int rect_height = 40;
	int spacing = 7;
	int start_x = 7;
	int start_y = 260;
	
	//Background
	//LCD_Clear(Orange);

	
	//Draw matrix
	
	/*
	for(j=0; j<=198; j=j+33){
		for(i=0; i<=198; i=i+33){
			LCD_DrawLine(33+i, 4+j, 4+i, 4+j, Blue);
			LCD_DrawLine(33+i, 33+j, 4+i, 33+j, Blue);
			LCD_DrawLine(4+j, 33+i, 4+j, 4+i, Blue);
			LCD_DrawLine(33+j, 33+i, 33+j, 4+i, Blue);
		}
	}
	*/
	
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
	start_game();
}

void write_remaining_walls_player1(){
	char str1[20];
	if(firstTimePlayer1)
		firstTimePlayer1--;
	else
		game.players.player1.walls--;
	
	sprintf(str1, "%d", game.players.player1.walls);
	GUI_Text(40, 280, (uint8_t *) str1, Black, White );
}
void write_remaining_walls_player2(){
	char str2[20];
		if(firstTimePlayer2)
				firstTimePlayer2--;
	else
		game.players.player2.walls--;
	sprintf(str2, "%d", game.players.player2.walls--);
	GUI_Text(192, 280,(uint8_t *) str2, Black, White );
}

	
void start_game(){
	write_remaining_walls_player1();
	write_remaining_walls_player2();
	setInitialPlayerPositions(&game);
}

void setInitialPlayerPositions(GameStatus *game){
	game-> players.player1.x = 3;
	game-> players.player1.y = 6;
	
	game-> players.player2.x = 3;
	game-> players.player2.y = 0;
	
	game->board.cells[game->players.player1.x][game->players.player1.y].type = PLAYER1;
	game->board.cells[game->players.player2.x][game->players.player2.y].type = PLAYER2;
	
	LCD_DrawArray(player2_draw, 30, 30, 105, 3);
	LCD_DrawArray(player1_draw, 30, 30, 105, 207);
	
	LCD_DrawArray(highlighted_cell, 30, 30, 139, 3);
	LCD_DrawArray(highlighted_cell, 30, 30, 71, 3);
	LCD_DrawArray(highlighted_cell, 30, 30, 105, 37);
	
	
	LCD_DrawArray(highlighted_cell, 30, 30, 139, 207);
	LCD_DrawArray(highlighted_cell, 30, 30, 71, 207);
	LCD_DrawArray(highlighted_cell, 30, 30, 105, 173);
}
void initilize_game(GameStatus *game){
	int i,j;
	for(i = 0; i < BOARD_SIZE; i++){
		for(j = 0; j < BOARD_SIZE; j++){
			game->board.cells[i][j].type = EMPTY;
		}
	}
	game->players.player1.walls = 8;
	game->players.player2.walls = 8;
	setInitialPlayerPositions(game);
	
}

int isCellFree(GameStatus *game, int x, int y, int up, int right){
	if ((x >= 0 && x < BOARD_SIZE) && (y >= 0 && y < BOARD_SIZE)){
		if(game->board.cells[x][y].type == EMPTY){
			return EMPTY;
		} else if(game->board.cells[x][y].type == HORIZONTAL_WALL){
					return INVALID_MOVE;
		} else {	//another player
				if(up==1 && right==0){ //up
					if (x-1 >= 0 && x-1 < BOARD_SIZE){
						if(game->board.cells[x-1][y].type != HORIZONTAL_WALL)
							return ANOTHER_PLAYER;
					}else{
						return INVALID_MOVE;
					}
				}else if(up==1 && right==1){ //down
					if (x+1 >= 0 && x+1 < BOARD_SIZE){
						if(game->board.cells[x+1][y].type != HORIZONTAL_WALL)
							return ANOTHER_PLAYER;
					}else{
						return INVALID_MOVE;
					}
				}else if (up==0 && right==1){ //right
					if (y+1>= 0 && y+1 < BOARD_SIZE){
						if(game->board.cells[x][y+1].type != HORIZONTAL_WALL)
							return ANOTHER_PLAYER;
					}else{
						return INVALID_MOVE;
					}
				} else if(up==0 && right==0){	//left
						if (y-1 >= 0 && y-1 < BOARD_SIZE){
							if(game->board.cells[x][y-1].type != HORIZONTAL_WALL)
								return ANOTHER_PLAYER;
						}else{
							return INVALID_MOVE;
						}
				}					
		}
	}
	return INVALID_MOVE; //aggiunta per togliere un warnings	
}

//Da chiamare quando clicclo select 
void move_token_up(GameStatus *game){
	if(game->currentPlayer == 1){
		int currentX = game->players.player1.x;
		int currentY = game->players.player1.y;
		int newX = currentX - 1;
    int newY = currentY;

		int cellUp = isCellFree(game,newX, newY, 1, 0);
		
		if(cellUp == EMPTY){
			//Restore cell to empty 
			game->board.cells[currentX][currentY].type = EMPTY;
			game-> players.player1.x = newX;
			game-> players.player1.y = newY;
			//Update cell to Player1
			game->board.cells[game->players.player1.x][game->players.player1.y].type = PLAYER1;
			//TODO: Call token_drawing() function
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

		int cellUp = isCellFree(game,newX, newY,1,0);
		
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

void highlight_cell(int cellUp, int cellDown, int cellLeft, int cellRight){
	
}

void availablePlayerCell (GameStatus *game){
	int i;
	int currentX = game->players.player2.x;
	int currentY = game->players.player2.y;
	int newX = currentX;
	int newY = currentY - 1;
	int cellUp;
	int cellDown;
	int cellLeft;
	int cellRight;
	
	if(game->currentPlayer == 1){
		currentX = game->players.player1.x;
		currentY = game->players.player1.y;
		cellUp = isCellFree(game,currentX-1,currentY,1,0);
		cellDown = isCellFree(game,currentX+1,currentY,1,1);
		cellLeft = isCellFree(game,currentX,currentY-1,0,0);
		cellRight = isCellFree(game,currentX,currentY+1,0,1);
		highlight_cell(cellUp, cellDown, cellLeft, cellRight);
	} else {
		currentX = game->players.player2.x;
		currentY = game->players.player2.y;
		cellUp = isCellFree(game,currentX-1,currentY,1,0);
		cellDown = isCellFree(game,currentX+1,currentY,1,1);
		cellLeft = isCellFree(game,currentX,currentY-1,0,0);
		cellRight = isCellFree(game,currentX,currentY+1,0,1);
	}
	
	
}





