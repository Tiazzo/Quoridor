#include "button.h"
#include "lpc17xx.h"
#include "../game_constants.h"
#include "../game_functions.h"
#include "../game_structures.h"

int down_0;
int down_1;
int down_2;

void EINT0_IRQHandler (void)	  	/* INT0														 */
{
	down_0 = 1;
	//start_game();
	
	NVIC_DisableIRQ(EINT0_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 20);     /* GPIO pin selection */
	LPC_SC->EXTINT &= (1 << 0);     /* clear pending interrupt         */
}


void EINT1_IRQHandler (void)	  	/* KEY1														 */
{
	down_1=1;
	//change_game_mode(&game);
	
	NVIC_DisableIRQ(EINT1_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 22);     /* GPIO pin selection */
	LPC_SC->EXTINT &= (1 << 1);     /* clear pending interrupt         */
}

void EINT2_IRQHandler (void)	  	/* KEY2														 */
{
	down_2=1;
	/*
	if(game.gameMode == WALLS_MODE){
		rotate_wall(&game);
	}*/

	NVIC_DisableIRQ(EINT2_IRQn);		/* disable Button interrupts			 */
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     /* GPIO pin selection */
  LPC_SC->EXTINT &= (1 << 2);     /* clear pending interrupt         */    
}


