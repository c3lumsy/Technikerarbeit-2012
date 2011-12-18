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
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "vnc1l.h"


int8_t usb_get_state(void)
{
	unsigned char unchecked = NULL;
		
		uart1_gets(STRING,CMD_CR);
		while (STRING[0] != NULL/* && unchecked == NULL*/)
		{
			uint8_t i= strlen(STRING);
			STRING[--i]='\0';			// 0x0D weg scheiden!
					
			if (!strcmp(STRING, ">"))
				{
/*					unchecked = 1;*/
					uart_puts("DONE");
					uart_putc(CMD_CR);	
				}
			if (!strcmp(STRING, "DD2"))
				{
					MSTATE->USB_CON=1;
					uart_puts("Disk Detected");
					uart_putc(CMD_CR);	
				}
			if (!strcmp(STRING, "DR2"))
				{
					MSTATE->USB_CON=0;
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
		if (MSTATE->USB_CON == 1)
			{
				PORTD |= _BV(6);
				PORTD &= ~_BV(5);
				return 1;
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

int8_t usb_open_file(char* f2open)
{
		uart1_putc(CMD_OPR);
		uart1_putc(CMD_SP);
		uart1_puts(f2open);
		uart1_putc(CMD_CR);
	
		if (usb_get_state() == 1)
		{
			MSTATE->USB_FILE_OPEN = 1;
			MSTATE->USB_SEK = 0;
			uart_puts("FILE ");
			uart_puts(FILENAME);
			uart_puts(" OPEN?");
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
	if (MSTATE->USB_FILE_OPEN == 1)
	{
		char c[3];
		itoa(byte,c,10);
		uart1_putc(CMD_RDF);
		uart1_putc(CMD_SP);	
		uart1_puts(c);
		uart1_putc(CMD_CR);
		MSTATE->USB_SEK += byte;
		_delay_ms(100);
		if (usb_get_state() == 1)
		{
			return 1;
		}	
	} 
	else
	{
		return 0;
	}
}

int8_t usb_get_command(void)
{
	if (usb_get_block(8) == 1)
	{
		uart1_gets(STRING);
		if (STRING[0] != NULL)
		{
			uart_puts(STRING);
		}
		switch (STRING[7])
			{
				case '(':
					uart_puts("Kommentar");

					while (STRING[1] != 0x29)
					{
						if (usb_get_block(1) == 1)
							{
								uart1_gets(STRING);
								uart_putc(STRING[0]);
					while (1)
					{
					}
							}
					}
					

					
					break;
				case 'M':
					uart_puts("M-Code");
					break;
				case 'G':
					uart_puts("G-Code");
					break;
				case 'T':
					uart_puts("Tool-Change");
					break;		
				default:
					break;
			}
		STRING[0]=0;
		return 1;
	} 
	else
	{
		return 0;
	}	
}