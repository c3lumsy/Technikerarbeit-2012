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
#include "gocnc.h"
#include "vnc1l.h"

void maschine_init()
{
	// CNC-Outputs / PA6 Ref-Switch
	DDRA |= _BV(7)|_BV(5)|_BV(4)|_BV(3)|_BV(2)|_BV(1)|_BV(0);   
	DDRD |= _BV(7)|_BV(6)|_BV(5);
	
	// Init UARTS
    uart_init( UART_BAUD_SELECT(UART_BAUD_RATE,F_CPU) ); 
	uart1_init( UART_BAUD_SELECT(UART1_BAUD_RATE,F_CPU) ); 

	// Init I2C
	i2c_init();
	
	// CNC ref
//	axis_ref();
	
	// IRQ enable
	// IRQ for Touchfield
 	EICRA |= (1<<ISC21); //INT2 bei steigender Flanke
 	EIMSK |= (1<<INT2);
    sei();
	
	// Set Maschinestates
	
	MSTATE->USB_CON =0;
	MSTATE->USB_FILE_OPEN = 0;	
	
	uart_puts("RDY!");

}

void edip_in()
{
	unsigned char c;

	i2c_start(0xDE);
	i2c_write(0x12);	// DC2
	i2c_write(0x01);	// L�nge der Daten
	i2c_write(0x53);	// S
	i2c_write(0x66);	// bcc

	i2c_rep_start(0xDF);
	c = i2c_readNak();
	i2c_stop();
	
	if (c == 0x06)
	{
		while (c != 0xFF)
		{
			uart_putc(c);
			i2c_rep_start(0xDF);
			c = i2c_readNak();
		}
		i2c_stop();
	}
}




int main(void)
{
	maschine_init();	// FIRST CMD!!!



	
	
	
	
	
	
	
// 	i2c_start(0xDE);
// 	i2c_write(0x11);	// DC1
// 	i2c_write(0x03);	// L�nge der Daten
// 	i2c_write(0x1B);	// ESC
// 	i2c_write(0x44);	// D
// 	i2c_write(0x4C);	// L
// 	i2c_write(0x11 + 0x03 + 0x1B + 0x44 + 0x4C);	// bcc
// 	i2c_stop();
// 
// 	_delay_us(200);
// 	
// 	i2c_start(0xDF);
// 	i2c_readAck();
// 	i2c_stop();
// 		
// 	_delay_us(200);
// 
// 	i2c_start(0xDE);
// 	i2c_write(0x11);	// DC1
// 	i2c_write(0x03);	// L�nge der Daten
// 	i2c_write(0x1B);	// ESC
// 	i2c_write(0x54);	// T
// 	i2c_write(0x45);	// E
// 	i2c_write(0x11 + 0x03 + 0x1B + 0x54 + 0x45);	// bcc
// 	i2c_stop();
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDF);
// 	i2c_readAck();
// 	i2c_stop();
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDE);
// 	i2c_write(0x11);	// DC1
// 	i2c_write(0x03);	// L�nge der Daten
// 	i2c_write(0x1B);	// ESC
// 	i2c_write(0x54);	// T
// 	i2c_write(0x56);	// V
// 	i2c_write(0xD9);	// bcc
// 	i2c_stop();
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDF);
// 	i2c_readAck();
// 	i2c_stop();	
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDE);
// 	i2c_write(0x11);	// DC1
// 	i2c_write(0x01);	// L�nge der Daten
// 	i2c_write(0x0D);	//
// 	i2c_write(0x11 + 0x01 + 0x0D);
// 	i2c_stop();
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDF);
// 	i2c_readAck();
// 	i2c_stop();
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDE);
// 	i2c_write(0x11);	// DC1
// 	i2c_write(0x01);	// L�nge der Daten
// 	i2c_write(0x0A);	//
// 	i2c_write(0x11 + 0x01 + 0x0A);
// 	i2c_stop();
// 	
// 	_delay_us(200);
// 	
// 	i2c_start(0xDF);
// 	i2c_readAck();
// 	i2c_stop();
// 	

while (1)
{
		if (MSTATE->USB_CON == 1)
			{
				PORTD |= _BV(6);
				PORTD &= ~_BV(5);
				MSTATE->USB_CON = 0;
			} 
		else
			{
				PORTD &= ~_BV(6);
				PORTD |= _BV(5);
				MSTATE->USB_CON = 1;
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
	cli();
	edip_in();
	sei();
}