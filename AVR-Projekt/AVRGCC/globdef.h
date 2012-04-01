/************************************************************************
Title:			Global Def-File
Version:		1.0
Created:		11/22/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.5.1
Hardware:		AVR ATmega1284P @ 18,4320MHz 

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 
#ifndef GLOBDEF_H_
#define GLOBDEF_H_

/* define CPU frequency in Hz here */
#ifndef F_CPU
#define F_CPU 18432000UL				// 18,4320MHz CPU-Freq
#endif

/* define BAUD of UART0 and UART1 */
#define UART_BAUD_RATE      19200		// 19,2kbs UART-Takt
#define UART1_BAUD_RATE     230400		// 230,4kbs UART1-Takt

/* define I2C Clock in Hz*/
#define SCL_CLOCK  100000UL				// 100kHz I2C-Freq

/* define DEBUG */
#define PCmode							// HMI over UART
//#define NOdelay						// no delay for AVR Studio Simulation

/* define I/O */
#define _DISABLE	0
#define _OFF		0 
#define _FALSE		0

#define	_ENABLE		1
#define _ON			1
#define _TRUE		!=0

#define CR			0x0D

/* define AXIS */
#define CNC_PORT PORTA					// Port to goCNC UNI1500
#define xyz_REF_SW (PINA & _BV(6))		// xyz-axis ref-switch & toollength-switch
// 
#define SpindelOFF (PORTA &= ~_BV(7))
#define SpindelON  (PORTA |= _BV(7))

#define xAxis 0
#define yAxis 1
#define zAxis 2

#define xyz_step_mm 96					// axis solution steps per mm

#define v_max 750						// max speed
#define v_min 7500						// min speed
#define v_ref 1825						// ref speed
#define v_teach 3500					// teach speed
/*
#define v_1								// calc speed
*/

#define zProbeOffset 3150				// offset from toollength-switch

#define X AXIS[xAxis]
#define Y AXIS[yAxis]
#define Z AXIS[zAxis]

#define X_POS_MAX 17760					// X max 185mm * xyz_step_mm 96 = 17760 Steps!
#define Y_POS_MAX 26400					// Y max 275mm * xyz_step_mm 96= 26400 Steps!
#define Z_POS_MAX -4320					// Z max 45mm * xyz_step_mm 96 = 4320 Steps!

#define X_REL_NULL -2709					// PCB X-Rel Nullpunkt
#define Y_REL_NULL -3795					// PCB X-Rel Nullpunkt

/* define eDIP240-7 Display */
char* eDIP_BUFFER[8];					// BUFFER for eDIP Input

/* define USB-Filenames */
static char FILE_TE[6]="te.tap";	// Top-Layout Filename
static char FILE_BE[6]="be.tap";	// Bottom-Layout Filename
static char FILE_TD[6]="td.tap";	// Top-Drilldata Filename
static char FILE_BD[6]="bd.tap";	// Bottom-Drilldata Filename
volatile char FILENAME[6];			// File to open

/* define Maschine State */
typedef struct _structmachinestate{
	unsigned int AXIS_v1:13;			// Glogal Maschinespeed 12Bit 4096
	unsigned char USB_CON:1;			// USB connected
	unsigned char USB_RDY:1;			// USB Ready
	unsigned char USB_FILE_OPEN:1;		// USB File open
	unsigned int USB_SEK:16;			// SEK Counter from FILE
	unsigned char PROG_SEL:8;			// Programm-Select		
	unsigned int Last_GCODE:16;			// Number of the last gcode command
	unsigned char USB_FILE_EOF:1;		// EndOfFile
	unsigned char GCODE_MM:1;			// Maschineunits inch=0 / mm=1;
	unsigned char GCODE_TOP:1;			// Top-File
	unsigned char GCODE_BOT:1;			// Bottom-File
	unsigned char GCODE_FILESET:1;		// If new FILENAME set
	
	unsigned char EDIP_NEWPOS:1;		// 
	unsigned char EDIP_ACTION:1;		// Flag for new Action from eDIP
	unsigned char EDIP_STOPP:1;			// Stopp Signal from eDIP
	unsigned char EDIP_CNC_HAND:1;
	unsigned char EDIP_POS_X:8;
	unsigned char EDIP_POS_Y:8;
	unsigned char EDIP_POS_Z:8;
	unsigned char LIFEBIT:1;			// Toggle every cycle!
	
	unsigned char AUTO_Action:1;
	unsigned char BREAK_Action:1;
	unsigned char ABORT_Action:1;
	
	unsigned char TOOL_Change:1;		// Tool changeing!
	unsigned char TOOL_OK:1;			// Toolcheck after change
	
	unsigned char ERROR_GLOB:1;			// Global ERROR-Flag

}structmachinestate;
volatile structmachinestate M_FLAGS[1];



#endif /* GLOBDEF_H_ */