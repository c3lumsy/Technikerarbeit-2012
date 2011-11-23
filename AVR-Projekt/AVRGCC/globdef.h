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

#define PCmode

/* define BAUD of UART0 and UART1 */
#define UART_BAUD_RATE      19200      
#define UART1_BAUD_RATE     9600

/* define I/O */
#define _DISABLE	0
#define	_ENABLE		1

/* define AXIS */
#define CNC_PORT PORTA

int32_t sX_IST, sY_IST, sZ_IST;

#define x_step_pr 144		// x-axis steps per around
#define y_step_pr 144		// y-axis steps per around
#define z_step_pr 144		// z-axis steps per around

#define x_way_pr 1.500		// x-axis spindle-solution in millimeter
#define y_way_pr 1.500		// y-axis spindle-solution in millimeter
#define z_way_pr 1.500		// z-axis spindle-solution in millimeter



#endif /* GLOBDEF_H_ */