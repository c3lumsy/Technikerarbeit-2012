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
		
void axis_step(int8_t AxisSelect,uint16_t AxisSpeed){
	CNC_PORT |= _BV(((AxisSelect*2)+1));
	#ifndef NOdelay
	_delay_us(AxisSpeed/2);
	#endif
	CNC_PORT &= ~_BV(((AxisSelect*2)+1));
	#ifndef NOdelay
	_delay_us(AxisSpeed);
	#endif
	AXIS[AxisSelect].AxisAbsPos += AXIS[AxisSelect].AxisDirection;
	AXIS[AxisSelect].AxisRelPos += AXIS[AxisSelect].AxisDirection;
}
void axis_move(int8_t AxisSelect,int16_t AxisGoto, uint16_t AxisSpeed)
{
	if (AxisGoto > 0)
		{	
			AXIS[AxisSelect].AxisDirection = 1;
			CNC_PORT |= _BV((AxisSelect*2));
		} 
	else
		{
			AXIS[AxisSelect].AxisDirection = -1;
			CNC_PORT &= ~_BV((AxisSelect*2));
		}
	AxisGoto = abs(AxisGoto);
	for (uint16_t i=0; i<AxisGoto; i++)
		{
			axis_step(AxisSelect,AxisSpeed);
		}
}

void um_to_steps(){
	
}

void axis_move_parallel(int16_t xAxisGoto,int16_t yAxisGoto,uint16_t AxisSpeed){
	uint16_t step_counter;
	
	if (xAxisGoto > 0)
		{	
			AXIS[xAxis].AxisDirection = 1;
			CNC_PORT |= _BV((xAxis*2));
		} 
	else
		{
			AXIS[xAxis].AxisDirection = -1;
			CNC_PORT &= ~_BV((xAxis*2));
		}
	if (yAxisGoto > 0)
		{	
			AXIS[yAxis].AxisDirection = 1;
			CNC_PORT |= _BV((yAxis*2));
		} 
	else
		{
			AXIS[yAxis].AxisDirection = -1;
			CNC_PORT &= ~_BV((yAxis*2));
		}
		
	xAxisGoto = abs(xAxisGoto);
	yAxisGoto = abs(yAxisGoto);
	
	if (xAxisGoto>=yAxisGoto)
		{
			step_counter = xAxisGoto;
		} 
	else
		{
			step_counter = yAxisGoto;
		}
	
	for(uint16_t i = step_counter ; i>0 ; i--)
		{		
			if (xAxisGoto != 0)
				{
					axis_move(xAxis,1,v_1);
					--xAxisGoto;
				}
			if (yAxisGoto != 0)
				{
					axis_move(yAxis,1,v_1);
					--yAxisGoto;
				}
		}
}

void axis_move_interpol(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t AxisSpeed){
	int16_t dx = abs(x1-x0),x2=0, sx = x0<x1 ? 1 : -1;
	int16_t dy = abs(y1-y0),y2=0, sy = y0<y1 ? 1 : -1;
	int16_t err = (dx>dy ? dx : -dy)/2, e2;
		
	for(;;)
		{  /* loop */
			axis_move_parallel(((x0-x2)*10),((y0-y2)*10),AxisSpeed);
			if (x0==x1 && y0==y1) break;
			x2 = x0; y2 = y0;
			e2 = err;
		    	if (e2 >-dx) { err -= dy; x0 += sx; }    
		  	if (e2 < dy) { err += dx; y0 += sy; }
		}}

void axis_ref(){
// Define AxisMaxPos
	// X max 200mm / 12800 Steps!
		X.AxisMaxPos = 12800;
	// Y max 290mm / 18560 Steps!
		Y.AxisMaxPos = 18560;
	// Z max 65mm / 4160 Steps!
		Z.AxisMaxPos = 4160;
				
// Ref Z Axis
while (!(xyz_REF_SW))
		{
			axis_move(zAxis,1,v_ref);
		}
	axis_move(zAxis,-96,v_ref);
	Z.AxisAbsPos = 0;
	Z.AxisStateRef = 1;
	// Z @RefPos
	
// Ref X Axis	
while (!(xyz_REF_SW))
		{
			axis_move(xAxis,-1,v_ref);
		}
	axis_move(xAxis,96,v_ref);
	X.AxisAbsPos = 0;
	X.AxisStateRef = 1;
	// X @RefPos
	
// Ref Y Axis
while (!(xyz_REF_SW))
		{
			axis_move(yAxis,-1,v_ref);
		}
	axis_move(yAxis,96,v_ref);
	Y.AxisAbsPos = 0;
	Y.AxisStateRef = 1;
	// X @RefPos
	_delay_ms(1000);
}

void go_cnc(void)
{

	axis_move_parallel(960,4800,v_1);
	
	uart_puts("FINISHED!");	
}