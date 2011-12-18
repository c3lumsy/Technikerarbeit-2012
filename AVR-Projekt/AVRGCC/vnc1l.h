/************************************************************************
Title:			VNC1L-Header-File
Version:		1.0
Created:		12/09/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284 @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 


#ifndef VNC1L_H_
#define VNC1L_H_

#define CMD_CR		0x0D		// Send CR, check if online
#define CMD_LF		0x0A		// Linefeed
#define CMD_SP		0x20		// Send space

#define CMD_E		'E'			// E for echo
#define CMD_ECS		0x11		// Switch to the extended command set
#define CMD_SCS		0x10		// Switch to the shortened command set
#define CMD_IPA		0x90		// Input numbers in ASCII
#define CMD_IPH		0x91		// Input numbers in HEX

#define CMD_DIR		0x01		// Lists the current directory
#define CMD_RD		0x04		// Read file <file>
#define CMD_OPR		0x0E		// Opens a file for reading with "RDF"
#define CMD_RDF		0x0B		// Reads the data of size from the current open file

volatile unsigned char STRING[64];


//volatile unsigned char FILENAME[12];
static unsigned char FILENAME[12] = "start.tap";

extern int8_t usb_get_state(void);
extern int8_t usb_open_file(char* f2open);
extern int8_t usb_get_command(void);
extern void usb_set_sek(int16_t sektor);















#endif /* VNC1L_H_ */