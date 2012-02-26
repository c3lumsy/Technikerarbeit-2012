/************************************************************************
Title:			eDIP240-File
Version:		1.0
Created:		12/10/2011

Project:		Technikerprojekt 2012
				CNC-Steuerung

Software:		AVR-GCC 4.1, AVR Libc 1.4
Hardware:		AVR ATmega1284P @20 MHz

Description:
						
Author:			Dennis Hohmann
Email:			hohmannd@web.de
Copyright:		(C)2012 Dennis Hohmann
************************************************************************/ 

#include <stdlib.h>
#include <avr/io.h>
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
	
	uint8_t i = strlen(daten);		// check sting lengh
	
//	uart_puts(daten);				// debugging can removed
	
resend:
	i2c_start(ADDRESS + WRITE);		// open I2C to write
	i2c_write(CMDTyp);				// CMD-Type
	bcc += CMDTyp;					// add CMD-Type to checksum
	i2c_write(i);					// how many payload bytes
	bcc += i;						// add payload count to checksum
	for (n=0; daten[n] != 0; n++)	// loop till all payloads written
		{
//uart_putc(daten[n]);				// debugging
			i2c_write(daten[n]);
			bcc += daten[n];		// add actual payload to checksum 
		}
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
uint8_t edip_put_POS(int32_t xpos,int16_t ypos,int16_t zpos)
{
	edip_put_text(15,15,'R',x);

}

uint8_t edip_put_text(uint8_t x_showpos,uint8_t y_showpos,char dir,uint32_t input)
{
	uint8_t bcc = 0;	//
	uint8_t i = 8;		// Anzahl payload 8 byte grundgerüst
	char c = 0;			//
	char* cx;

	i += strlen(cx);

	bcc += (DC1 + i + 0x23 + 0x5A + dir + 0x2C +x_showpos+y_showpos+ 0x2C + CR);	
	i2c_start(ADDRESS + WRITE);

	i2c_write(DC1);				// DC1
	i2c_write(i);				// payload count
	i2c_write(0x23);			// ESC
	i2c_write(0x5A);			// "Z"eichen
	i2c_write(dir);			// 
	
	itoa(x_showpos,cx,10);
	i2c_write(x_showpos);
	i2c_write(0x2C);
	i2c_write(y_showpos);
	i2c_write(0x2C);
	
	itoa(input,cx,10);
	
	for (uint8_t n=0; n != (i-8); n++)	// loop till all payloads written preload 8 vom grundgerüst
		{
uart_putc(cx[n]);				// debugging
			i2c_write(cx[n]);
			bcc += cx[n];		// add actual payload to checksum 
		}
		
		i2c_write(CR);	
//uart_putc(bcc);
	i2c_write(bcc);					// send checksum




	i2c_rep_start(ADDRESS + READ);		// open I2C to read
	c = i2c_readNak();				// read one byte ACK or NAK
uart_putc(c);
	i2c_stop();
	
	if ( c != ACK)					// if ACK ok, if NAK resend last command
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
				switch (daten[3])
					{
					case '0':
						uart_puts("STOPP!");
						break;
						
					case '1':
							uart_puts("Z-AUF");
							while (daten[3] != '0')
							{
								axis_move(zAxis,1,M_FLAGS->AXIS_v1);
							}
						break;
					
					case '2':
							uart_puts("Z-AB");
							while (daten[3] != '0')
							{
								axis_move(zAxis,-1,M_FLAGS->AXIS_v1);
							}
						break;
					
					case '3':
						uart_puts("X-LINKS");
							while (daten[3] != '0')
							{
								axis_move(xAxis,-1,M_FLAGS->AXIS_v1);
							}
						break;
					
					case '4':
						uart_puts("X-RECHTS");
						while (daten[3] != '0')
							{
								axis_move(xAxis,1,M_FLAGS->AXIS_v1);
							}
						break;
					
					case '5':
						uart_puts("Y-ZURUECK");
						while (daten[3] != '0')
							{
								axis_move(yAxis,-1,M_FLAGS->AXIS_v1);
							}
						break;
					
					case '6':
						uart_puts("Y-VOR");
						while (daten[3] != '0')
							{
								axis_move(yAxis,1,M_FLAGS->AXIS_v1);
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
					axis_set_speed(daten[4]);		// setzt die geschwindigkeit

				}
				break;
				
			case 'E':
				// ERROR-Handling quit
				if (daten[2] == 'Q')
				{
					uart_puts("Quittiert");
					M_FLAGS->ERROR_GLOB = 0;
				}
				break;
			}
			
			daten[0]='\0';				// STRING clear
			
	M_FLAGS->EDIP_ACTION = 0;			// ActionFlag clear
	return 1;
	
	}
	return 0;
}