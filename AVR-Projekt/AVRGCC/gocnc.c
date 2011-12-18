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
#include "globdef.h"
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "gocnc.h"

#define xAxis 0
#define yAxis 1
#define zAxis 2

#define X AXIS[xAxis]
#define Y AXIS[yAxis]
#define Z AXIS[zAxis]
	
int16_t um_to_steps(int32_t umGoTo)
{
	umGoTo*=(int16_t)xyz_step_mm;
	umGoTo/=1000;
	return umGoTo;
}

int32_t steps_to_um(int32_t steps)
{
	steps*=1000;
	steps/=(int32_t)xyz_step_mm;
	return steps;		
}	
	
void axis_step(int8_t AxisSelect,uint16_t AxisSpeed){

	if (AXIS[AxisSelect].AxisDirection == 1)
	{
		CNC_PORT |= _BV((AxisSelect*2));
	} 
	else
	{
		CNC_PORT &= ~_BV((AxisSelect*2));
	}
	
	CNC_PORT |= _BV(((AxisSelect*2)+1));
	#ifndef NOdelay
	_delay_us(1000/*AxisSpeed/2*/);
	#endif
	CNC_PORT &= ~_BV(((AxisSelect*2)+1));
	#ifndef NOdelay
	_delay_us(1000/*AxisSpeed*/);
	#endif
	AXIS[AxisSelect].AxisAbsPos += (int8_t)AXIS[AxisSelect].AxisDirection;
	AXIS[AxisSelect].AxisRelPos += (int8_t)AXIS[AxisSelect].AxisDirection;
}
void axis_move(int8_t AxisSelect,int16_t AxisGoto, uint16_t AxisSpeed)
{
	if (AxisGoto > 0)
		{	
			AXIS[AxisSelect].AxisDirection = 1;
		} 
	else
		{
			AXIS[AxisSelect].AxisDirection = -1;
		}
	AxisGoto = abs(AxisGoto);
	for (uint16_t i=0; i<AxisGoto; i++)
		{
			axis_step(AxisSelect,AxisSpeed);
		}
}
void axis_move_parallel(int16_t xAxisGoto,int16_t yAxisGoto,uint16_t AxisSpeed){
	uint16_t step_counter;
	int8_t xAxisDir,yAxisDir;
	if (xAxisGoto > 0)
		{	
			xAxisDir = 1;
		} 
	else
		{
			xAxisDir = -1;
		}
	if (yAxisGoto > 0)
		{	
			yAxisDir = 1;
		} 
	else
		{
			yAxisDir = -1;
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
					axis_move(xAxis,xAxisDir,v_1);
					--xAxisGoto;
				}
			if (yAxisGoto != 0)
				{
					axis_move(yAxis,yAxisDir,v_1);
					--yAxisGoto;
				}
		}
}
void axis_move_interpol(int16_t x0, int16_t y0, int16_t xAxisGoto, int16_t yAxisGoto, uint16_t AxisSpeed){
	int16_t dx = abs(xAxisGoto-x0), sx = x0<xAxisGoto ? 1 : -1;
	int16_t dy = abs(yAxisGoto-y0), sy = y0<yAxisGoto ? 1 : -1;
	int16_t err = (dx>dy ? dx : -dy)/2, e2;
	
	for(;;)
		{ 
			axis_move_parallel((x0-AXIS[xAxis].AxisRelPos),(y0-AXIS[yAxis].AxisRelPos),AxisSpeed);
			if (x0==xAxisGoto && y0==yAxisGoto) 
				break;
			e2 = err;
		    if (e2 >-dx) { err -= dy; x0 += sx; }    
		  	if (e2 < dy) { err += dx; y0 += sy; }
		}	
}



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
	int16_t x = X.AxisAbsPos;
 	axis_move_parallel(steps_to_um(22517),25,v_1);
	x = X.AxisAbsPos;
	axis_move_interpol(0,0,-30,-30,v_1);
	x = X.AxisAbsPos;
//  	axis_move(zAxis,-3948,v_1);
//  	_delay_ms(1000);
//	axis_move_circle(AXIS[xAxis].AxisRelPos,AXIS[yAxis].AxisRelPos,10,0,v_1);
}