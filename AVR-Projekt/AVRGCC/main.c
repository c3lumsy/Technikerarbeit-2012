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
#include "globdef.h"

#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "uart.h"
#include "gocnc.h"
#include "vnc1l.h"

void main(void)
{ 
	// CNC-Outputs / PA6 Ref-Switch
	DDRA |= _BV(7)|_BV(5)|_BV(4)|_BV(3)|_BV(2)|_BV(1)|_BV(0);   
	DDRD |= _BV(7)|_BV(6)|_BV(5);
	
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU) ); 

    sei();		// Interrupt enable
	MSTATE->USB_CON =0;
	MSTATE->USB_FILE_OPEN = 0;

while (1)
{
	
	usb_get_state();
	
// 		if (MSTATE->USB_FILE_OPEN != 1)
// 		{
// 			usb_open_file(FILENAME);
// 		}
// 		if (MSTATE->USB_FILE_OPEN == 1)
// 		{
// 			usb_get_command();
// 		}	

}




	
// while (usb_state()==0)
// {	
// 	uart_puts("x");
// 	usb_open_file(FILENAME);
// 	
// //	usb_get_command();
// // 					uart1_gets(STRING);
// // 				if (STRING[0] != NULL)
// // 				{
// // 					uart_puts(STRING);
// // 					STRING[0]=0;
// // 				}
// 
// 
// 
// }





}
	
// Interrupts
void UART1_RX_INT()
{
}

void UART0_RX_INT()
{
	char uart0_trans;
	
		uart0_trans = uart_getc();
		if (uart0_trans != 0)
		{
			uart1_putc(uart0_trans);
		}
}

