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
	
// Definition Arrays
unsigned char input[128];

const char POS_OUT(int32_t POS)
{
	char out[8];
	unsigned char buffer[3];
	int32_t x= POS*((x_way_pr/x_step_pr)*10000);
	if (x>0)
	{out[0]=0x2B;} 
	else
	{out[0]=0x2D;}
	uint16_t y = (int32_t)x/10000;
	itoa(y,buffer,10);
	out[1]= buffer[0];
	out[2]= buffer[1];
	out[3]= buffer[2];
	out[4]=0x2C;
	uint16_t z = (int32_t)x%10000;
	itoa(z,buffer,10);
	out[5]= buffer[0];
	out[6]= buffer[1];
	out[7]= buffer[2];
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

	sX_IST = 5308;
	
#ifdef PCmode
	uart_puts("PCmode enable!");
	uart_puts("All Axis are in Position!");
	


uart_putc(POS_OUT(sX_IST));

#endif
axis_ref();
	
	while (1)
	{
	
		go_cnc();
		
	}

		
		
}
// Interrupts
void UART1_RX_INT(){
	
	char uart1_trans;
	int i = 0;

		uart1_trans = uart1_getc();
		if (uart1_trans != NULL)
		{
			if (i <= 128)
			{
				input[i] = uart1_trans;
				i++;
			}
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
