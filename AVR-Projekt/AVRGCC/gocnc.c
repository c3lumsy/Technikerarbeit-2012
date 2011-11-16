/*************************************************************************
Title:    goCNC-Controller
Author:   Dennis Hohmann hohmannd@web.de

DESCRIPTION:
          goCNC A4 Ansteuerung

*************************************************************************/

#include <stdlib.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/delay.h>

#define CNC_PORT PORTB

#define x_step_pr 72		// x-axis steps per around
#define y_step_pr 72		// y-axis steps per around
#define z_step_pr 72		// z-axis steps per around

#define x_way_pr 1,5		// x-axis spindle-solution in millimeter
#define y_way_pr 1,5		// y-axis spindle-solution in millimeter
#define z_way_pr 1,5		// z-axis spindle-solution in millimeter

#define microstep = 1;		// if microsteps used, steps*2 per around )

#define speed 1		// Speed


#define x_tact_h (CNC_PORT |= (1 << PB1));
#define x_tact_l (CNC_PORT &= ~(1 << PB1));

#define y_tact_h (CNC_PORT |= (1 << PB3));
#define y_tact_l (CNC_PORT &= ~(1 << PB3));

#define z_tact_h (CNC_PORT |= (1 << PB5));
#define z_tact_l (CNC_PORT &= ~(1 << PB5));

#define x_CCW (CNC_PORT |= (1 << PB0));		// RIGHT	// x-axis direction ContraClockWise	
#define x_CW (CNC_PORT &= ~(1 << PB0));		// LEFT		// x-axis direction ClockWise

#define y_CCW (CNC_PORT |= (1 << PB2));		// FRONT	// y-axis direction ContraClockWise
#define y_CW (CNC_PORT &= ~(1 << PB2));		// BACK		// y-axis direction ClockWise		

#define z_CCW (CNC_PORT |= (1 << PB4));		// UP		// z-axis direction ContraClockWise
#define z_CW (CNC_PORT &= ~(1 << PB4));		// DOWN		//z-axis direction ClockWise


void go_cnc(void){
	x_CCW;
	y_CCW;
	z_CCW;
		for(int i=0; i<(x_step_pr*2); i++)
		{
			_delay_ms(speed);
			x_tact_h;
			y_tact_h;
			z_tact_h;
			_delay_ms(speed);
			x_tact_l;
			y_tact_l;
			z_tact_l;		
		}
		_delay_ms(1000);
		
	x_CW;
	y_CW;
	z_CW;
		for(int i=0; i<(x_step_pr*2); i++)
		{
			_delay_ms(speed);
			x_tact_h;
			y_tact_h;
			z_tact_h;
			_delay_ms(speed);
			x_tact_l;
			y_tact_l;
			z_tact_l;		
		}
		_delay_ms(1000);
}

void z_move (int step){
		for(int i=0; i<(step); i++)
		{
			_delay_ms(speed);
			z_tact_h;
			_delay_ms(speed);
			z_tact_l;		
		}	
}
void x_move (int step){
		for(int i=0; i<(step); i++)
		{
			_delay_ms(speed);
			x_tact_h;
			_delay_ms(speed);
			x_tact_l;		
		}	
}
void y_move (int step){
		for(int i=0; i<(step); i++)
		{
			_delay_ms(speed);
			y_tact_h;
			_delay_ms(speed);
			y_tact_l;		
		}	
}

void sqare(void){
	// Z heben
	z_CCW;
	z_move(480);	
	
	x_CCW;
	y_CCW;
	x_move(960);
	y_move(960);
		
	_delay_ms(500);	
	
	for (int row=0;row<10;row++)
		{
			for (int column=0; column<10; column++)
			{
				z_CW;
				z_move(480);
				_delay_ms(10);
				z_CCW;
				z_move(480);
				
				x_CCW;
				x_move(960);				
			}			
			
			x_CW;
			x_move(9600);
			y_CCW;
			y_move(960);
		}
		
		y_CW;
		y_move(9600);
		

}