/*************************************************************************
Title:    example program for the Interrupt controlled UART library
Author:   Peter Fleury <pfleury@gmx.ch>   http://jump.to/fleury
File:     $Id: test_uart.c,v 1.4 2005/07/10 11:46:30 Peter Exp $
Software: AVR-GCC 3.3
Hardware: any AVR with built-in UART, tested on AT90S8515 at 4 Mhz

DESCRIPTION:
          This example shows how to use the UART library uart.c

*************************************************************************/
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <avr/delay.h>

#include "uart.h"

/* define CPU frequency in Mhz here */
#ifndef F_CPU
#define F_CPU 20000000UL
#endif

/* define BAUD of UART0 and UART1 */
#define UART_BAUD_RATE      19200      
#define UART1_BAUD_RATE     9600

#define CMD_CR 0x0D
#define CMD_LF 0x0A

#define VNC_CMD_READ "rd "

#define FILE "text.txt"


// Definition Arrays
unsigned char input[128];

int main(void)
{ 
	      
	DDRD |= (1 << PD7)|(1 << PD6)|(1 << PD5);
	
    /*
     *  Initialize UART library, pass baudrate and AVR cpu clock
     *  with the macro 
     *  UART_BAUD_SELECT() (normal speed mode )
     *  or 
     *  UART_BAUD_SELECT_DOUBLE_SPEED() ( double speed mode)
     */
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU) ); 

    sei();		// Interrupt enable
	
	uart_puts("READY!");
	uart_putc(CMD_LF);
	uart1_putc(CMD_CR);

	int c = 0;

	while (1)
	{
		if (input[c] != NULL )
		{
			uart1_putc(input[c]);
			c++;
		}
		
		

		
		
		if (PIND & (1<<PD4))
		{
			uart1_puts("dir");
			uart1_putc(CMD_CR);
			_delay_ms(1000);		
		}
	}
}


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
