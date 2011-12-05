/************************************************************************
Title:			MAIN-File
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

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <avr/delay.h>

#include "globdef.h"
#include "uart.h"
#include "gocnc.h"



#define CMD_CR 0x0D
#define CMD_LF 0x0A

#define VNC_CMD_READ "rd "

#define FILE "text.txt"

const char POS_OUT(int32_t POS)
{
unsigned	char out[10];
	char buffer[3];
	int32_t x= POS/*(x_way_pr/x_step_pr)*10000*/;
	if (x>0)
	{out[0]='+';} 
	else
	{out[0]='-';}
	uint16_t y = (int32_t)x/10000;
	itoa(abs(y),buffer,10);
	if (buffer[1] == NULL)
		{
			out[1]= '0';
			out[2]= '0';
			out[3]= buffer[0];
		}				
		else
		{
			if (buffer[2] == NULL)
			{
				out[1]= '0';
				out[2]= buffer[0];
				out[3]= buffer[1];
			} 
			else
			{
			out[1]= buffer[0];
			out[2]= buffer[1];
			out[3]= buffer[2];			
			}
		}
		
	out[4]='.';
	uint16_t z = (int32_t)x%10000;
	itoa(abs(z),buffer,10);
	
		if (buffer[1] == NULL)
		{
			out[5]= '0';
			out[6]= '0';
			out[7]= buffer[0];
		}				
		else
		{
			if (buffer[2] == NULL)
			{
				out[5]= '0';
				out[6]= buffer[0];
				out[7]= buffer[1];
			} 
			else
			{
			out[5]= buffer[0];
			out[6]= buffer[1];
			out[7]= buffer[2];			
			}
		}
		out[8]='m';
		out[9]='m';
		
				uart_putc(out[0]);
				uart_putc(out[1]);
				uart_putc(out[2]);
				uart_putc(out[3]);
				uart_putc(out[4]);
				uart_putc(out[5]);
				uart_putc(out[6]);
				uart_putc(out[7]);
				uart_putc(out[8]);
				uart_putc(out[9]);
		return out;		
}

void POS_OUT_UART()
{
	uart_putc(CMD_CR);
	uart_puts("Z Pos: ");
	POS_OUT(sZ_IST);
	uart_putc(CMD_CR);
	uart_puts("X Pos: ");
	POS_OUT(sX_IST);
	uart_putc(CMD_CR);
	uart_puts("Y Pos: ");
	POS_OUT(sY_IST);	
}

void main(void)
{ 
	// CNC-Outputs / PA6 Ref-Switch
	DDRA |= _BV(7)|_BV(5)|_BV(4)|_BV(3)|_BV(2)|_BV(1)|_BV(0);
	    
	DDRD |= _BV(7)|_BV(6)|_BV(5);
	
	
	
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU) ); 

    sei();		// Interrupt enable
	
	uart_puts("READY!");
	uart_putc(CMD_LF);
	uart1_putc(CMD_CR);

#ifndef NOdelay
	axis_ref();
#endif

	
	
	go_cnc();
	
#ifdef PCmode
	uart_puts("PCmode enable!");
	uart_puts("All Axis are in Position!");
	POS_OUT_UART();

#endif
	while (1)
	{
	}
			
}

// Interrupts
void UART1_RX_INT(){
	
	char uart1_trans;
	
		uart1_trans = uart1_getc();
		if (uart1_trans != NULL)
		{

		}
}

void UART0_RX_INT(){
	char uart0_trans;
	
		uart0_trans = uart_getc();
		if (uart0_trans != NULL)
		{
		uart1_putc(uart0_trans);
		}
}
