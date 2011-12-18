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
extern void axis_move(int8_t AxisSelect,int16_t AxisGoto, uint16_t AxisSpeed);
extern void axis_move_parallel(int16_t xAxisGoto,int16_t yAxisGoto,uint16_t AxisSpeed);
extern void axis_move_interpol(int16_t x0, int16_t y0, int16_t xAxisGoto, int16_t yAxisGoto, uint16_t AxisSpeed);

extern void go_cnc(void);

#define xyz_step_mm 96	// x-axis steps per mm

#define v_max 700	// max speed 15mm/s
#define v_1 1000 
#define v_ref 1000	// ref speed

typedef struct _structaxis{
	int8_t AxisStateRef:1;		// Achse referiert ?
	int8_t	AxisStateRdy:1;		// Achse bereit? keine bewegung!
	uint16_t AxisAbsPos:15;		// Achse Absolut Position in Steps
	uint16_t AxisMaxPos:15;		// SoftEnd 300mm/19200 max!
	int16_t AxisRelPos:16;		// Achse relative Position in Steps
	int16_t AxisGoTo:16;		// Achse wo hin??
	int8_t AxisDirection:8;		// Richtung der Achse.		
}structaxis;
volatile structaxis AXIS[3];

#endif /* GOCNC_H_ */