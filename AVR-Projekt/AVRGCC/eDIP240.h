/************************************************************************
Title:			eDIP240-Header-File
Version:		1.0
Created:		12/10/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284 @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 

#ifndef EDIP240_H_
#define EDIP240_H_

#define ADDRESS	0xDE
#define READ	0x01
#define WRITE	0x00

// Handshakezeichen der EA eDIP240-Schnittstelle
#define DC1 0x11
#define DC2 0x12
#define ACK 0x06
#define NAK 0x15
#define ESC 0x1B


extern uint8_t edip_put_CMD(uint8_t CMDTyp, char* daten);
extern uint8_t edip_get_buffer(char * daten);
extern uint8_t edip_check_input(char * daten);
extern void edip_msg(uint8_t msgnr);







#endif /* EDIP240_H_ */