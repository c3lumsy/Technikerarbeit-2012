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

#include <stdlib.h>
#include <avr/io.h>
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
	
	uint8_t i = strlen(daten);		// check sting lengh
	
	uart_puts(daten);				// debugging can removed
	
resend:
	i2c_start(ADDRESS + WRITE);		// open I2C to write
	i2c_write(CMDTyp);				// CMD-Type
	bcc += CMDTyp;					// add CMD-Type to checksum
	i2c_write(i);					// how many payload bytes
	bcc += i;						// add payloadcount to checksum
	for (n=0; daten[n] != 0; n++)	// loop till all payloads written
	{
		i2c_write(daten[n]);
		bcc += daten[n];			// add actual payload to checksum 
	}
	i2c_write(bcc);					// send checksum
	
	i2c_rep_start(ADDRESS + READ);	// open I2C to read
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
	if (daten[0] == ESC)
	{
		switch (daten[1])
			{
			case 'A':
				switch (daten[3])
					{
					case '0':
						uart_puts("STOPP!");
						break;
						
					case '1':
							uart_puts("Z-AUF");
							while (daten[3] != '0')
							{
								axis_move(zAxis,1,v_hand);
							}
						break;
					
					case '2':
							uart_puts("Z-AB");
							while (daten[3] != '0')
							{
								axis_move(zAxis,-1,v_hand);
							}
						break;
					
					case '3':
						uart_puts("X-LINKS");
							while (daten[3] != '0')
							{
								axis_move(xAxis,-1,v_hand);
							}
						break;
					
					case '4':
						uart_puts("X-RECHTS");
						while (daten[3] != '0')
							{
								axis_move(xAxis,1,v_hand);
							}
						break;
					
					case '5':
						uart_puts("Y-ZURUECK");
						while (daten[3] != '0')
							{
								axis_move(yAxis,-1,v_hand);
							}
						break;
					
					case '6':
						uart_puts("Y-VOR");
						while (daten[3] != '0')
							{
								axis_move(yAxis,1,v_hand);
							}
						break;
						
					case 'R':
						uart_puts("REF");
						axis_ref();
						break;
						
					case 'W':
						uart_puts("WZG-Teaching");
						break;
					}
				break;
			
			case 'B':
				// Bargraph 1 Maschinenspeed
				if (daten[3] == 1)
				{
	// setzt die geschwindigkeit
					
					//v_hand = 2500/daten[4];
				}
				break;
				
			case 'E':
				// ERROR-Handling quit
				if (daten[2] == 'Q')
				{
					M_FLAGS->ERROR_GLOB = 0;
				}
				break;
			}
			
			daten[0]='\0';				// STRING clear
			
	M_FLAGS->EDIP_ACTION = 0;			// ??? 
	return 1;
	
	}
	return 0;
}