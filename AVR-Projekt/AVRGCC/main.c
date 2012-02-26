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
#include "i2cmaster.h"
#include "eDIP240.h"
#include "gocnc.h"
#include "vnc1l.h"

void maschine_init()
{
// CNC-Outputs / PA6 Ref-Switch
	DDRA |= _BV(7)|_BV(5)|_BV(4)|_BV(3)|_BV(2)|_BV(1)|_BV(0);   
	DDRD |= _BV(7)|_BV(6)|_BV(5);
	
// Delay for startup Display and VDRIVE2
	_delay_ms(250);
	
// Init UARTS
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU) ); 

// Init I2C
	i2c_init();
	
// IRQ defines

// IRQ for eDIP Touch PIN20
 	EICRA |= (1<<ISC21);				// INT2 on rising edge
 	EIMSK |= (1<<INT2);

// IRQ for CTC-Mode	

// 	TCCR1A = 0x00;						// no Port operation, kein PWM
// 	TCCR1B = (1<<WGM12);				// CTC1
// 	TIMSK0 |= (1<<OCIE1A);				// Compare Match A Interrupt Enable
// 	 
	 
	 
	 
	 
// IRQ enable	 
    sei();								// Interrupts enable
	
// Set Maschineflags
	M_FLAGS->ERROR_GLOB = 0;			// clear all errors	

	M_FLAGS->EDIP_ACTION = 0;			// no action from eDIP
	M_FLAGS->EDIP_CNC_HAND = 0;			// eDIP in Hand menu
	
		
	M_FLAGS->USB_CON =0;				// USB-Stick not connected till check
	M_FLAGS->USB_FILE_OPEN = 0;			// If Stick connected, first File is closed
	M_FLAGS->USB_FILE_EOF = 0;			// EndOfFile Flag cleared
	
	M_FLAGS->AXIS_v1 = 2000;			// Set Maschinespeed to default

// Display RDY MSG
	
	edip_msg(100);						// "Controller BEREIT!"
}


int main(void)
{
	maschine_init();					// first CMD in main-loop!

edip_put_POS(X.AxisRelPos,0,0);


while (1)
{
	
// Check if new action from eDIP avaliable ?
	if (M_FLAGS->EDIP_ACTION == 1)
	{
/*		uart_puts(eDIP_BUFFER);*/
		edip_check_input(eDIP_BUFFER);
	}

// Toggle LED´s on POLLIN-Board
		if (M_FLAGS->USB_CON == 1)
			{
				PORTD |= _BV(6);
				PORTD &= ~_BV(5);
				M_FLAGS->USB_CON = 0;
			} 
		else
			{
				PORTD &= ~_BV(6);
				PORTD |= _BV(5);
				M_FLAGS->USB_CON = 1;
			}			
}




	
// while (usb_get_state()!=1)
// {
//	char temp[8];
//	axis_move_interpol(0,0,-500,-2000,v_max);
// 	uart_puts(ltoa(AXIS[xAxis].AxisRelPos,temp,10));
// 	uart_puts("/");
// 	uart_puts(ltoa(AXIS[yAxis].AxisRelPos,temp,10));
// 	uart_putc(CMD_CR);
//	_delay_ms(1000);
	

//	axis_move_interpol(0,0,-250,-1000,v_max);
// 	uart_puts(ltoa(AXIS[xAxis].AxisRelPos,temp,10));
// 	uart_puts("/");
// 	uart_puts(ltoa(AXIS[yAxis].AxisRelPos,temp,10));
// 	uart_putc(CMD_CR);
//}
//
// while (MSTATE->USB_CON==1)
// {
// 	usb_open_file(FILENAME);
// 	while (MSTATE->USB_FILE_EOF != 1)
// 	{
// 		while (usb_get_command() != 1)
// 		{
// 		}
//  		_delay_us(10);
// 	}
// 	while (1)
// 	{
// 	}
// 	
// }
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

SIGNAL(INT2_vect)
{
// Interrupt from eDIP-Pin20
	
	cli();								// disable interrupts		
	edip_get_buffer(eDIP_BUFFER);		// save I2C-BUFFER
	M_FLAGS->EDIP_ACTION = 1;			// set Flag for main-loop
//	uart_puts(eDIP_BUFFER);				// only for debugging can be removed!
	sei();								// enable interrupts
	
}

SIGNAL(TIMER0_COMPA_vect)
{

}

SIGNAL(TWI_vect)
{
}	