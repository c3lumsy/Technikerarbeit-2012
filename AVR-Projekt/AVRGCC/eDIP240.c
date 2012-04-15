/************************************************************************
Title:			eDIP240-File
Version:		1.0
Created:		12/10/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.5.1
Hardware:		AVR ATmega1284P @ 18,432 MHz 

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de

				Open Source
************************************************************************/ 

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>

#include "globdef.h"
#include "gocnc.h"
#include "i2cmaster.h"
#include "uart.h"

#include "eDIP240.h"

uint8_t edip_put_CMD(uint8_t CMDTyp, char* daten)
{
	uint8_t bcc = 0;
	uint8_t n = 0;
	char c = 0;
	
	uint8_t i = strlen(daten)+1;	// check sting lengh
	
//	uart_puts(daten);				// debugging can removed
	
resend:
	i2c_start(ADDRESS + WRITE);		// open I2C to write
	i2c_write(CMDTyp);				// CMD-Type
	bcc += CMDTyp;					// add CMD-Type to checksum
	i2c_write(i);					// how many payload bytes
	bcc += i;						// add payload count to checksum
	for (n=0; daten[n] != 0; n++)	// loop till all payloads written
		{
			i2c_write(daten[n]);
			bcc += daten[n];		// add actual payload to checksum 
		}
	i2c_write(CR);
	bcc += CR;
	i2c_write(bcc);					// send checksum

	i2c_start(ADDRESS + READ);		// open I2C to read
	c = i2c_readNak();				// read one byte ACK or NAK
	i2c_stop();
	
	if ( c != ACK)					// if ACK ok, if NAK resend last command
		{
			goto resend;
		}
	else
		{
			return 1;					// return 1 if ACK
		}
}
uint8_t edip_msg(uint8_t msgnr)
{	
	uint8_t bcc = 0;
	uint8_t i = 0;
	char c[3];
		
	itoa(msgnr,c,10);

	bcc += (DC1 + 0x07 + 0x23 + 0x4D + 0x4E + 0x0D);
	bcc += c[0];
	bcc += c[1];
	bcc += c[2];
	
	i2c_start(ADDRESS + WRITE);

	i2c_write(DC1);					// DC1
	i2c_write(0x07);				// 7 byte payload
	i2c_write(0x23);				// #
	i2c_write(0x4D);				// M
	i2c_write(0x4E);				// N  Checksumme bis hier "0xDC" 

	i2c_write(c[0]);
	i2c_write(c[1]);
	i2c_write(c[2]);
	
	i2c_write(0x0D);				// CR zur übernahme!

	i2c_write(bcc);					// send checksum
	
	i2c_start(ADDRESS + READ);		// open I2C to read
	i = i2c_readNak();				// read one byte ACK or NAK
	i2c_stop();
	
	if ( i != ACK)					// if ACK ok, if NAK resend last command
		{
			return 0;
		}
	else
		{
			return 1;					// return 1 if ACK
		}
}

 uint8_t edip_put_pos(uint8_t x_showpos,uint8_t y_showpos,char dir,int32_t input)
{
uint8_t bcc = 0;					// Checksum clear!
uint8_t i = 6;						// Anzahl payload 8 byte grundgerüst
uint8_t n = 0;
unsigned char cc;					//
unsigned char x[4];
unsigned char y[4];
unsigned char c[8];					//
	
	utoa(x_showpos,x,10);
	i += strlen(x);

	utoa(y_showpos,y,10);
	i += strlen(y);

	ltoa(input,c,10);

	sprintf(c,"%7ld",+input);		// str = 8 Stellen mit Nullen vor der Zahl
	
 	i += strlen(c);
 	bcc += (DC1 + i + 0x23 + 0x5A + dir + 0x2C + 0x2C + CR);

	i2c_start(ADDRESS + WRITE);

	i2c_write(DC1);				// DC1
	i2c_write(i);				// payload count
	i2c_write(0x23);			// ESC
	i2c_write(0x5A);			// "Z"eichen
	i2c_write(dir);				// Value-Direction

	n=0;	
	while (x[n] != '\0')
		{
			i2c_write(x[n]);
			bcc += x[n];
			n++;
		}

 	i2c_write(0x2C);			// Komma
	
	n=0;
	while (y[n] != '\0')
		{
			i2c_write(y[n]);
			bcc += y[n];
			n++;
		}
		
 	i2c_write(0x2C);			// Komma

	n=0;
	while(c[n] != '\0')
		{
//			uart_putc(c[n]);			// debugging
			i2c_write(c[n]);
			bcc += c[n];				// add actual payload to checksum 
			n++;
		}
		
	i2c_write(CR);						// ! ENTER !
	

	i2c_write(bcc);						// send checksum

	i2c_rep_start(ADDRESS + READ);		// open I2C to read
	cc = i2c_readNak();					// read one byte ACK or NAK
	i2c_stop();
	
	if ( cc != ACK)						// if ACK ok, if NAK resend last command
		{
			return 0;
		}
	else
		{
			return 1;					// return 1 if ACK
		}
}


uint8_t edip_check_buffer()
{
	char c = 0;
 	uint8_t n = 0;
	 
	n = edip_put_CMD(DC2,"I");		// I für Buffergröße
	if (n == 1)
	{
		i2c_rep_start(ADDRESS + READ);
		c = i2c_readAck();
		
		if (c = DC2)
		{
			n = i2c_readAck();
 			for (uint8_t i=0; i<n ; i++)
			{
				c = i2c_readAck();
				uart_putc(c);
			}
			
			// hier noch die checksumme weghauen ;-)!	
			n = i2c_readAck();
			
			i2c_stop();
		}
		else
		{
			uart_puts("eDIP_CMD_ERROR");
		}
		return 1;
	}
	else
	{
		uart_puts("eDIP_SEND_ERROR");
 	}
	return 0;
}

