/************************************************************************
Title:			Global Def-File
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
#ifndef GLOBDEF_H_
#define GLOBDEF_H_

/* define CPU frequency in MHz here */
#ifndef F_CPU
#define F_CPU 20000000UL
#endif

#define PCmode		// HMI over UART
#define NOdelay		// no delay for AVR Studio Simulation

/* define BAUD of UART0 and UART1 */
#define UART_BAUD_RATE      19200      
#define UART1_BAUD_RATE     9600

/* define I/O */
#define _DISABLE,_OFF,_FALSE	0
#define	_ENABLE,_ON		1
#define _TRUE !=0

/* define AXIS */
#define CNC_PORT PORTA				// Port to goCNC UNI1500
#define xyz_REF_SW (PINA & _BV(6))	// xyz-axis ref-switch

#define x_step_mm 96	// x-axis steps per mm
#define y_step_mm 96	// y-axis steps per mm
#define z_step_mm 96	// z-axis steps per mm

int32_t sX_IST, sY_IST, sZ_IST;

typedef struct {
	uint8_t AxisStateRef:1;		// Achse referiert ?
	uint8_t	AxisStateRdy:1;		// Achse bereit? keine bewegung!
	uint16_t AxisAbsPos:15;		// Achse Absolut Position in Steps
	uint16_t AxisMaxPos:15;		// SoftEnd 300mm/19200 max!
	int16_t AxisRelPos:16;		// Achse relative Position in Steps
	int16_t AxisGoTo:16;		// Achse wo hin??
	int8_t AxisDirection:8;		// Richtung der Achse.		
}structaxis;

#define xAxis 0
#define yAxis 1
#define zAxis 2

structaxis AXIS[2];
#define X AXIS[xAxis]
#define Y AXIS[yAxis]
#define Z AXIS[zAxis]

#endif /* GLOBDEF_H_ */