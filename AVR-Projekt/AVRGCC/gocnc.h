/************************************************************************
Title:			goCNC-Header-File
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

#ifndef GOCNC_H_
#define GOCNC_H_

extern void axis_ref(void);
extern void axis_set_speed(uint8_t speed);

extern void axis_move(int8_t AxisSelect,int16_t AxisGoto, uint16_t AxisSpeed);
extern void axis_move_single(int8_t AxisSelect,int32_t AxisGoto,uint16_t AxisSpeed); // Move single axis
extern void axis_move_parallel(int32_t xAxisGoto,int32_t yAxisGoto,uint16_t AxisSpeed);	// Move XY axis
extern void axis_move_interpol(int16_t x0, int16_t y0, int32_t xAxisGoto, int32_t yAxisGoto, uint16_t AxisSpeed);	// Move XY interpol

typedef struct _structaxis{
	int8_t AxisStateRef:1;		// Achse referiert ?
	int8_t	AxisStateRdy:1;		// Achse bereit? keine bewegung!
	uint32_t AxisAbsPos:32;		// Achse Absolut Position in Steps
	uint16_t AxisMaxPos:15;		// SoftEnd 300mm/19200 max!
	int32_t AxisRelPos:32;		// Achse relative Position in Steps
	int32_t AxisGoTo:32;		// Achse wo hin??
	int8_t AxisDirection:8;		// Richtung der Achse.		
}structaxis;
volatile structaxis AXIS[3];

#endif /* GOCNC_H_ */