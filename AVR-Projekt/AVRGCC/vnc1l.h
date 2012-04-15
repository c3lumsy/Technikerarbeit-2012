/************************************************************************
Title:			VNC1L-Header-File
Version:		1.0
Created:		12/09/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.5.1
Hardware:		AVR ATmega1284P @ 18,432 MHz 

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de

				Open Source
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
#define CMD_CLF		0x0A
#define CMD_RDF		0x0B		// Reads the data of size from the current open file

volatile unsigned char STRING[64];




extern int8_t usb_get_state(void);

extern int8_t usb_open_file(unsigned char* f2open);
extern int8_t usb_close_file(unsigned char* f2close);

extern int8_t usb_get_command(void);
extern int8_t usb_get_block(int8_t byte);
extern void usb_set_sek(int16_t sektor);
extern int32_t usb_get_value();
extern void usb_string_reset();















#endif /* VNC1L_H_ */