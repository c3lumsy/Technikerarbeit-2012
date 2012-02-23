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

/* define CPU frequency in Hz here */
#ifndef F_CPU
#define F_CPU 20000000UL				// 20MHz CPU-Freq
#endif

/* define DEBUG */
#define PCmode							// HMI over UART
//#define NOdelay						// no delay for AVR Studio Simulation

/* define BAUD of UART0 and UART1 */
#define UART_BAUD_RATE      19200      
#define UART1_BAUD_RATE     9600

/* define I2C Clock in Hz*/
#define SCL_CLOCK  100000UL				// 100kHz I2C-Freq

/* define I/O */
#define _DISABLE	0
#define _OFF		0 
#define _FALSE		0
#define	_ENABLE		1
#define _ON			1
#define _TRUE		!=0

/* define AXIS */
#define CNC_PORT PORTA					// Port to goCNC UNI1500
#define xyz_REF_SW (PINA & _BV(6))		// xyz-axis ref-switch

#define xAxis 0
#define yAxis 1
#define zAxis 2

#define xyz_step_mm 96					// x-axis steps per mm

#define v_max 200						// max speed
#define v_1 1000
#define v_hand 1000						// hand speed
#define v_ref 1500						// ref speed

#define X AXIS[xAxis]
#define Y AXIS[yAxis]
#define Z AXIS[zAxis]

/* define eDIP240-7 Display */
char* eDIP_BUFFER[8];					// BUFFER for eDIP Input

/* define USB-Filenames */
static char FILENAME[12] = "start.tap";

/* define Maschine State */
typedef struct _structmachinestate{
	unsigned char USB_CON:1;			// USB connected
	unsigned char USB_RDY:1;			// USB Ready
	unsigned char USB_FILE_OPEN:1;		// USB File open
	unsigned int USB_SEK:16;			// SEK Counter from FILE
	unsigned char PROG_SEL:8;			// Programm-Select		
	unsigned int Last_GCODE:16;			// Number of the last gcode command
	unsigned char USB_FILE_EOF:1;		// EndOfFile
	
	unsigned char GCODE_MM:1;			// Maschineunits inch=0 / mm=1; 

	unsigned char EDIP_ACTION:1;		// Flag for new Action from eDIP
	unsigned char EDIP_CNC_HAND:1;

	unsigned char ERROR_GLOB:1;			// Global ERROR-Flag

}structmachinestate;
volatile structmachinestate M_FLAGS[1];



#endif /* GLOBDEF_H_ */