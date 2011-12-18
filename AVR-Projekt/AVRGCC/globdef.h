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

/* define DEBUG */
#define PCmode		// HMI over UART
//#define NOdelay		// no delay for AVR Studio Simulation

/* define BAUD of UART0 and UART1 */
#define UART_BAUD_RATE      19200      
#define UART1_BAUD_RATE     9600

/* define I/O */
#define _DISABLE	0
#define _OFF		0 
#define _FALSE		0
#define	_ENABLE		1
#define _ON			1
#define _TRUE		!=0

/* define AXIS */
#define CNC_PORT PORTA				// Port to goCNC UNI1500
#define xyz_REF_SW (PINA & _BV(6))	// xyz-axis ref-switch

/* define USB State */
typedef struct _structmachinestate{
	unsigned char USB_CON:1;			// USB connected
	unsigned char USB_FILE_OPEN:1;		// USB File open
	unsigned int USB_SEK:16;	// SEK Counter from FILE
	unsigned char PROG_SEL:8;			// Programm-Select		
	unsigned int Last_GCODE:16;			// Number of the last gcode command
}structmachinestate;
volatile structmachinestate MSTATE[1];



#endif /* GLOBDEF_H_ */