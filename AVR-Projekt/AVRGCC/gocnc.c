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
#include <inttypes.h>
#include "uart.h"


#define CNC_PORT PORTA

#define x_step_pr 144		// x-axis steps per around
#define y_step_pr 144		// y-axis steps per around
#define z_step_pr 144		// z-axis steps per around

#define x_way_pr 1.5		// x-axis spindle-solution in millimeter
#define y_way_pr 1.5		// y-axis spindle-solution in millimeter
#define z_way_pr 1.5		// z-axis spindle-solution in millimeter

#define speed 300	// Speed


#define x_tact_h (CNC_PORT |= (1 << PA1));
#define x_tact_l (CNC_PORT &= ~(1 << PA1));

#define y_tact_h (CNC_PORT |= (1 << PA3));
#define y_tact_l (CNC_PORT &= ~(1 << PA3));

#define z_tact_h (CNC_PORT |= (1 << PA5));
#define z_tact_l (CNC_PORT &= ~(1 << PA5));

#define x_CCW (CNC_PORT |= (1 << PA0));		// RIGHT	// x-axis direction ContraClockWise	
#define x_CW (CNC_PORT &= ~(1 << PA0));		// LEFT		// x-axis direction ClockWise

#define y_CCW (CNC_PORT |= (1 << PA2));		// FRONT	// y-axis direction ContraClockWise
#define y_CW (CNC_PORT &= ~(1 << PA2));		// BACK		// y-axis direction ClockWise		

#define z_CCW (CNC_PORT |= (1 << PA4));		// UP		// z-axis direction ContraClockWise
#define z_CW (CNC_PORT &= ~(1 << PA4));		// DOWN		// z-axis direction ClockWise

#define xyz_REF_SW (PINA & (1<<PA6))		// xyz-axis ref-switch

float x_way_step = (x_step_pr/x_way_pr);
float y_way_step = (y_step_pr/y_way_pr);
float z_way_step = (z_step_pr/z_way_pr);


// auslagern...
uint16_t rounded(float n)
{
  if (n-(uint16_t)(n) >= 0.5)
    return (uint16_t)(n+1);
  else
    return (uint16_t)(n);
}

int sgn(int x){
  return (x > 0) ? 1 : (x < 0) ? -1 : 0;
}

void x_step (uint16_t trip_speed)
	{
			_delay_us(trip_speed);
			x_tact_h;
			_delay_us(trip_speed/7);
			x_tact_l;
	}
void x_move (float way_mm, uint16_t trip_speed){
	// select direction
		if (way_mm < 0)
			{
				x_CW;
				// Vorzeichen drehen!
				way_mm = -way_mm;
			} 
		else
			{
				x_CCW;
			}
		
	// way to steps
	float steps;
		steps = x_way_step * way_mm;
		
		for(uint16_t i=0; i<(rounded(steps)); i++)
		{
			x_step(trip_speed);
		}	
}

void y_step (uint16_t trip_speed)
	{
			_delay_us(trip_speed);
			y_tact_h;
			_delay_us(trip_speed/7);
			y_tact_l;
	}
void y_move (float way_mm, uint16_t trip_speed){
	// select direction
		if (way_mm < 0)
			{
				y_CW;
				// Vorzeichen drehen!
				way_mm = -way_mm;
			} 
		else
			{
				y_CCW;
			}
		
	// way to steps
	float steps;
		steps = y_way_step * way_mm;
		
		for(uint16_t i=0; i<(rounded(steps)); i++)
		{
			y_step(trip_speed);
		}	
}

void z_step (uint16_t trip_speed)
	{
			_delay_us(trip_speed);
			z_tact_h;
			_delay_us(trip_speed/7);
			z_tact_l;
	}
void z_move (float way_mm, uint16_t trip_speed){
	// select direction
		if (way_mm < 0.0)
			{
				z_CCW;
				// Vorzeichen drehen!
				way_mm = -way_mm;
			}
		else
			{
				z_CW;
			}

	// way to steps
	float steps;
		steps = z_way_step * way_mm;
		
		for(uint16_t i=0; i<(rounded(steps)); i++)
		{
			z_step(trip_speed);
		}	
}

