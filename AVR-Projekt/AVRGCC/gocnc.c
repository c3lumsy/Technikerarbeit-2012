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
	
int32_t um_to_steps(int32_t umGoTo)
{
	umGoTo*=(int32_t)xyz_step_mm;
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
	_delay_us(AxisSpeed/5);
	#endif
	CNC_PORT &= ~_BV(((AxisSelect*2)+1));
	#ifndef NOdelay
	_delay_us(AxisSpeed);
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
void axis_move_single(int8_t AxisSelect,int32_t AxisGoto,uint16_t AxisSpeed)
{
	AxisGoto = um_to_steps(AxisGoto);
	AxisGoto -= AXIS[AxisSelect].AxisRelPos;
	axis_move(AxisSelect,AxisGoto,AxisSpeed);
}
void axis_move_parallel(int32_t xAxisGoto,int32_t yAxisGoto,uint16_t AxisSpeed){
	uint16_t step_counter;
	int8_t xAxisDir,yAxisDir;
	
// 	xAxisGoto = um_to_steps(xAxisGoto);
// 	yAxisGoto = um_to_steps(yAxisGoto);
// 
// 	xAxisGoto -= AXIS[xAxis].AxisRelPos;
// 	yAxisGoto -= AXIS[yAxis].AxisRelPos;
// 	char temp[8];
// 	uart_puts(" ");
// 	uart_puts(ltoa(xAxisGoto,temp,10));
// 	uart_puts(" / ");
// 	uart_puts(ltoa(yAxisGoto,temp,10));
// 	
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
		
	xAxisGoto = labs(xAxisGoto);
	yAxisGoto = labs(yAxisGoto);
	
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
					axis_move(xAxis,xAxisDir,AxisSpeed);
					--xAxisGoto;
				}
			if (yAxisGoto != 0)
				{
					axis_move(yAxis,yAxisDir,AxisSpeed);
					--yAxisGoto;
				}
		}
}
void axis_move_interpol(int16_t x0, int16_t y0, int32_t xAxisGoto, int32_t yAxisGoto, uint16_t AxisSpeed)
{
 	xAxisGoto = um_to_steps(xAxisGoto);
 	yAxisGoto = um_to_steps(yAxisGoto);
 	x0 = AXIS[xAxis].AxisRelPos;
	y0 = AXIS[yAxis].AxisRelPos;
	
	int32_t dx = labs(xAxisGoto-x0), sx = x0<xAxisGoto ? 1 : -1;
	int32_t dy = labs(yAxisGoto-y0), sy = y0<yAxisGoto ? 1 : -1;
	int16_t err = (dx>dy ? dx : -dy)/2, e2;
	
	for(;;)
		{ 
			axis_move_parallel((x0-AXIS[xAxis].AxisRelPos),(y0-AXIS[yAxis].AxisRelPos),AxisSpeed/2);
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
	axis_move(zAxis,-4000,v_max);
	Z.AxisAbsPos = 0;
	Z.AxisStateRef = 1;
	// Z @RefPos
	
// Ref X Axis	
while (!(xyz_REF_SW))
		{
			axis_move(xAxis,-1,v_ref);
		}
	axis_move(xAxis,9600,v_max);
	X.AxisAbsPos = 0;
	X.AxisStateRef = 1;
	// X @RefPos
	
// Ref Y Axis
while (!(xyz_REF_SW))
		{	
			axis_move(yAxis,-1,v_ref);
		}
	axis_move(yAxis,960,v_max);
	Y.AxisAbsPos = 0;
	Y.AxisStateRef = 1;
	// X @RefPos
	_delay_ms(1000);
}

void go_cnc(void)
{	

}