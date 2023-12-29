#include "LPC17xx.h"
#include "GLCD/GLCD.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "button_EXINT/button.h"
#include "joystick/joystick.h"

void draw_boardgame(void){
	int i,j;
	
	LCD_Clear(White);
	for(j=0; j<=198; j=j+33){
		for(i=0; i<=198; i=i+33){
			LCD_DrawLine(33+i, 4+j, 4+i, 4+j, Blue);
			LCD_DrawLine(33+i, 33+j, 4+i, 33+j, Blue);
			LCD_DrawLine(4+j, 33+i, 4+j, 4+i, Blue);
			LCD_DrawLine(33+j, 33+i, 33+j, 4+i, Blue);
		}
	}
}