void xy_move (float x_way_mm, float y_way_mm, uint16_t x_trip_speed, uint16_t y_trip_speed)
	{
		uint16_t step_counter;
		float x_steps, y_steps;
				x_steps = x_way_step * x_way_mm;
				y_steps = y_way_step * y_way_mm;
		
		// select x direction
		if (x_way_mm < 0)
			{
				x_CW;
				// Vorzeichen drehen!
				x_way_mm = -x_way_mm;
			} 
		else
			{
				x_CCW;
			}
		// select y direction
		if (y_way_mm < 0)
			{
				y_CW;
				// Vorzeichen drehen!
				y_way_mm = -y_way_mm;
			} 
		else
			{
				y_CCW;
			}
			
			// Gesamtsteps berechnen!
			if (rounded(x_steps)>=rounded(y_steps))
			{
				step_counter = rounded(x_steps);
			} 
			else
			{
				step_counter = rounded(y_steps);
			}
			
		// Diagonalfahrt berechnen
		for(uint16_t i = step_counter ; i>0 ; i--)
		{		
			if (x_steps != 0)
			{
				x_step(x_trip_speed);
				x_steps--;
			}
			if (y_steps != 0)
			{
				y_step(y_trip_speed);
				y_steps--;
			}
		}	
			
	}


void line(int x0, int y0, int x1, int y1)
{
  int dx =  abs((x1)-x0), sx = x0<x1 ? 1 : -1;
  int dy = -abs((y1)-y0), sy = y0<y1 ? 1 : -1;
  int err = dx+dy, e2; /* error value e_xy */
 
  for(;;){  /* loop */
    xy_move((x0),(y0),800,800);
    if (x0==x1 && y0==y1) break;
    e2 = 2*err;
    if (e2 > dy) { err += dy; x0 += sx; } /* e_xy+e_x > 0 */
    if (e2 < dx) { err += dx; y0 += sy; } /* e_xy+e_y < 0 */
  }
}

void rasterCircle(float x0, float y0, float radius)
  {
    float f = 1 - radius;
    float ddF_x = 0;
    float ddF_y = -2 * radius;
    float x = 0;
    float y = radius;
 
    xy_move(x0, y0 + radius, 800, 800);
    xy_move(x0, y0 - radius, 800, 800);
    xy_move(x0 + radius, y0, 800, 800);
    xy_move(x0 - radius, y0, 800, 800);
 
    while(x < y) 
    {
      if(f >= 0) 
      {
        y--;
        ddF_y += 2;
        f += ddF_y;
      }
      x++;
      ddF_x += 2;
      f += ddF_x + 1;
 
      xy_move(x0 + x, y0 + y, 800, 800);
      xy_move(x0 - x, y0 + y, 800, 800);
      xy_move(x0 + x, y0 - y, 800, 800);
      xy_move(x0 - x, y0 - y, 800, 800);
      xy_move(x0 + y, y0 + x, 800, 800);
      xy_move(x0 - y, y0 + x, 800, 800);
      xy_move(x0 + y, y0 - x, 800, 800);
      xy_move(x0 - y, y0 - x, 800, 800);
    }
  }

void sqare(void){
	// Z heben
	z_CCW;
	z_move(480, speed);	
	
	x_CW;
	y_CW;
	x_move(960, speed);
	y_move(960, speed);
		
	_delay_ms(500);	
	
	for (int row=0;row<10;row++)
		{
			for (int column=0; column<10; column++)
			{
				z_CW;
				z_move(480,speed);
				_delay_ms(10);
				z_CCW;
				z_move(480,speed);
				
				x_CCW;
				x_move(960, speed);				
			}			
			
			x_CW;
			x_move(9600, speed);
			y_CCW;
			y_move(960, speed);
		}
		
		y_CW;
		y_move(9600, speed);
		

}

void axis_ref(){
	// Z Ref
	
	// Z heben bis Ref
	while (!(xyz_REF_SW))
		{
			z_move(-0.05, speed*6);
		}
	// Z auf Ref-Pos
	z_move(5.0, speed*2);

	while (!(xyz_REF_SW))
		{
			x_move(-0.05, speed*6);
		}
	// X auf Ref-Pos
	x_move(5.0, speed*2);
	
	while (!(xyz_REF_SW))
		{
			y_move(-0.05, speed*6);
		}
	// Y aus Ref-Pos
	y_move(5.0, speed*2);
	
	
	
	
	
	
	
	/*
	while (1)
	{
		if ( !(PINA & (1<<PA6)) )
		{
			PORTD |= (1<<PD6);
		}
		else
		{
			PORTD &= ~(1<<PD6);
		}
	}
	*/
	
	

	
	
	
	
}

void go_cnc(void){

	while (1)
	{
	}
	
}