/************************************************************************
Title:			VNC1L-File
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


#include "globdef.h"
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "vnc1l.h"
#include "gcode.h"

int8_t usb_get_state(void)
{
		uart1_gets(STRING,CMD_CR);
		while (STRING[0] != NULL/* && unchecked == NULL*/)
		{
			uint8_t i= strlen(STRING);
			STRING[--i]='\0';			// 0x0D weg scheiden!
					
			if (!strcmp(STRING, ">"))
				{
					M_FLAGS->USB_RDY=1;
					uart_puts("DONE");
					uart_putc(CMD_CR);
					return 1;	
				}
			if (!strcmp(STRING, "DD2"))
				{
					M_FLAGS->USB_CON=1;
					uart_puts("Disk Detected");
					uart_putc(CMD_CR);	
				}
			if (!strcmp(STRING, "DR2"))
				{
					M_FLAGS->USB_CON=0;
					uart_puts("Disk Removed");
					uart_putc(CMD_CR);	
				}
			if (!strcmp(STRING, "NU"))
				{
					uart_puts("No Upgrade");
					uart_putc(CMD_CR);	
				}
			if (!strcmp(STRING, "FI"))
				{
					uart_puts("File Invalid");
					uart_putc(CMD_CR);
				}
			if (!strcmp(STRING, "BC"))
				{
					uart_puts("Bad Command");
					uart_putc(CMD_CR);	
				}
			if (!strcmp(STRING, "CF"))
				{
					uart_puts("Command Failed");
					uart_putc(CMD_CR);	
				}
		 	STRING[0]='\0';
		}
		if (M_FLAGS->USB_CON == 1)
			{
				PORTD |= _BV(6);
				PORTD &= ~_BV(5);
			} 
		else
			{
				PORTD &= ~_BV(6);
				PORTD |= _BV(5);
				return 0;
			}
}/*END*/

int8_t usb_cmd_check(void)
{
}

int8_t usb_open_file(unsigned char* f2open)
{
		uart1_putc(CMD_OPR);
		uart1_putc(CMD_SP);
		uart1_puts(f2open);
		uart1_putc(CMD_CR);
		_delay_ms(100);		// Zeit zum antworten vom Stick...
		if (usb_get_state() == 1)
		{
			M_FLAGS->USB_FILE_OPEN = 1;
			M_FLAGS->USB_FILE_EOF = 0;
			M_FLAGS->USB_SEK = 0;
			uart_puts("FILE ");
			uart_puts(FILENAME);
			uart_putc(CMD_CR);
			return 1;
		}		
	else
	{
		return 0;
	}
}

void usb_get_dir(void)
{	
}

void usb_set_sek(int16_t sektor)
{
}

int8_t usb_get_block(int8_t byte)
{
	if (M_FLAGS->USB_FILE_OPEN == 1)
	{
		char c[3];
		itoa(byte,c,10);
		uart1_putc(CMD_RDF);
		uart1_putc(CMD_SP);	
		uart1_puts(c);
		uart1_putc(CMD_CR);
		M_FLAGS->USB_SEK += byte;
		_delay_ms(byte + 8);			// pro byte 1ms warten!
		uart1_gets(STRING,CMD_CR);
		return 1;	
	} 
	else
	{
		return 0;
	}
}

int8_t usb_get_command(void)
{	
	if (usb_get_block(1) == 1)			// GCODE N auswerten!
	{
		if (STRING[0] == 'N')
		{
			usb_get_block(6);			// Step-Number auslesen und wegschreiben!
			M_FLAGS->Last_GCODE = atoi(STRING);
			M_FLAGS->Last_GCODE /= 10;
			itoa(M_FLAGS->Last_GCODE,STRING,10);
			uart_putc(CMD_CR);
			uart_puts(STRING);
		}
		else
		{
			uart_puts("Code-ERROR!");	// Aufhängen wenn was falsches kommt...
			while (1)
			{
			}
			return 0;
		}
	}
	if (usb_get_block(1) == 1)
	{
		if (STRING[0] != NULL)
		{
		}
		switch (STRING[0])
			{
				case '(':							// Kommentare wegschneiden
						uart_puts("Kommentar");
 						while (STRING[0] != CMD_LF)
						{
							usb_get_block(1);
						}
					break;
				case 'M':							// M-Code extern auswerten
						gcode_m_check();
					break;
				case 'G':							// G-Code extern auswerten
						gcode_g_check();
					break;
				case 'T':							// T-Code extern auswerten
						gcode_t_check();
					break;		
				default:
						uart_puts(STRING);
						while (1)
						{
							uart_puts("Code-ERROR");
							uart_puts(STRING);
						}
					break;
			}
		return 1;
	} 
	else
	{
		return 0;
	}	
}

int32_t usb_get_value()
{
	unsigned char TEMP[8];
	uint8_t i=0;
	if (usb_get_block(1)==1)
	{
		while (STRING[0] != CMD_SP && STRING[0] != CMD_LF && STRING[0] != NULL )
		{
			if (STRING[0] != '.')
			{
				TEMP[i++]=STRING[0];
				usb_get_block(1);
			}
			else
			{
				usb_get_block(1);	
			}
		
		}
		TEMP[--i]='\0';
 		int32_t l = atol(TEMP);
	#ifdef PCmode 
		uart_puts(ltoa(l,TEMP,10));
	#endif

	return (l);
	}
}