#include <stdio.h>
#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"

int walls_player_one = 8;
int walls_player_two = 8;

void draw_boardgame(void){
	int i,j,k;
	int rect_width = 70;
  int rect_height = 40;
	int spacing = 7;
	int start_x = 7;
	int start_y = 260;
	
	//Background
	LCD_Clear(Orange);
	
	//Draw matrix
	for(j=0; j<=198; j=j+33){
		for(i=0; i<=198; i=i+33){
			LCD_DrawLine(33+i, 4+j, 4+i, 4+j, Blue);
			LCD_DrawLine(33+i, 33+j, 4+i, 33+j, Blue);
			LCD_DrawLine(4+j, 33+i, 4+j, 4+i, Blue);
			LCD_DrawLine(33+j, 33+i, 33+j, 4+i, Blue);
		}
	}
	
	//Draw rectangles
	for (k = 0; k < 3; k++) {
			int x0 = start_x + k * (rect_width + spacing);
			int y0 = start_y;
			int x1 = x0 + rect_width;
			int y1 = y0 + rect_height;
			char str1[20];
			char str2[20];
		
			LCD_DrawLine(x0, y0, x1, y0, Blue);  // Linea superiore
			LCD_DrawLine(x0, y0, x0, y1, Blue);  // Linea sinistra
			LCD_DrawLine(x1, y0, x1, y1, Blue);  // Linea destra
			LCD_DrawLine(x0, y1, x1, y1, Blue);  // Linea inferiore
			if(k==0){
				GUI_Text(x0+7, y0+2, (uint8_t *) "P1 Wall",Black, White );
				sprintf(str1, "%d", walls_player_one);
				GUI_Text(40, 280, (uint8_t *) str1, Black, White );
			} else if (k==2){
				GUI_Text(x0+7, y0+2, (uint8_t *) "P2 Wall",Black, White );
				sprintf(str2, "%d", walls_player_two);
				GUI_Text(192, 280,(uint8_t *) str2, Black, White );
			}
	}
}

void write_remaining_walls_player1(){
	char str1[20];	
	walls_player_one--;
	sprintf(str1, "%d", walls_player_one);
	GUI_Text(40, 280, (uint8_t *) str1, Black, White );
}
void write_remaining_walls_player2(){
	char str2[20];
	walls_player_two--;
	sprintf(str2, "%d", walls_player_two);
	GUI_Text(192, 280,(uint8_t *) str2, Black, White );
}

	