uint8_t edip_get_buffer(char* daten)
{
	char c = 0;
 	uint8_t n = 0;
	 
	n = edip_put_CMD(DC2,"S");		// S für Bufferinhalt anfordern
	if (n == 1)
	{
		i2c_rep_start(ADDRESS + READ);
		c = i2c_readAck();
		
		if (c = DC1)
		{
			n = i2c_readAck();
 			for (uint8_t i=0; i<n ; i++)
			{
				daten[i] = i2c_readAck();
//				uart_putc(daten[i]);
			}
			
			// hier noch die checksumme weghauen ;-)!	
			n = i2c_readAck();
			
			i2c_stop();
		}
		else
		{
			uart_puts("eDIP_CMD_ERROR");
		}
		return 1;
	}
	else
	{
		uart_puts("eDIP_SEND_ERROR");
 	}
	return 0;
}

uint8_t edip_check_input(char* daten)
{	
	uart_puts(daten);
	if (daten[0] == ESC)
		{	
			switch (daten[1])
				{
				case 'A':
					uart_puts("A_CMD");
						switch (daten[2])
							{
							case 'A':
								if (daten[3] == 'A')
									{
										M_FLAGS->AUTO_Action = 0;
										M_FLAGS->ABORT_Action = 1;
										uart_puts("ABBRUCH!");
									}
								if (daten[3] == 'B')
									{
										M_FLAGS->BREAK_Action = 1;
										uart_puts("BREAK!");
									}
								if (daten[3] == 'S')
									{
										M_FLAGS->BREAK_Action = 0;
										M_FLAGS->AUTO_Action = 1;
										uart_puts("START!");
									}
								if (daten[3] == 'X')
									{
										usb_string_reset();
									}
								break;
							case 'T':
									M_FLAGS->GCODE_BOT = 0;
									M_FLAGS->GCODE_TOP = 1;
									uart_puts("TOP");
								break;		
							case 'B':
									M_FLAGS->GCODE_TOP = 0;
									M_FLAGS->GCODE_BOT = 1;
									uart_puts("BOT");
								break;
							default:
								break;
							}
						switch (daten[3])
						{
							case '1':	uart_puts("Z-AUF");							
								while (daten[3] != '0')
									{
										axis_move(zAxis,1,M_FLAGS->AXIS_v1);
									}
								break;
							case '2':	uart_puts("Z-AB");
								while (daten[3] != '0')
									{
										axis_move(zAxis,-1,M_FLAGS->AXIS_v1);
									}
								break;						
							case '3':	uart_puts("X-LINKS");
								while (daten[3] != '0')
									{
										axis_move(xAxis,-1,M_FLAGS->AXIS_v1);
									}
								break;
							case '4':	uart_puts("X-RECHTS");
								while (daten[3] != '0')
									{
										axis_move(xAxis,1,M_FLAGS->AXIS_v1);
									}
								break;
							case '5':	uart_puts("Y-ZURUECK");
								while (daten[3] != '0')
									{
										axis_move(yAxis,-1,M_FLAGS->AXIS_v1);
									}
								break;
							case '6':	uart_puts("Y-VOR");
								while (daten[3] != '0')
									{
										axis_move(yAxis,1,M_FLAGS->AXIS_v1);
									}
								break;
							case 'E':
									if (M_FLAGS->GCODE_BOT == 1)
										{
											strncpy(FILENAME,FILE_BE,6);
											M_FLAGS->GCODE_FILESET = 1;
										}
									if (M_FLAGS->GCODE_TOP == 1)
										{
											strncpy(FILENAME,FILE_TE,6);
											M_FLAGS->GCODE_FILESET = 1;
										}
									uart_puts("ETCH");
								break;
							case 'D':
									if (M_FLAGS->GCODE_BOT == 1)
										{
											strncpy(FILENAME,FILE_BD,6);
											
											M_FLAGS->GCODE_FILESET = 1;
										}
									if (M_FLAGS->GCODE_TOP == 1)
										{
											strncpy(FILENAME,FILE_TD,6);
											M_FLAGS->GCODE_FILESET = 1;
										}
									uart_puts("DRILL");
								break;
							case 'R':
								uart_puts("REF");
								
								if (axis_ref())
									{
										edip_msg(105);				// "Achsen referiert!"
									}
								break;
							case 'W':
								uart_puts("TOOL");
								if (axis_tool_length())
									{
										M_FLAGS->EDIP_NEWPOS = 1;
										edip_msg(106);				// "Werkzeug eingemessen!"
									}
								break;						
							default:
								break;
						}	
				case 'B':
					if (daten[3] == 1)					// Bargraph 1 Maschinenspeed
						{
							if (axis_set_speed(daten[4]))
								{
									uart_puts("Bar1Set");
								}
						}
					break;
				case 'E':
					if (daten[2] == 'Q')				// ERROR-Handling quit
						{
							M_FLAGS->ERROR_GLOB = 0;
							uart_puts("Quittiert");
						}
					if (daten[2] == 'T')
						{
							M_FLAGS->TOOL_Change = 1;
						}
					break;
				default:
					break;
				}
		uart_puts("SUCC");
			daten[0]='\0';								// STRING clear		
			M_FLAGS->EDIP_ACTION = 0;					// ActionFlag clear
			return 1;
		}
	else
		{
			return 0;		
		}
}