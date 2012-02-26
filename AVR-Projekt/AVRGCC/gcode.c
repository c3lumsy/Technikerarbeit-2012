/************************************************************************
Title:			gCode-File
Version:		1.0
Created:		12/29/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284 @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/
#include "globdef.h"

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#include "uart.h"
#include "vnc1l.h"
#include "gocnc.h"

void gcode_cmd()
{
	int32_t i = 0;
	if (usb_get_block(1)==1)
	{
		if (STRING[0] == 'P')
			{	// Pasuenzeit auslesen!
				uart_puts("Warten: ");
			i =	usb_get_value();
				uart_puts("us");
			i /= 100;
			_delay_ms(i);
			}
		if (STRING[0] == 'Z')
			{	// Z-Pos auslesen!
		#ifdef PCmode 
				uart_puts("Z:");
		#endif	
			i =	usb_get_value();
			axis_move_single(zAxis,i,M_FLAGS->AXIS_v1);
			}
		if (STRING[0] == 'X')
			{	// X-Pos auslesen!
				int32_t j = 0;
		#ifdef PCmode 
				uart_puts("X/Y:");
				i =	usb_get_value();	
				uart_puts(" / ");
		#endif
				usb_get_block(1);	// SPACE weg schneiden!
				if (STRING[0] == 'Y')
					{	// Y-Pos auslesen!
						j =	usb_get_value();
					}
				axis_move_interpol(0,0,i,j,M_FLAGS->AXIS_v1);
			}
	}
}


void pufferleer()
{
				while (STRING[0] != CMD_LF)
			{
				usb_get_block(1);
			}
}

void gcode_g_check()
{
	if (usb_get_block(3)==1)
	{
		uint8_t i = atoi(STRING);
		switch (i)
		{
		case 0:
				uart_puts("Eilgang: ");
				gcode_cmd();
				
				pufferleer();
			break;
		case 1:
				uart_puts("Interpolation: ");
				gcode_cmd();
				
				pufferleer();
			
			break;
		case 4:
				uart_puts("Pause: ");
				gcode_cmd();
				
			pufferleer();
		
			break;
		case 20:
				M_FLAGS->GCODE_MM = 1;
				uart_puts("Maschineneinheit ist INCH");	
			break;
		case 21:
				M_FLAGS->GCODE_MM = 1;
				uart_puts("Maschineneinheit ist MM");	
			break;
		case 90:
				uart_puts("Absolute Coordinaten");
			break;
		default:
			break;
		}
	}				
	
}


void gcode_m_check()
{
	if (usb_get_block(3)==1)
	{
		uint8_t i = atoi(STRING);
		switch (i)
		{
		case 2:	
		// Programmende
				uart_putc(CMD_CR);
				uart_puts("EOF");
				M_FLAGS->USB_FILE_EOF = 1;
			break;
		case 3:	
		// Spindelstart CW
				uart_puts("Spindelstart CW");
			break;
		case 5:	
		// Spindelstop
				uart_puts("Spindelstop");
			break;
		case 6:	
		// Werkzeugwechsel
				uart_puts("Werkzeugwechsel");
				
				pufferleer();
			break;
		default:
			break;
		}
	}					
}

void gcode_t_check()
{
	if (usb_get_block(3)==1)
	{
		uint8_t i = atoi(STRING);
		switch (i)
		{
		case 1:
				uart_putc(CMD_CR);
				uart_puts("Werkzeug 1");
			break;
		case 2:
				uart_putc(CMD_CR);
				uart_puts("Werkzeug 2");	
			break;
		case 3:	
				uart_putc(CMD_CR);
				uart_puts("Werkzeug 3");
			break;
		default:
			break;
		}
	}				
	while (STRING[0] != CMD_LF)
	{
		usb_get_block(1);
	}	
}

void gcode_pos_check()
{
	if (usb_get_block(1)==1)
	{
		switch (STRING[0])
		{
		case 'X':
			break;
		case 'Z':
			break;
		}
	}
}

