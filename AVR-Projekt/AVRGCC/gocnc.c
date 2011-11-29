/************************************************************************
Title:			goCNC-File
Version:		1.0
Created:		11/22/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284 @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 

#include <stdlib.h>
#include <avr/io.h>
#include <avr/signal.h>
#include <avr/delay.h>
#include <inttypes.h>
#include <math.h>
#include "uart.h"
#include "globdef.h"

#define v_max 700	// max speed 15mm/s
#define v_1 1000 
#define v_ref 1000	// ref speed

#define x_tact_h (CNC_PORT |= _BV(1));
#define x_tact_l (CNC_PORT &= ~_BV(1));
#define y_tact_h (CNC_PORT |= _BV(3));
#define y_tact_l (CNC_PORT &= ~_BV(3));
#define z_tact_h (CNC_PORT |= _BV(5));
#define z_tact_l (CNC_PORT &= ~_BV(5));

#define x_CCW	(CNC_PORT |= _BV(0));		// RIGHT	// x-axis direction ContraClockWise	
#define x_CW	(CNC_PORT &= ~_BV(0));		// LEFT		// x-axis direction ClockWise
#define y_CCW	(CNC_PORT |= _BV(2));		// FRONT	// y-axis direction ContraClockWise
#define y_CW	(CNC_PORT &= ~_BV(2));		// BACK		// y-axis direction ClockWise		
#define z_CCW	(CNC_PORT |= _BV(4));		// UP		// z-axis direction ContraClockWise
#define z_CW	(CNC_PORT &= ~_BV(4));		// DOWN		// z-axis direction ClockWise

#define xyz_REF_SW (PINA & _BV(6))		// xyz-axis ref-switch

uint8_t x_way_step = (x_step_pr/x_way_pr);
uint8_t y_way_step = (y_step_pr/y_way_pr);
uint8_t z_way_step = (z_step_pr/z_way_pr);

// auslagern...

int sgn(uint16_t x){
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

uint16_t rounded(float n)
{
  if (n-(uint16_t)(n) >= 0.5)
    return (uint16_t)(n+1);
  else
    return (uint16_t)(n);
}

int32_t fmm_to_ium(float f)
	{
		int32_t i = f * 1000;
		return i;
	}
	
uint16_t i_round_1000(uint32_t i)
	{
		uint16_t o = i/1000;
		uint16_t j = i%1000;
 		if (j >= 500)
		{
			o++;
		}
		return o;
	}	
	
	
// bis hier her!	
	

void x_step (uint16_t trip_speed)
	{
			x_tact_h;
			_delay_us(trip_speed/2);
			x_tact_l;
			_delay_us(trip_speed);
	}
void x_move (int32_t way_um, uint16_t trip_speed){
	// select direction
	int8_t direction;
		if (way_um > 0)
			{x_CCW;
			direction = 1;
			} 
		else
			{
			x_CW;
			direction = -1;
			}
		
	// way to steps
		uint32_t steps = labs(way_um) * x_way_step;
		steps = i_round_1000(steps);
		for(uint16_t i=0; i<steps; i++)
		{
			x_step(trip_speed);
			if (direction > 0)
			{sX_IST++;} 
			else
			{sX_IST--;}
		}	
}

void y_step (uint16_t trip_speed)
	{
			y_tact_h;
			_delay_us(trip_speed);
			y_tact_l;
	}
void y_move (int32_t way_um, uint16_t trip_speed){
	// select direction
	int8_t direction;
		if (way_um > 0)
			{y_CCW;
			direction = 1;
			} 
		else
			{
			y_CW;
			direction = -1;
			}
		
	// way to steps
		uint32_t steps = labs(way_um) * y_way_step;
		steps = i_round_1000(steps);
		for(uint16_t i=0; i<steps; i++)
		{
			y_step(trip_speed);
			if (direction > 0)
			{sY_IST++;} 
			else
			{sY_IST--;}
		}	
}

void z_step (uint16_t trip_speed)
	{
			z_tact_h;
			_delay_us(trip_speed);
			z_tact_l;
	}
void z_move (int32_t way_um, uint16_t trip_speed){
	// select direction
	int8_t direction;
		if (way_um > 0)
			{z_CW;
			direction = 1;
			} 
		else
			{
			z_CCW;
			direction = -1;
			}
		
	// way to steps
		uint32_t steps = labs(way_um) * z_way_step;
		steps = i_round_1000(steps);
		for(uint16_t i=0; i<steps; i++)
		{
			z_step(trip_speed);
			if (direction > 0)
			{sZ_IST++;} 
			else
			{sZ_IST--;}
		}	
}

void xy_move (int32_t x_way_um, int32_t y_way_um, uint16_t trip_speed)
	{
		uint32_t step_counter;
		int8_t x_direction,y_direction;
	
		// select x direction
		if (x_way_um < 0)
			{
				x_CW;
				x_direction = -1;
			} 
		else
			{
				x_CCW;
				x_direction = 1;
			}
		// select y direction
		if (y_way_um < 0)
			{
				y_CW;
				y_direction = -1;
			} 
		else
			{
				y_CCW;
				y_direction = 1;
			}
			
		uint32_t x_steps = labs(x_way_um) * x_way_step;
		uint32_t y_steps = labs(y_way_um) * y_way_step;
		x_steps = i_round_1000(x_steps);
		y_steps = i_round_1000(y_steps);
			
			
			// Gesamtsteps berechnen!
			if (x_steps>=y_steps)
			{
				step_counter = x_steps;
			} 
			else
			{
				step_counter = y_steps;
			}
			
		// Diagonalfahrt berechnen
		for(uint32_t i = step_counter ; i>0 ; i--)
		{		
			if (x_steps != 0)
			{
				x_step(trip_speed);
				if (x_direction > 0)
				{sX_IST++;} 
				else
				{sX_IST--;}
				x_steps--;
				
			}
			if (y_steps != 0)
			{
				y_step(trip_speed);
				if (y_direction > 0)
				{sY_IST++;} 
				else
				{sY_IST--;}
				y_steps--;
			}
		}	
			
	}

void line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint16_t trip_speed){
		int32_t dx = labs(x1-x0), sx = x0<x1 ? 1 : -1;
		int32_t dy = labs(y1-y0), sy = y0<y1 ? 1 : -1;
		int32_t err = (dx>dy ? dx : -dy)/2, e2;
		
	for(;;)
		{  /* loop */
		xy_move((x0),(y0),trip_speed);
			if ((x0*100)==x1 && (y0*100)==y1) break;
			e2 = err;
		    	if (e2 >-dx) { err -= dy; x0 += sx; }    
		  	if (e2 < dy) { err += dx; y0 += sy; }
	}}

void axis_ref(){
	// Z heben bis Ref
while (!(xyz_REF_SW))
		{
			z_move(fmm_to_ium(-0.1), v_ref);
		}
	// Z auf Ref-Pos
	z_move(fmm_to_ium(5.0), v_max);
	sZ_IST = 0;

while (!(xyz_REF_SW))
		{
			x_move(fmm_to_ium(-0.1), v_ref);
		}
	// X auf Ref-Pos
	x_move(fmm_to_ium(5.00), v_max);
	sX_IST = 0;
	
while (!(xyz_REF_SW))
		{
			y_move(fmm_to_ium(-0.1), v_ref);
		}
	// Y auf Ref-Pos
	y_move(fmm_to_ium(5.00), v_max);
	sY_IST = 0;	

	_delay_ms(100);
}

void go_cnc(void)
{
	z_move(fmm_to_ium(38.0),v_1);
	uart_puts("LINE_MOVE");
//		line(0,0,fmm_to_ium(4.0),fmm_to_ium(4.0),v_1);

	line(0,0,fmm_to_ium(30.0),fmm_to_ium(5.0),v_1);
	
	uart_puts("FINISHED!");	
}