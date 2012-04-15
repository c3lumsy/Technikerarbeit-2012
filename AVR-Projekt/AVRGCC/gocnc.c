/************************************************************************
Title:			goCNC-File
Version:		1.0
Created:		11/22/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.5.1
Hardware:		AVR ATmega1284P @ 18,432 MHz 

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de

				Open Source
************************************************************************/ 
#include "globdef.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "eDIP240.h"
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
	
	AXIS[AxisSelect].AxisAbsPos += (int8_t)AXIS[AxisSelect].AxisDirection;
	AXIS[AxisSelect].AxisRelPos += (int8_t)AXIS[AxisSelect].AxisDirection;
	
	if (!M_FLAGS->ERROR_GLOB)
	{	
	if ((!(AXIS[AxisSelect].AxisAbsPos < 0) && !(AXIS[AxisSelect].AxisAbsPos > AXIS[AxisSelect].AxisMaxPos)) || !(AXIS[AxisSelect].AxisStateRef))
		{
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
			
			M_FLAGS->EDIP_NEWPOS = 1;
		}
		
	else
		{
			M_FLAGS->ERROR_GLOB = 1;
			
			AXIS[AxisSelect].AxisAbsPos -= (int8_t)AXIS[AxisSelect].AxisDirection;
			AXIS[AxisSelect].AxisRelPos -= (int8_t)AXIS[AxisSelect].AxisDirection;
			
			edip_msg(107);						// "Achse ausser Absolutbereich!"
			error_wait();
		}
	}

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
void axis_rel_move_interpol(int16_t x0, int16_t y0, int32_t xAxisGoto, int32_t yAxisGoto, uint16_t AxisSpeed)
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

void axis_abs_move_interpol(int16_t x0, int16_t y0, int32_t xAxisGoto, int32_t yAxisGoto, uint16_t AxisSpeed)
{
 	xAxisGoto = um_to_steps(xAxisGoto);
 	yAxisGoto = um_to_steps(yAxisGoto);
 	x0 = AXIS[xAxis].AxisAbsPos;
	y0 = AXIS[yAxis].AxisAbsPos;
	
	int32_t dx = labs(xAxisGoto-x0), sx = x0<xAxisGoto ? 1 : -1;
	int32_t dy = labs(yAxisGoto-y0), sy = y0<yAxisGoto ? 1 : -1;
	int16_t err = (dx>dy ? dx : -dy)/2, e2;
	
	for(;;)
		{ 
			axis_move_parallel((x0-AXIS[xAxis].AxisAbsPos),(y0-AXIS[yAxis].AxisAbsPos),AxisSpeed/2);
			if (x0==xAxisGoto && y0==yAxisGoto) 
				break;
			e2 = err;
		    if (e2 >-dx) { err -= dy; x0 += sx; }    
		  	if (e2 < dy) { err += dx; y0 += sy; }
		}	
}


uint8_t axis_ref(){
// Define AxisMaxPos
		X.AxisMaxPos = X_POS_MAX;
		Y.AxisMaxPos = Y_POS_MAX;
		Z.AxisMaxPos = Z_POS_MAX;
		
// Clear RefFlags
		X.AxisStateRef = 0;
		Y.AxisStateRef = 0;
		Z.AxisStateRef = 0;
					
// Ref Z Axis
while (!(xyz_REF_SW))
		{
			axis_move(zAxis,1,v_ref);
		}
	axis_move(zAxis,-96,v_ref);
	Z.AxisAbsPos = 0;
	// Z @RefPos
	
// Ref X Axis	
while (!(xyz_REF_SW))
		{
			axis_move(xAxis,-1,v_ref);
		}
	axis_move(xAxis,96,v_ref);
	X.AxisAbsPos = 0;
	X.AxisRelPos = X_REL_NULL;
	X.AxisStateRef = 1;
	// X @RefPos
	
// Ref Y Axis
while (!(xyz_REF_SW))
		{	
			axis_move(yAxis,-1,v_ref);
		}
	axis_move(yAxis,96,v_ref);
	Y.AxisAbsPos = 0;
	Y.AxisRelPos = Y_REL_NULL;
	Y.AxisStateRef = 1;
	// X @RefPos
	
	return 1;
}

uint8_t axis_tool_length()
{
	Z.AxisStateRef = 0;
	
	while (!(xyz_REF_SW))
		{
			axis_move(zAxis,1,v_ref);
		}
	axis_move(zAxis,-96,v_ref);

// hier über Sensor fahren!
	axis_abs_move_interpol(0,0,29500,5500,v_ref);
	
// hier wird gemessen!	
	while (!(xyz_REF_SW))
		{
			axis_move(zAxis,-1,v_teach);
		}
		
	Z.AxisRelPos = zProbeOffset;		
	_delay_ms(1000);
	
	
// Sensor frei fahren!
		axis_move(zAxis,96,v_ref);
		
// Achse frei fahren!
	while (!(xyz_REF_SW))
		{
			axis_move(zAxis,1,v_ref);
		}
	axis_move(zAxis,-96,v_ref);	
	
	M_FLAGS->TOOL_OK = 1;
	
	axis_rel_move_interpol(0,0,0,0,M_FLAGS->AXIS_v1);

	return 1;
}

uint8_t axis_tool_change()
{
	M_FLAGS->TOOL_Change = 0;
	if (axis_tool_length())
		{
			edip_msg(106);
			M_FLAGS->TOOL_OK = 1;
			return 1;
		} 
	else
		{
			return 0;
		}
}

uint8_t axis_set_speed(uint8_t speed)
{
	uint16_t v_1 = v_min - v_max;
	v_1 /= 100;
	v_1 *= speed;
	v_1 = v_min - v_1;
	M_FLAGS->AXIS_v1 = v_1;
	
	return 1;
}