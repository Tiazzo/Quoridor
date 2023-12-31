/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../button_EXINT/button.h"
#include "../game_constants.h"
#include "../game_functions.h"
#include "../game_structures.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern char led_value;

void RIT_IRQHandler (void)
{					
	static int J_select=0;
	static int J_up = 0;
	static int J_right = 0;
	static int J_left = 0;
	static int J_down = 0;
		
	if((LPC_GPIO1->FIOPIN & (1<<29)) == 0){		
		J_right = 0;
		J_left = 0;
		J_down = 0;
		J_up++;
		switch(J_up){
			case 1:
				preview_move_token(&game, UP);
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<28)) == 0){	
		J_up = 0;
		J_left = 0;
		J_down = 0;
		J_right++;
		switch(J_right){
			case 1:
				preview_move_token(&game, RIGHT);
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<27)) == 0){	
		J_up = 0;
		J_right = 0;
		J_down = 0;
		J_left++;
		switch(J_left){
			case 1:
				preview_move_token(&game, LEFT);
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<26)) == 0){	
		/* Joytick J_Select pressed p1.25*/
		/* Joytick J_Down pressed p1.26 --> using J_DOWN due to emulator issues*/
		J_up = 0;
		J_right = 0;
		J_left = 0;
		J_down++;
		switch(J_down){
			case 1:
				preview_move_token(&game, DOWN);	
			//move_token_up(&game);
				break;
			default:
				break;
		}
	}
	else if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){
		J_up = 0;
		J_right = 0;
		J_left = 0;
		J_down = 0;
		J_select++;
		switch(J_select){
			case 1:
					conferm_player_move(&game);
				break;
			default:
				break;
		}
	}
	else{
		J_up = 0;
		J_right	= 0;
		J_left = 0;
		J_down=0;
	}
	
	/* button management */
	
	if(down_1!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<11)) == 0){	/* KEY1 pressed */
			down_1++;				
			switch(down_1){
				case 2:				/* pay attention here: please see slides to understand value 2 */
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_1=0;			
			NVIC_EnableIRQ(EINT1_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 22);     /* External interrupt 0 pin selection */
		}
	}
	
	if(down_2!=0){ 
		if((LPC_GPIO2->FIOPIN & (1<<12)) == 0){	/* KEY1 pressed */
			down_2++;				
			switch(down_2){
				case 2:				/* pay attention here: please see slides to understand value 2 */
					break;
				default:
					break;
			}
		}
		else {	/* button released */
			down_2=0;			
			NVIC_EnableIRQ(EINT2_IRQn);							 /* enable Button interrupts			*/
			LPC_PINCON->PINSEL4    |= (1 << 24);     /* External interrupt 0 pin selection */
		}
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
