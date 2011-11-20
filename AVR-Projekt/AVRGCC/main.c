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
#include "gocnc.h"

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

void main(void)
{ 
	// CNC-Outputs / PA6 Ref-Switch
	DDRA |= (1 << PA7)|(1 << PA5)|(1 << PA4)|(1 << PA3)|(1 << PA2)|(1 << PA1)|(1 << PA0);    
	
	 
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

	
	uart_puts("All Axis are in Position!");
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
